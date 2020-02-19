/*
    Proteus -- High-performance query processing on heterogeneous hardware.

                            Copyright (c) 2017
        Data Intensive Applications and Systems Laboratory (DIAS)
                École Polytechnique Fédérale de Lausanne

                            All Rights Reserved.

    Permission to use, copy, modify and distribute this software and
    its documentation is hereby granted, provided that both the
    copyright notice and this permission notice appear in all copies of
    the software, derivative works or modified versions, and any
    portions thereof, and that both notices appear in supporting
    documentation.

    This code is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. THE AUTHORS
    DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER
    RESULTING FROM THE USE OF THIS SOFTWARE.
*/

#include "operators/router.hpp"

#include <cstring>
#include <memory/memory-manager.hpp>
#include <util/timing.hpp>

#include "expressions/expressions-generator.hpp"
#include "routing/routing-policy.hpp"
#include "util/jit/pipeline.hpp"

using namespace llvm;

void Router::produce_(ParallelContext *context) {
  generate_catch(context);

  context->popPipeline();

  catch_pip = context->removeLatestPipeline();

  // push new pipeline for the throw part
  context->pushPipeline();

  context->registerOpen(this, [this](Pipeline *pip) { this->open(pip); });
  context->registerClose(this, [this](Pipeline *pip) { this->close(pip); });

  getChild()->produce(context);
}

void Router::generate_catch(ParallelContext *context) {
  LLVMContext &llvmContext = context->getLLVMContext();
  // IRBuilder<> * Builder       = context->getBuilder    ();
  // BasicBlock  * insBB         = Builder->GetInsertBlock();
  // Function    * F             = insBB->getParent();

  // Builder->SetInsertPoint(context->getCurrentEntryBlock());
  Plugin *pg =
      Catalog::getInstance().getPlugin(wantedFields[0]->getRelationName());

  const ExpressionType *ptoid = pg->getOIDType();

  Type *oidType = ptoid->getLLVMType(llvmContext);

  // Value * subState   = ((ParallelContext *) context)->getSubStateVar();
  // Value * subStatePtr = context->CreateEntryBlockAlloca(F, "subStatePtr",
  // subState->getType());

  std::vector<Type *> param_typelist;
  for (auto field : wantedFields) {
    Type *wtype = field->getLLVMType(llvmContext);
    if (wtype == nullptr)
      wtype = oidType;  // FIXME: dirty hack for JSON inner lists

    param_typelist.push_back(wtype);
    need_cnt = need_cnt || (field->getOriginalType()->getTypeID() == BLOCK);
  }

  param_typelist.push_back(oidType);                // oid
  if (need_cnt) param_typelist.push_back(oidType);  // cnt

  // param_typelist.push_back(subStatePtr->getType());

  params_type = StructType::get(llvmContext, param_typelist);
  buf_size = context->getSizeOf(params_type);
  // context->SetInsertPoint(insBB);

  RecordAttribute tupleCnt(wantedFields[0]->getRelationName(), "activeCnt",
                           pg->getOIDType());  // FIXME: OID type for blocks ?
  RecordAttribute tupleIdentifier(wantedFields[0]->getRelationName(),
                                  activeLoop, pg->getOIDType());

  // Generate catch code
  auto p =
      context->appendParameter(PointerType::get(params_type, 0), true, true);
  context->setGlobalFunction();

  IRBuilder<> *Builder = context->getBuilder();
  BasicBlock *entryBB = Builder->GetInsertBlock();
  Function *F = entryBB->getParent();

  context->setCurrentEntryBlock(entryBB);

  BasicBlock *mainBB = BasicBlock::Create(llvmContext, "main", F);

  BasicBlock *endBB = BasicBlock::Create(llvmContext, "end", F);
  context->setEndingBlock(endBB);

  Builder->SetInsertPoint(entryBB);

  Value *params = Builder->CreateLoad(context->getArgument(p));

  map<RecordAttribute, ProteusValueMemory> variableBindings;

  for (size_t i = 0; i < wantedFields.size(); ++i) {
    Value *param = Builder->CreateExtractValue(params, i);

    // FIMXE: should we alse transfer this information ?
    variableBindings[*(wantedFields[i])] =
        context->toMem(param, context->createFalse());
  }
  Value *oid = Builder->CreateExtractValue(params, wantedFields.size());
  variableBindings[tupleIdentifier] =
      context->toMem(oid, context->createFalse());

  if (need_cnt) {
    Value *cnt = Builder->CreateExtractValue(params, wantedFields.size() + 1);

    variableBindings[tupleCnt] = context->toMem(cnt, context->createFalse());
  }

  Builder->SetInsertPoint(mainBB);

  OperatorState state{*this, variableBindings};
  getParent()->consume(context, state);

  Builder->CreateBr(endBB);

  Builder->SetInsertPoint(context->getCurrentEntryBlock());
  // Insert an explicit fall through from the current (entry) block to the
  // CondBB.
  Builder->CreateBr(mainBB);

  Builder->SetInsertPoint(context->getEndingBlock());
  // Builder->CreateRetVoid();
}

void *Router::acquireBuffer(int target, bool polling) {
  nvtxRangePushA("acq_buff");

  if (free_pool[target].empty() && polling) {
    nvtxRangePop();
    return nullptr;
  }

  std::unique_lock<std::mutex> lock(free_pool_mutex[target]);

  if (free_pool[target].empty()) {
    if (polling) {
      lock.unlock();
      nvtxRangePop();
      return nullptr;
    }
    nvtxRangePushA("cv_buff");
    // std::cout << "Blocking" << std::endl;
    eventlogger.log(this, log_op::EXCHANGE_PRODUCER_WAIT_START);
    free_pool_cv[target].wait(
        lock, [this, target]() { return !free_pool[target].empty(); });
    eventlogger.log(this, log_op::EXCHANGE_PRODUCER_WAIT_END);
    nvtxRangePop();
  }

  nvtxRangePushA("stack_buff");
  void *buff = free_pool[target].top();
  free_pool[target].pop();
  nvtxRangePop();

  lock.unlock();
  nvtxRangePushA("got_acq_buff");
  return buff;
}

void Router::releaseBuffer(int target, void *buff) {
  eventlogger.log(this, log_op::EXCHANGE_PRODUCE_PUSH_START);
  nvtxRangePop();
  // std::unique_lock<std::mutex> lock(ready_pool_mutex[target]);
  // eventlogger.log(this, log_op::EXCHANGE_PRODUCE);
  // ready_pool[target].emplace(buff);
  // ready_pool_cv[target].notify_one();
  // lock.unlock();
  ready_fifo[target].push(buff);
  nvtxRangePop();
  eventlogger.log(this, log_op::EXCHANGE_PRODUCE_PUSH_END);
}

void Router::freeBuffer(int target, void *buff) {
  nvtxRangePushA("waiting_to_release");
  std::unique_lock<std::mutex> lock(free_pool_mutex[target]);
  nvtxRangePop();
  free_pool[target].emplace(buff);
  free_pool_cv[target].notify_one();
  lock.unlock();
}

bool Router::get_ready(int target, void *&buff) {
  // // while (ready_pool[target].empty() && remaining_producers > 0);

  // std::unique_lock<std::mutex> lock(ready_pool_mutex[target]);

  // if (ready_pool[target].empty()){
  //     eventlogger.log(this, log_op::EXCHANGE_CONSUMER_WAIT_START);
  //     ready_pool_cv[target].wait(lock, [this, target](){return
  //     !ready_pool[target].empty() || (ready_pool[target].empty() &&
  //     remaining_producers <= 0);}); eventlogger.log(this,
  //     log_op::EXCHANGE_CONSUMER_WAIT_END  );
  // }

  // if (ready_pool[target].empty()){
  //     assert(remaining_producers == 0);
  //     lock.unlock();
  //     return false;
  // }

  // buff = ready_pool[target].front();
  // ready_pool[target].pop();

  // lock.unlock();
  // return true;

  return ready_fifo[target].pop(buff);
}

void Router::fire(int target, PipelineGen *pipGen) {
  nvtxRangePushA((pipGen->getName() + ":" + std::to_string(target)).c_str());

  eventlogger.log(this, log_op::EXCHANGE_CONSUME_OPEN_START);

  // size_t packets = 0;
  // time_block t("Xchange pipeline (target=" + std::to_string(target) + "): ");

  eventlogger.log(this, log_op::EXCHANGE_CONSUME_OPEN_END);

  const auto &cu = aff->getAvailableCU(target);
  // set_exec_location_on_scope d(cu);
  auto exec_affinity = cu.set_on_scope();
  auto pip = pipGen->getPipeline(target);
  std::this_thread::yield();  // if we remove that, following opens may allocate
                              // memory to wrong socket!
  void *mem;
  {
    assert(buf_size);
    mem = MemoryManager::mallocPinned(buf_size * slack);
    for (int j = 0; j < slack; ++j) {
      freeBuffer(target, ((char *)mem) + j * buf_size);
    }
  }
  nvtxRangePushA(
      (pipGen->getName() + ":" + std::to_string(target) + "open").c_str());
  pip->open();
  nvtxRangePop();
  eventlogger.log(this, log_op::EXCHANGE_CONSUME_OPEN_START);

  eventlogger.log(this, log_op::EXCHANGE_CONSUME_OPEN_END);

  {
    // time_block t("Texchange consume (target=" + std::to_string(target) + "):
    // ");
    do {
      void *p;
      if (!get_ready(target, p)) break;
      // ++packets;
      nvtxRangePushA((pipGen->getName() + ":cons").c_str());

      // if (fanout > 1){
      //     int node;
      //     int r = move_pages(0, 1, (void **) p, nullptr, &node,
      //     MPOL_MF_MOVE); assert((target & 1) == node); assert((target & 1) ==
      //     (sched_getcpu() & 1)); if (r != 0 || node < 0) {
      //         std::cout << *((void **)p) << " " << target << " " << node << "
      //         " << r << " " << strerror(-node); std::cout << std::endl;
      //     }
      // }
      {
        //          time_block t{"Tfire_" + std::to_string(pip->getGroup()) +
        //          "_" + std::to_string((uintptr_t) this) + ": "};
        pip->consume(0, p);
      }
      nvtxRangePop();

      freeBuffer(target, p);  // FIXME: move this inside the generated code

      // std::this_thread::yield();
    } while (true);
  }

  eventlogger.log(this, log_op::EXCHANGE_CONSUME_CLOSE_START);

  nvtxRangePushA(
      (pipGen->getName() + ":" + std::to_string(target) + "close").c_str());
  pip->close();
  nvtxRangePop();

  MemoryManager::freePinned(mem);
  // std::cout << "Xchange pipeline packets (target=" << target << "): " <<
  // packets << std::endl;

  nvtxRangePop();

  eventlogger.log(this, log_op::EXCHANGE_CONSUME_CLOSE_END);
}

extern "C" {
void *acquireBuffer(int target, Router *xch) {
  return xch->acquireBuffer(target, false);
}

void *try_acquireBuffer(int target, Router *xch) {
  return xch->acquireBuffer(target, true);
}

void releaseBuffer(int target, Router *xch, void *buff) {
  return xch->releaseBuffer(target, buff);
}

void freeBuffer(int target, Router *xch, void *buff) {
  return xch->freeBuffer(target, buff);
}
}

std::unique_ptr<routing::RoutingPolicy> Router::getPolicy() const {
  switch (policy_type) {
    case RoutingPolicy::HASH_BASED: {
      assert(hashExpr.has_value());
      return std::make_unique<routing::HashBased>(fanout, hashExpr.value());
    }
    case RoutingPolicy::LOCAL: {
      return std::make_unique<routing::Local>(
          fanout, wantedFields, new AffinityPolicy(fanout, aff.get()));
    }
    case RoutingPolicy::RANDOM: {
      return std::make_unique<routing::Random>(fanout);
    }
  }
  assert(false);
}

void Router::consume(ParallelContext *const context,
                     const OperatorState &childState) {
  // Generate throw code
  LLVMContext &llvmContext = context->getLLVMContext();
  IRBuilder<> *Builder = context->getBuilder();
  BasicBlock *insBB = Builder->GetInsertBlock();
  Function *F = insBB->getParent();

  Type *charPtrType = Type::getInt8PtrTy(llvmContext);

  Value *params = UndefValue::get(params_type);

  Plugin *pg =
      Catalog::getInstance().getPlugin(wantedFields[0]->getRelationName());

  for (size_t i = 0; i < wantedFields.size(); ++i) {
    ProteusValueMemory mem_valWrapper = childState[*(wantedFields[i])];

    params = Builder->CreateInsertValue(
        params, Builder->CreateLoad(mem_valWrapper.mem), i);
  }

  BasicBlock *tryBB = BasicBlock::Create(llvmContext, "tryAcq", F);
  Builder->CreateBr(tryBB);
  Builder->SetInsertPoint(tryBB);

  auto r = getPolicy()->evaluate(context, childState);

  r.target->setName("target");
  auto target =
      Builder->CreateTruncOrBitCast(r.target, Type::getInt32Ty(llvmContext));

  RecordAttribute tupleIdentifier(wantedFields[0]->getRelationName(),
                                  activeLoop, pg->getOIDType());

  ProteusValueMemory mem_oidWrapper = childState[tupleIdentifier];
  params = Builder->CreateInsertValue(
      params, Builder->CreateLoad(mem_oidWrapper.mem), wantedFields.size());

  if (need_cnt) {
    RecordAttribute tupleCnt(wantedFields[0]->getRelationName(), "activeCnt",
                             pg->getOIDType());  // FIXME: OID type for blocks ?

    ProteusValueMemory mem_cntWrapper = childState[tupleCnt];
    params = Builder->CreateInsertValue(params,
                                        Builder->CreateLoad(mem_cntWrapper.mem),
                                        wantedFields.size() + 1);
  }

  Value *exchangePtr =
      ConstantInt::get(llvmContext, APInt(64, ((uint64_t)this)));
  Value *exchange = Builder->CreateIntToPtr(exchangePtr, charPtrType);

  vector<Value *> kernel_args{target, exchange};

  Function *acquireBuffer;
  if (r.may_retry)
    acquireBuffer = context->getFunction("try_acquireBuffer");
  else
    acquireBuffer = context->getFunction("acquireBuffer");

  Value *param_ptr = Builder->CreateCall(acquireBuffer, kernel_args);

  Value *null_ptr =
      ConstantPointerNull::get(((PointerType *)param_ptr->getType()));
  Value *is_null = Builder->CreateICmpEQ(param_ptr, null_ptr);

  BasicBlock *contBB = BasicBlock::Create(llvmContext, "cont", F);
  Builder->CreateCondBr(is_null, tryBB, contBB);

  Builder->SetInsertPoint(contBB);

  param_ptr =
      Builder->CreateBitCast(param_ptr, PointerType::get(params->getType(), 0));

  Builder->CreateStore(params, param_ptr);

  Function *releaseBuffer = context->getFunction("releaseBuffer");
  kernel_args.push_back(Builder->CreateBitCast(param_ptr, charPtrType));

  Builder->CreateCall(releaseBuffer, kernel_args);
}

void Router::open(Pipeline *pip) {
  std::lock_guard<std::mutex> guard(init_mutex);

  if (firers.empty()) {
    free_pool = new std::stack<void *>[fanout];
    free_pool_mutex = new std::mutex[fanout];
    free_pool_cv = new std::condition_variable[fanout];
    ready_fifo = new AsyncQueueMPSC<void *>[fanout];
    assert(free_pool);

    for (int i = 0; i < fanout; ++i) {
      ready_fifo[i].reset();
    }

    eventlogger.log(this, log_op::EXCHANGE_INIT_CONS_START);
    remaining_producers = producers;
    for (int i = 0; i < fanout; ++i) {
      firers.emplace_back(&Router::fire, this, i, catch_pip);
    }
    eventlogger.log(this, log_op::EXCHANGE_INIT_CONS_END);
  }
}

void Router::close(Pipeline *pip) {
  // time_block t("Tterm_exchange: ");

  int rem = --remaining_producers;
  assert(rem >= 0);

  // for (int i = 0 ; i < fanout ; ++i) ready_pool_cv[i].notify_all();

  if (rem == 0) {
    for (int i = 0; i < fanout; ++i) ready_fifo[i].close();

    eventlogger.log(this, log_op::EXCHANGE_JOIN_START);
    nvtxRangePushA("Exchange_waiting_to_close");
    for (auto &t : firers) t.join();
    nvtxRangePop();
    eventlogger.log(this, log_op::EXCHANGE_JOIN_END);
    firers.clear();

    delete[] free_pool;
    delete[] free_pool_mutex;
    delete[] free_pool_cv;
    delete[] ready_fifo;
  }
}
