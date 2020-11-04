/*
     Proteus -- High-performance query processing on heterogeneous hardware.

                            Copyright (c) 2019
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

#ifndef TRANSACTION_MANAGER_HPP_
#define TRANSACTION_MANAGER_HPP_

#include <unistd.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <platform/util/timing.hpp>

#include "oltp/common/constants.hpp"
#include "oltp/execution/worker.hpp"
#include "oltp/storage/table.hpp"
#include "oltp/transaction/concurrency-control/concurrency-control.hpp"
#include "oltp/transaction/txn_utils.hpp"

namespace txn {

class TransactionManager {
 public:
  static inline TransactionManager &getInstance() {
    static TransactionManager instance;
    return instance;
  }
  TransactionManager(TransactionManager const &) = delete;  // Don't Implement
  void operator=(TransactionManager const &) = delete;      // Don't implement

  // TODO: move the following to snapshot manager
  bool snapshot() {
    time_block t("[TransactionManger] snapshot_: ");
    /* FIXME: full-barrier is needed only to get num_records of each relation
     *        which is implementation specific. it can be saved by the
     *        storage-layer whenever it sees the master-version different from
     *        the last or previous write.
     * */

    scheduler::WorkerPool::getInstance().pause();
    ushort snapshot_master_ver = this->switch_master();
    storage::Schema::getInstance().twinColumn_snapshot(this->get_next_xid(0),
                                                       snapshot_master_ver);
    // storage::Schema::getInstance().snapshot(this->get_next_xid(0), 0);
    scheduler::WorkerPool::getInstance().resume();
    return true;
  }

  ushort switch_master() {
    master_version_t curr_master = this->current_master.load();

    /*
          - switch master_id
          - clear the update bits of the new master. ( keep a seperate column or
       bit per column?)
    */

    // Before switching, clear up the new master. OR proteus do it.

    master_version_t tmp = (curr_master + 1) % global_conf::num_master_versions;
    current_master.store(tmp);

    // while (scheduler::WorkerPool::getInstance().is_all_worker_on_master_id(
    //            tmp) == false)
    //   ;

    // std::cout << "Master switch completed" << std::endl;
    return curr_master;
  }

  static __inline__ auto rdtsc() {
#if defined(__i386__)

    uint64_t x;
    __asm__ volatile(".byte 0x0f, 0x31" : "=A"(x));
    return x;

#elif defined(__x86_64__)

    uint32_t hi, lo;
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)lo) | (((uint64_t)hi) << 32u);

#elif defined(__powerpc64__) || defined(__ppc64__)
    uint64_t c;
    asm volatile("mfspr %0, 268" : "=r"(c));
    return c;
#endif
  }

  inline xid_t __attribute__((always_inline)) get_next_xid(worker_id_t wid) {
    return (rdtsc() & 0x00FFFFFFFFFFFFFF) | (((uint64_t)wid) << 56u);
  }

  uint64_t txn_start_time;
  std::atomic<master_version_t> current_master;

 private:
  TransactionManager()
      : txn_start_time(std::chrono::duration_cast<std::chrono::nanoseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count()) {
    // curr_master = 0;
    current_master = 0;
    txn_start_time = get_next_xid(0);
  }
};

};  // namespace txn

#endif /* TRANSACTION_MANAGER_HPP_ */
