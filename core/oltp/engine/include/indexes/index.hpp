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

#ifndef INDEXES_INDEX_HPP_
#define INDEXES_INDEX_HPP_

#include <iostream>

namespace indexes {

// class Index {
// public:
//  Index() { std::cout << "Index constructor\n" << std::endl; };
//
//  void put(void* key, void* val);
//  void* getByKey(void* key);
//
//  void gc();
//};

template <class K = uint64_t, class V = void *>
class Index {
 public:
  Index() = default;
  Index(std::string name) : name(std::move(name)) {}
  Index(std::string name, uint64_t initial_capacity) : name(std::move(name)) {}
  virtual V find(K key) = 0;
  virtual bool insert(K key, V &value) = 0;
  virtual ~Index() {}

  const std::string name;
};

};  // namespace indexes

#endif /* INDEXES_INDEX_HPP_ */