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

#ifndef CONCURRENCY_CONTROL_HPP_
#define CONCURRENCY_CONTROL_HPP_

#include <iostream>
#include <mutex>
#include <utility>
#include <vector>

#include "oltp/common/lock.hpp"
#include "oltp/common/spinlock.h"
#include "oltp/storage/multi-version/delta_storage.hpp"
#include "oltp/transaction/txn_utils.hpp"

namespace txn {

class CC_MV2PL;

#define CC_extract_offset(v) ((v)&0x000000FFFFFFFFFFu)
#define CC_extract_pid(v) (((v)&0x0000FF0000000000u) >> 40u)
#define CC_extract_m_ver(v) (((v)&0x00FF000000000000u) >> 48u)

class CC_MV2PL {
 public:
  struct __attribute__((packed)) PRIMARY_INDEX_VAL {
    xid_t t_min;  //  | 1-byte w_id | 6 bytes xid |
    rowid_t VID;  // | empty 1-byte | 1-byte master_ver | 1-byte
                  // partition_id | 5-byte VID |
    lock::Spinlock_Weak latch;
    lock::AtomicTryLock write_lck;

    storage::DeltaList delta_list{};
    PRIMARY_INDEX_VAL(xid_t tid, rowid_t vid) : t_min(tid), VID(vid) {}
  };

  // TODO: this needs to be modified as we changed the format of TIDs
  static inline bool __attribute__((always_inline))
  is_readable(xid_t tmin, xid_t tid) {
    // FIXME: the following is wrong as we have encoded the worker_id in the
    //  txn_id. the comparison should be of the xid only and if same then idk
    //  because two threads can read_tsc at the same time. it doesnt mean thread
    //  with lesser ID comes first.

    xid_t w_tid = tid & 0x00FFFFFFFFFFFFFFu;
    xid_t w_tmin = tmin & 0x00FFFFFFFFFFFFFFu;

    assert(w_tmin != w_tid);

    if (w_tid >= w_tmin) {
      return true;
    } else {
      return false;
    }
  }

  static inline void __attribute__((always_inline)) release_locks(
      std::vector<CC_MV2PL::PRIMARY_INDEX_VAL *> &hash_ptrs_lock_acquired) {
    for (auto c : hash_ptrs_lock_acquired) c->write_lck.unlock();
  }

  static inline void __attribute__((always_inline))
  release_locks(CC_MV2PL::PRIMARY_INDEX_VAL **hash_ptrs, uint count) {
    for (int i = 0; i < count; i++) {
      hash_ptrs[i]->write_lck.unlock();
    }
  }
};

}  // namespace txn

#endif /* CONCURRENCY_CONTROL_HPP_ */
