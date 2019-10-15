/*
    Proteus -- High-performance query processing on heterogeneous hardware.

                            Copyright (c) 2014
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

#ifndef OPERATORS_HPP_
#define OPERATORS_HPP_

#include "common/common.hpp"
#include "expressions/expressions.hpp"
#include "llvm/IR/IRBuilder.h"
#include "plugins/output/plugins-output.hpp"
#include "plugins/plugins.hpp"
#include "topology/device-types.hpp"

// Fwd declaration
class Plugin;
class OperatorState;

class DegreeOfParallelism {
 private:
  size_t dop;

  friend class Router;

 public:
  explicit DegreeOfParallelism(size_t dop) : dop(dop) {}

  inline bool operator==(const DegreeOfParallelism &o) { return dop == o.dop; }
};

class Operator {
 public:
  Operator() : parent(nullptr) {}
  virtual ~Operator() { LOG(INFO) << "Collapsing operator"; }
  virtual void setParent(Operator *parent) { this->parent = parent; }
  Operator *const getParent() const { return parent; }
  // Overloaded operator used in checks for children of Join op. More complex
  // cases may require different handling
  bool operator==(
      const Operator &i) const { /*if(this != &i) LOG(INFO) << "NOT EQUAL
                                       OPERATORS"<<this<<" vs "<<&i;*/
    return this == &i;
  }
  virtual void produce() = 0;
  /**
   * Consume is not a const method because Nest does need to keep some state
   * info. Context needs to be passed from the consuming to the producing
   * side to kickstart execution once an HT has been built
   */
  virtual void consume(Context *const context,
                       const OperatorState &childState) = 0;

  virtual RecordType getRowType() const {
    // FIXME: throw an exception for now, but as soon as existing classes
    // implementat it, we should mark it function abstract
    throw runtime_error("unimplemented");
  }
  /* Used by caching service. Aim is finding whether data to be cached has been
   * filtered by some of the children operators of the plan */
  virtual bool isFiltering() const = 0;

  virtual DeviceType getDeviceType() const = 0;
  virtual DegreeOfParallelism getDOP() const = 0;

 private:
  Operator *parent;
};

class UnaryOperator : public Operator {
 public:
  UnaryOperator(Operator *const child) : Operator(), child(child) {}
  virtual ~UnaryOperator() { LOG(INFO) << "Collapsing unary operator"; }

  Operator *const getChild() const { return child; }
  void setChild(Operator *const child) { this->child = child; }

  virtual DeviceType getDeviceType() const {
    return getChild()->getDeviceType();
  }

  virtual DegreeOfParallelism getDOP() const { return getChild()->getDOP(); }

 private:
  Operator *child;
};

class BinaryOperator : public Operator {
 public:
  BinaryOperator(Operator *leftChild, Operator *rightChild)
      : Operator(), leftChild(leftChild), rightChild(rightChild) {}
  BinaryOperator(Operator *leftChild, Operator *rightChild,
                 Plugin *const leftPlugin, Plugin *const rightPlugin)
      : Operator(), leftChild(leftChild), rightChild(rightChild) {}
  virtual ~BinaryOperator() { LOG(INFO) << "Collapsing binary operator"; }
  Operator *getLeftChild() const { return leftChild; }
  Operator *getRightChild() const { return rightChild; }
  void setLeftChild(Operator *leftChild) { this->leftChild = leftChild; }
  void setRightChild(Operator *rightChild) { this->rightChild = rightChild; }

  virtual DeviceType getDeviceType() const {
    auto dev = getLeftChild()->getDeviceType();
    assert(dev == getRightChild()->getDeviceType());
    return dev;
  }

  virtual DegreeOfParallelism getDOP() const {
    auto dop = getLeftChild()->getDOP();
    assert(dop == getRightChild()->getDOP());
    return dop;
  }

 protected:
  Operator *leftChild;
  Operator *rightChild;
};

class OperatorState {
 public:
  OperatorState(const Operator &producer,
                const map<RecordAttribute, ProteusValueMemory> &vars)
      : producer(producer), activeVariables(vars) {}
  OperatorState(const OperatorState &opState)
      : producer(opState.producer), activeVariables(opState.activeVariables) {
    LOG(INFO) << "[Operator State: ] Copy Constructor";
  }

  [[deprecated]] const map<RecordAttribute, ProteusValueMemory> &getBindings()
      const {
    return activeVariables;
  }
  const Operator &getProducer() const { return producer; }

  const ProteusValueMemory &operator[](const RecordAttribute &key) const {
    try {
      return activeVariables.at(key);
    } catch (std::out_of_range) {
      LOG(INFO) << "Looking for: " << key.getRelationName() << "."
                << key.getAttrName();
      for (auto v : activeVariables) {
        LOG(INFO) << "  Active binding: " << v.first.getRelationName() << "."
                  << v.first.getAttrName();
      }
      throw;
    }
  }

 private:
  const Operator &producer;
  // Variable bindings produced by operator and provided to its parent
  // const map<string, AllocaInst*>& activeVariables;
  const map<RecordAttribute, ProteusValueMemory> &activeVariables;
};

#endif /* OPERATORS_HPP_ */
