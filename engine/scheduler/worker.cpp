/*
                  AEOLUS - In-Memory HTAP-Ready OLTP Engine

                             Copyright (c) 2019-2019
           Data Intensive Applications and Systems Laboratory (DIAS)
                   Ecole Polytechnique Federale de Lausanne

                              All Rights Reserved.

      Permission to use, copy, modify and distribute this software and its
    documentation is hereby granted, provided that both the copyright notice
  and this permission notice appear in all copies of the software, derivative
  works or modified versions, and any portions thereof, and that both notices
                      appear in supporting documentation.

  This code is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE. THE AUTHORS AND ECOLE POLYTECHNIQUE FEDERALE DE LAUSANNE
DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE
                             USE OF THIS SOFTWARE.
*/

#include "scheduler/worker.hpp"

#include <assert.h>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <limits>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "scheduler/affinity_manager.hpp"
#include "storage/table.hpp"
#include "transactions/transaction_manager.hpp"

#define NUM_ITER 5000000

namespace scheduler {

void WorkerPool::shutdown_manual() {
  bool e_false_s = false;
  if (this->terminate.compare_exchange_strong(e_false_s, true)) {
    // cv.notify_all();
    std::cout << "Manual worker shutdown." << std::endl;
    for (auto& worker : workers) {
      worker.second.second->terminate = true;
    }
  }
}

// TODO: check the reader lock on master and fall back thingy
inline uint64_t __attribute__((always_inline))
calculate_delta_ver(uint64_t txn_id, uint64_t start_time) {
  // global_conf::time_master_switch_ms;
  // assuming start time is nanosec  so convert to millisec
  // start_time = start_time / 1000000;
  // uint di = start_time / global_conf::time_master_switch_ms;
  // uint mod = start_time % global_conf::time_master_switch_ms;

  // txn_id = ((txn_id << 8) >> 8);  // remove the worker_id

  uint64_t duration =
      ((txn_id & 0x00FFFFFFFFFFFFFF) -
       (start_time & 0x00FFFFFFFFFFFFFF));  ///
                                            // 1000000000;  // nanosec

  // ushort curr_master = (duration / global_conf::num_master_versions) %
  //                     global_conf::num_master_versions;

  // check which master should it be in. then check if there is a lock
  // on that master version

  // return curr_master;
  // std::cout << duration << std::endl;
  // return duration >> 6;  //(duration / global_conf::num_delta_storages);

  return duration >> 20;  // 1000000L;
}

void Worker::run() {
  // std::cout << "[WORKER] Worker (TID:" << (int)(this->id)
  //          << "): Assigining Core ID:" << this->exec_core->id << std::endl;

  AffinityManager::getInstance().set(this->exec_core);

  WorkerPool* pool = &WorkerPool::getInstance();
  txn::TransactionManager* txnManager = &txn::TransactionManager::getInstance();
  storage::Schema* schema = &storage::Schema::getInstance();
  void* txn_mem = pool->txn_bench->get_query_struct_ptr();

  curr_delta = 0;
  prev_delta = 0;
  this->txn_start_time =
      std::chrono::system_clock::now();  // txnManager->txn_start_time;

  this->txn_start_tsc = txnManager->get_next_xid(this->id);
  uint64_t tx_st = txnManager->txn_start_time;

  this->curr_txn = txnManager->get_next_xid(this->id);
  this->prev_delta = this->curr_delta;
  this->curr_delta = calculate_delta_ver(this->curr_txn, tx_st);
  schema->add_active_txn(curr_delta % global_conf::num_delta_storages,
                         this->curr_delta, this->id);

  state = RUNNING;
  while (!terminate) {
    // check which master i should be on.

    if (pause) {
      state = PAUSED;

      while (pause && !terminate) {
        std::this_thread::sleep_for(std::chrono::microseconds(50));
      }
      state = RUNNING;
      continue;
    }
    if (this->curr_master != txnManager->current_master) {
      std::cout << "Worker->" << (uint)this->id
                << " : switching master from: " << this->curr_master << " to "
                << txnManager->current_master << std::endl;
    }

    this->curr_master = txnManager->current_master;
    this->curr_txn = txnManager->get_next_xid(this->id);
    this->prev_delta = this->curr_delta;
    this->curr_delta = calculate_delta_ver(this->curr_txn, tx_st);

    ushort curr_delta_id = curr_delta % global_conf::num_delta_storages;
    ushort prev_delta_id = prev_delta % global_conf::num_delta_storages;

    if (prev_delta != curr_delta) {  // && curr_delta_id != prev_delta_id
      schema->switch_delta(prev_delta_id, curr_delta_id, curr_delta, this->id);

    }  // else didnt switch.

    // custom query coming in.
    // if (this->id == 0) {
    //   std::function<bool(uint64_t)> task;
    //   std::unique_lock<std::mutex> lock(pool->m);
    //   if (!pool->tasks.empty()) {
    //     //    NO-WAIT -> If task in queue, exec ELSE gen/exec txn
    //     //    pool->cv.wait(
    //     //    lock, [this, pool] { return this->terminate ||
    //     //    !pool->tasks.empty(); });
    //     task = std::move(pool->tasks.front());
    //     pool->tasks.pop();
    //     std::cout << "[WORKER] Worker (TID:" << (int)(this->id)
    //               << ") Got a Task!" << std::endl;
    //     if (task(this->curr_txn))
    //       num_commits++;
    //     else
    //       num_aborts++;
    //   }
    // }

    pool->txn_bench->gen_txn((uint)this->id, txn_mem);

    // if (txnManager->executor.execute_txn(
    //         c, curr_txn, txnManager->current_master, curr_delta_id))
    if (pool->txn_bench->exec_txn(txn_mem, curr_txn, this->curr_master,
                                  curr_delta_id))
      num_commits++;
    else
      num_aborts++;

    num_txns++;

    if (num_txns == NUM_ITER) break;
  }
  state = TERMINATED;

  txn_end_time = std::chrono::system_clock::now();
  // std::cout << "Worker exited: " << (int)(this->id) << std::endl;
  /*if (pool->txn_bench && pool->txn_bench->name.compare("TPCC") == 0) {
    bench::TPCC* tpcc_bench = (bench::TPCC*)pool->txn_bench;
    tpcc_bench->verify_consistency((uint)this->id);
  }*/
}

std::vector<uint64_t> WorkerPool::get_active_txns() {
  std::vector<uint64_t> ret = std::vector<uint64_t>(this->size());

  for (auto& wr : workers) {
    ret.push_back(wr.second.second->curr_txn);
  }

  return ret;
}

uint64_t WorkerPool::get_min_active_txn() {
  uint64_t min_epoch = std::numeric_limits<uint64_t>::max();

  for (auto& wr : workers) {
    if (wr.second.second->curr_delta < min_epoch) {
      min_epoch = wr.second.second->curr_delta;
    }
  }

  return min_epoch;
}

uint64_t WorkerPool::get_max_active_txn() {
  uint64_t max_epoch = std::numeric_limits<uint64_t>::min();

  for (auto& wr : workers) {
    if (wr.second.second->curr_delta > max_epoch) {
      max_epoch = wr.second.second->curr_delta;
    }
  }

  return max_epoch;
}

bool WorkerPool::is_all_worker_on_master_id(ushort master_id) {
  for (auto& wr : workers) {
    if (wr.second.second->curr_master != master_id) {
      return false;
    }
  }

  return true;
}

void WorkerPool::pause() {
  for (auto& wr : workers) {
    wr.second.second->pause = true;
  }

  for (auto& wr : workers) {
    while (wr.second.second->state != PAUSED) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }
}
void WorkerPool::resume() {
  for (auto& wr : workers) {
    wr.second.second->pause = false;
  }

  for (auto& wr : workers) {
    while (wr.second.second->state == PAUSED) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }
}

void WorkerPool::print_worker_stats(bool global_only) {
  std::cout << "------------ WORKER STATS ------------" << std::endl;
  double tps = 0;
  double num_commits = 0;
  double num_aborts = 0;
  double num_txns = 0;

  const auto& vec = scheduler::Topology::getInstance().getCpuNumaNodes();
  int num_sockets = vec.size();

  std::vector<double> socket_tps(num_sockets, 0.0);

  for (auto it = workers.begin(); it != workers.end(); ++it) {
    // std::cout << " " << it->first << ":" << it->second;
    Worker* tmp = it->second.second;
    std::chrono::duration<double> diff =
        tmp->txn_end_time - tmp->txn_start_time;

    if (!global_only) {
      std::cout << "Worker-" << (int)(tmp->id)
                << "(core_id: " << tmp->exec_core->id << ")" << std::endl;
      std::cout << "\t# of txns\t" << (tmp->num_txns / 1000000.0) << " M"
                << std::endl;
      std::cout << "\t# of commits\t" << (tmp->num_commits / 1000000.0) << " M"
                << std::endl;
      std::cout << "\t# of aborts\t" << (tmp->num_aborts / 1000000.0) << " M"
                << std::endl;
      std::cout << "\tTPS\t\t" << (tmp->num_txns / 1000000.0) / diff.count()
                << " mTPS" << std::endl;
    }
    socket_tps[tmp->id / NUM_CORE_PER_SOCKET] +=
        (tmp->num_txns / 1000000.0) / diff.count();

    tps += (tmp->num_txns / 1000000.0) / diff.count();
    num_commits += (tmp->num_commits / 1000000.0);
    num_aborts += (tmp->num_aborts / 1000000.0);
    num_txns += (tmp->num_txns / 1000000.0);
  }

  std::cout << "---- SOCKET ----" << std::endl;
  int i = 0;
  for (const auto& stps : socket_tps) {
    std::cout << "\tSocket-" << i++ << ": TPS\t\t" << stps << " mTPS"
              << std::endl;
  }

  std::cout << "---- GLOBAL ----" << std::endl;
  std::cout << "\t# of txns\t" << num_txns << " M" << std::endl;
  std::cout << "\t# of commits\t" << num_commits << " M" << std::endl;
  std::cout << "\t# of aborts\t" << num_aborts << " M" << std::endl;
  std::cout << "\tTPS\t\t" << tps << " mTPS" << std::endl;

  std::cout << "------------ END WORKER STATS ------------" << std::endl;
  proc_completed = true;
}

void WorkerPool::init(bench::Benchmark* txn_bench) {
  std::cout << "[WorkerPool] Init" << std::endl;

  if (txn_bench == nullptr) {
    this->txn_bench = new bench::Benchmark();
  } else {
    this->txn_bench = txn_bench;
  }
  std::cout << "[WorkerPool] TXN Bench: " << this->txn_bench->name << std::endl;
  // txn_bench->exec_txn(txn_bench->gen_txn(0));
  // std::cout << "[WorkerPool] TEST TXN" << std::endl;
}

// Hot Plug
void WorkerPool::add_worker(core* exec_location) {
  assert(workers.find(exec_location->id) == workers.end());
  Worker* wrkr = new Worker(worker_counter++, exec_location);
  std::thread* thd = new std::thread(&Worker::run, wrkr);

  workers.emplace(std::make_pair(exec_location->id, std::make_pair(thd, wrkr)));
}

// Hot Plug
void WorkerPool::remove_worker(core* exec_location) {
  auto get = workers.find(exec_location->id);
  assert(get != workers.end());
  get->second.second->terminate = true;
  // TODO: remove from the vector too?
}

void WorkerPool::start_workers(int num_workers) {
  std::cout << "[WorkerPool] start_workers -- requested_num_workers: "
            << num_workers << std::endl;
  std::vector<core>* worker_cores =
      Topology::getInstance().get_worker_cores(num_workers);

  std::cout << "[WorkerPool] Number of Workers (AUTO) " << num_workers
            << std::endl;

  /* FIX ME:HACKED because we dont have topology returning specific number of
   * cores, this will be fixed when the elasticity and container stuff. until
   * then, just manually limit the number of wokrers
   */

  // interleave
  int i = 0;
  int j = 0;
  for (auto& exec_core : *worker_cores) {
    if (j % 2 == 1) {
      Worker* wrkr = new Worker(worker_counter++, &exec_core);
      std::thread* thd = new std::thread(&Worker::run, wrkr);

      workers.emplace(std::make_pair(exec_core.id, std::make_pair(thd, wrkr)));
      if (++i == num_workers) {
        break;
      }
    }
    j++;
  }

  // second socket ibm machine
  // int i = 0;
  // int j = 0;
  // for (auto& exec_core : *worker_cores) {
  //   ++j;
  //   // FIXME
  //   if (j < 64) continue;
  //   Worker* wrkr = new Worker(worker_counter++, &exec_core);
  //   std::thread* thd = new std::thread(&Worker::run, wrkr);

  //   workers.emplace(std::make_pair(exec_core.id, std::make_pair(thd, wrkr)));
  //   if (++i == num_workers) {
  //     break;
  //   }
  // }
}

template <class F, class... Args>
std::future<typename std::result_of<F(Args...)>::type> WorkerPool::enqueueTask(
    F&& f, Args&&... args) {
  using packaged_task_t =
      std::packaged_task<typename std::result_of<F(Args...)>::type()>;

  std::shared_ptr<packaged_task_t> task(new packaged_task_t(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...)));

  auto res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(m);
    tasks.emplace([task]() { (*task)(); });
  }

  cv.notify_one();
  return res;
}

/*
<< operator for Worker pool to print stats of worker pool.
format:
*/
// std::ostream& operator<<(std::ostream& out, const WorkerPool& topo) {
//   out << "NOT IMPLEMENTED\n";
//   return out;
// }

}  // namespace scheduler
