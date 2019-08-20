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

#ifndef AEOLUS_SNAPSHOT_ARENA_HPP_
#define AEOLUS_SNAPSHOT_ARENA_HPP_

#include <cstddef>
#include <cstdlib>
#include <utility>

namespace aeolus {
namespace snapshot {

template <typename T>
class Arena {
  struct metadata {
    size_t numOfRecords;
    size_t epoch_id;
  };

 protected:
  metadata duringSnapshot;

  Arena() = default;

  Arena(const Arena&) = delete;
  Arena(Arena&&) = delete;
  Arena& operator=(const Arena&) = delete;
  Arena& operator=(Arena&&) = delete;

 public:
  void create_snapshot(metadata save) {
    duringSnapshot = std::move(save);
    static_cast<T&>(*this).create_snapshot_();
  }

  const metadata& getMetadata() const { return duringSnapshot; }

  void destroy_snapshot() { static_cast<T&>(*this).destroy_snapshot_(); }

  void* oltp() const { return T::oltp(); }
  void* olap() const { return T::olap(); }

  static void init(size_t size) { T::init(size); }
  static void deinit() { T::deinit(); }
};

}  // namespace snapshot
}  // namespace aeolus

#endif /* AEOLUS_SNAPSHOT_ARENA_HPP_ */
