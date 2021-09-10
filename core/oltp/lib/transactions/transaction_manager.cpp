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

#include "oltp/transaction/transaction_manager.hpp"

#include "oltp/transaction/txn-executor.hpp"

namespace txn {

bool TransactionManager::executeFullQueue(
    ThreadLocal_TransactionTable& txnTable,
    const StoredProcedure& storedProcedure, worker_id_t workerId,
    partition_id_t partitionId) {
  static thread_local TransactionExecutor executor;
  static thread_local storage::Schema& schema = storage::Schema::getInstance();

  // if current_delta_id == -1, means not registered anywhere
  // if exist, that is, this thread is registered somewhere.

  // begin
  auto txn = txnTable.beginTxn(workerId, partitionId, storedProcedure.readOnly);

  // LOG(INFO) << "W " << (uint)workerId << " - " << txn.txnTs.txn_start_time;

  //  LOG(INFO) << "Starting Txn: " << txn.txnTs.txn_start_time;
  if constexpr (GcMechanism == GcTypes::OneShot) {
    static thread_local int32_t current_delta_id = -1;
    auto txn_st_time = txn.txnTs.txn_start_time;

    // FIXME: what about readOnly? lets ignore them for now.

    if (__unlikely(current_delta_id == -1)) {
      // first-time
      current_delta_id = txn.delta_version;
      schema.add_active_txn(current_delta_id, txn_st_time, workerId);
    } else {
      if (current_delta_id != txn.delta_version) {
        // we need to switch delta
        // switching means unregister from old, and add to new.
        schema.switch_delta(current_delta_id, txn.delta_version, txn_st_time,
                            workerId);
        current_delta_id = txn.delta_version;
      } else {
        // just update the max_active_epoch.
        schema.update_delta_epoch(current_delta_id, txn_st_time, workerId);
      }
    }
  }

  // execute
  bool success = storedProcedure.tx(executor, txn, storedProcedure.params);
  const auto& finishedTxn = txnTable.endTxn(txn);

  if constexpr (GcMechanism == GcTypes::SteamGC) {
    if (!finishedTxn.undoLogVector.empty()) {
      auto min = this->get_last_alive();
      txnTable.steamGC({min << txnPairGen::baseShift, min});
    }
  }

  return success;
}

bool TransactionManager::execute(StoredProcedure& storedProcedure,
                                 worker_id_t workerId,
                                 partition_id_t partitionId) {
  assert(false);

  //  // begin
  //  auto& txn =
  //      txnTable.beginTxn(workerId, partitionId, storedProcedure.readOnly);
  //
  //  auto dver = txn.delta_version;
  //  auto txn_st_time = txn.txnTs.txn_start_time;
  //
  //  if (__likely(!storedProcedure.readOnly))
  //    schema.add_active_txn(dver, txn_st_time, workerId);
  //
  //  // execute
  //  bool success = storedProcedure.tx(executor, txn, storedProcedure.params);
  //
  //  // end
  //
  //  auto& finishedTxn = txnTable.endTxn(txn);
  //
  //  if (__likely(!storedProcedure.readOnly))
  //    schema.remove_active_txn(dver, txn_st_time, workerId);
  //
  //  return success;

  return false;
}

}  // namespace txn
