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
#ifndef MEM_MOVE_DEVICE_HPP_
#define MEM_MOVE_DEVICE_HPP_

#include <future>
#include <thread>

#include "codegen/util/parallel-context.hpp"
#include "operators/operators.hpp"
#include "topology/affinity_manager.hpp"
#include "util/async_containers.hpp"

// void * make_mem_move_device(char * src, size_t bytes, int target_device,
// cudaStream_t strm);

class MemMoveDevice : public UnaryOperator {
 public:
  struct workunit {
    void *data;
    cudaEvent_t event;
    cudaStream_t strm;
  };

  struct MemMoveConf {
    AsyncQueueSPSC<workunit *>
        idle;  //_lockfree is slower and seems to have a bug
    AsyncQueueSPSC<workunit *> tran;

    std::future<void> worker;
    // std::thread               * worker   ;
    cudaStream_t strm;
    // cudaStream_t                strm2    ;

    cudaEvent_t *lastEvent;

    size_t slack;
    // cudaEvent_t               * events   ;
    // void                     ** old_buffs;
    size_t next_e;

    void *data_buffs;
    workunit *wus;
  };

  MemMoveDevice(Operator *const child, ParallelContext *const context,
                const vector<RecordAttribute *> &wantedFields, size_t slack,
                bool to_cpu)
      : UnaryOperator(child),
        context(context),
        wantedFields(wantedFields),
        slack(slack),
        to_cpu(to_cpu) {}

  virtual ~MemMoveDevice() { LOG(INFO) << "Collapsing MemMoveDevice operator"; }

  virtual void produce();
  virtual void consume(Context *const context, const OperatorState &childState);
  virtual bool isFiltering() const { return false; }

 private:
  ParallelContext *const context;

  const vector<RecordAttribute *> wantedFields;
  size_t device_id_var;
  size_t memmvconf_var;

  PipelineGen *catch_pip;
  llvm::Type *data_type;

  size_t slack;
  bool to_cpu;

  void open(Pipeline *pip);
  void close(Pipeline *pip);

  void catcher(MemMoveConf *conf, int group_id, exec_location target_dev);
};

#endif /* MEM_MOVE_DEVICE_HPP_ */
