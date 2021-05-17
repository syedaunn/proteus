/*
    Proteus -- High-performance query processing on heterogeneous hardware.

                            Copyright (c) 2020
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

#include "oltp/storage/multi-version/mv-record-list.hpp"

#include <iostream>
#include <platform/memory/allocator.hpp>

#include "oltp/common/constants.hpp"
#include "oltp/transaction/transaction.hpp"

namespace storage::mv {

// void rollback::get_readable_version(
//    const DeltaList& delta_list, const txn::TxnTs &txTs, char* write_loc,
//    const std::vector<std::pair<uint16_t, uint16_t>>&
//    column_size_offset_pairs, const column_id_t* col_idx, const short
//    num_cols, bool read_committed_only){
//
//}

void MV_RecordList_Partial::rollback(const txn::TxnTs& txTs,
                                     global_conf::IndexVal* idx_ptr,
                                     ColumnVector& columns,
                                     const column_id_t* col_idx,
                                     short num_cols) {
  assert(false);
}

void MV_RecordList_Full::rollback(const txn::TxnTs& txTs,
                                  global_conf::IndexVal* idx_ptr,
                                  ColumnVector& columns,
                                  const column_id_t* col_idx, short num_cols) {
  assert(false &&
         "needs to be fixed when transaction has updated multiple times");
  auto& delta_list_ptr =
      static_cast<TaggedDeltaDataPtr<version_t>&>(idx_ptr->delta_list);

  LOG_IF(FATAL, delta_list_ptr.ptr() == nullptr)
      << "delta-tag verification failed";

  auto* version =
      VersionChain<MV_RecordList_Full>::get_readable_ver(delta_list_ptr, txTs);

  LOG_IF(FATAL, version == nullptr)
      << "Rollback Failed for Txn: " << txTs.txn_id << " | "
      << txTs.txn_start_time;
  assert(version != nullptr);
  auto* version_data = reinterpret_cast<char*>(version->data);

  // place back the version data here

  for (auto& col : columns) {
    // col->unit_size
    col->updateElem(idx_ptr->VID, version_data + col->byteOffset_record);
  }

  // then update the actual tmin
  idx_ptr->t_min = version->t_min;
  assert(version->t_min < txTs.txn_start_time);
}

std::bitset<1> MV_RecordList_Full::get_readable_version(
    const DeltaMemoryPtr& delta_list, const txn::TxnTs& txTs, char* write_loc,
    const std::vector<std::pair<uint16_t, uint16_t>>& column_size_offset_pairs,
    const column_id_t* col_idx, const short num_cols) {
  static thread_local std::bitset<1> ret_bitmask("1");

  TaggedDeltaDataPtr<version_t> delta_list_ptr(delta_list._val);
  LOG_IF(FATAL, delta_list_ptr.ptr() == nullptr)
      << "delta-tag verification failed";

  auto* version =
      VersionChain<MV_RecordList_Full>::get_readable_ver(delta_list_ptr, txTs);
  assert(version != nullptr);
  auto* version_data = reinterpret_cast<char*>(version->data);

  if (__unlikely(col_idx == nullptr || num_cols == 0)) {
    for (auto& col_s_pair : column_size_offset_pairs) {
      memcpy(write_loc, version_data + col_s_pair.second, col_s_pair.first);
      write_loc += col_s_pair.first;
    }

  } else {
    // copy the required attr
    for (auto i = 0; i < num_cols; i++) {
      auto& col_s_pair = column_size_offset_pairs[col_idx[i]];
      // assumption: full row is in the version.
      memcpy(write_loc, version_data + col_s_pair.second, col_s_pair.first);
      write_loc += col_s_pair.first;
    }
  }
  return ret_bitmask;
}

std::vector<MV_RecordList_Full::version_t*> MV_RecordList_Full::create_versions(
    xid_t xid, global_conf::IndexVal* idx_ptr,
    std::vector<uint16_t>& attribute_widths, storage::DeltaStore& deltaStore,
    partition_id_t partition_id, const column_id_t* col_idx, short num_cols) {
  size_t ver_size = 0;
  for (auto& attr_w : attribute_widths) {
    ver_size += attr_w;
  }

  auto* ver = (MV_RecordList_Full::version_t*)deltaStore.insert_version(
      static_cast<DeltaDataPtr&>(idx_ptr->delta_list), idx_ptr->t_min, 0,
      ver_size, partition_id);

  assert(ver != nullptr && ver->data != nullptr);

  return {ver};
}

std::vector<MV_RecordList_Partial::version_t*>
MV_RecordList_Partial::create_versions(
    xid_t xid, global_conf::IndexVal* idx_ptr,
    std::vector<uint16_t>& attribute_widths, storage::DeltaStore& deltaStore,
    partition_id_t partition_id, const column_id_t* col_idx, short num_cols) {
  std::bitset<64> attr_mask;

  /*static thread_local*/
  std::vector<uint16_t> ver_offsets{64, 0};

  auto ver_data_size = MV_RecordList_Partial::version_t::get_partial_mask_size(
      attribute_widths, ver_offsets, attr_mask, col_idx, num_cols);

  auto offset_arr_sz = attr_mask.count() * sizeof(uint16_t);

  auto* ver = (MV_RecordList_Partial::version_t*)deltaStore.insert_version(
      static_cast<DeltaDataPtr&>(idx_ptr->delta_list), idx_ptr->t_min, 0,
      ver_data_size + offset_arr_sz, partition_id);
  assert(ver != nullptr && ver->data != nullptr);

  char* offset_arr = reinterpret_cast<char*>(ver->data) + ver_data_size;

  ver->create_partial_mask((uint16_t*)offset_arr, attr_mask);

  memcpy(offset_arr, ver_offsets.data(), offset_arr_sz);
  return {ver};
}

std::bitset<64> MV_RecordList_Partial::get_readable_version(
    const DeltaMemoryPtr& delta_list, const txn::TxnTs& txTs, char* write_loc,
    const std::vector<std::pair<uint16_t, uint16_t>>& column_size_offset_pairs,
    const column_id_t* col_idx, short num_cols) {
  TaggedDeltaDataPtr<version_t> tmpTagPtr(delta_list._val);
  auto* version_ptr = tmpTagPtr.ptr();

  LOG_IF(FATAL, version_ptr == nullptr) << "delta-tag verification failed";

  // CREATE containment mask.
  std::bitset<64> done_mask;
  std::bitset<64> required_mask;

  // keep a thread-local allocated vector to remove allocations
  // on the critical path: removed static thread_local  for now.
  std::vector<uint16_t, proteus::memory::PinnedMemoryAllocator<uint16_t>>
      return_col_offsets(64, 0);

  if (__likely(num_cols > 0 && col_idx != nullptr)) {
    assert(num_cols <= 64 && "MAX columns supported: 64");
    done_mask.set();
    for (auto i = 0, offset = 0; i < num_cols; i++) {
      done_mask.reset(col_idx[i]);
      // required_mask.set(col_idx[i]);

      offset += column_size_offset_pairs[col_idx[i]].first;
      return_col_offsets[i] = offset;
    }
  } else {
    for (auto i = column_size_offset_pairs.size(); i < done_mask.size(); i++) {
      done_mask.reset(i);
    }
    // offsets are not set in this case!
  }
  required_mask = ~done_mask;

  assert(!(done_mask.all()) && "haven't even started and its done?");

  while (version_ptr != nullptr) {
    // The problem with before-images is that you have to traverse the snapshot
    // until the visible part, no matter the required attribute is there or not.

    // before-images copy
    // The following, undo the record image, for the set of required attributes
    // only.
    auto tmp = version_ptr->attribute_mask & (~done_mask);
    if (tmp.any()) {
      // set the bits that what we have it.
      done_mask |= tmp;

      // so this version contains some of the required stuff.
      // FIXME: use intrinsic to find first set bit and
      // start i from there
      for (auto i = 0; i < tmp.size(); i++) {
        if (tmp[i] == false) {
          continue;
        }
        auto& col_s_pair = column_size_offset_pairs[i];

        // Offset of some column in the version itself
        auto version_col_offset = version_ptr->get_offset(i);

        // Offset of column in the requested set of columns.
        // if requested all columns, the its columns cumulative offset.
        auto offset_idx_output =
            (num_cols == 0)
                ? col_s_pair.second
                : return_col_offsets
                      [(required_mask >> (required_mask.size() - i)).count()];

        assert(version_ptr->data != nullptr);
        memcpy((write_loc + offset_idx_output),
               static_cast<char*>(version_ptr->data) + version_col_offset,
               col_s_pair.first);
      }
    }

    // END before-images copy
    // FIXME: seems buggy
    if (version_ptr->t_min == txTs.txn_id ||
        version_ptr->t_min < txTs.txn_start_time) {
      break;
    } else {
      version_ptr = version_ptr->next.ptr();
    }
  }
  return done_mask;
}

}  // namespace storage::mv
