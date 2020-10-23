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

#include <olap/util/parallel-context.hpp>

#include "common/common.hpp"
#include "lib/plugins/output/plugins-output.hpp"
#include "llvm/IR/IRBuilder.h"
#include "olap/expressions/expressions.hpp"
#include "olap/plugins/plugins.hpp"
#include "olap/routing/degree-of-parallelism.hpp"
#include "topology/device-types.hpp"

// Fwd declaration
class Plugin;
class OperatorState;

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

 protected:
  virtual void produce_(ParallelContext *context) = 0;

 public:
  virtual void produce(ParallelContext *context) final {
    //#ifndef NDEBUG
    //    auto * pip = context->getCurrentPipeline();
    //#endif
    produce_(context);
    //    assert(pip == context->getCurrentPipeline());
  }

  /**
   * Consume is not a const method because Nest does need to keep some state
   * info. Context needs to be passed from the consuming to the producing
   * side to kickstart execution once an HT has been built
   */
  virtual void consume(Context *const context,
                       const OperatorState &childState) = 0;

  virtual RecordType getRowType() const = 0;
  //  {
  //    // FIXME: throw an exception for now, but as soon as existing classes
  //    // implementat it, we should mark it function abstract
  //    throw runtime_error("unimplemented");
  //  }
  /* Used by caching service. Aim is finding whether data to be cached has been
   * filtered by some of the children operators of the plan */
  virtual bool isFiltering() const = 0;

  virtual DeviceType getDeviceType() const = 0;
  virtual DegreeOfParallelism getDOP() const = 0;
  virtual DegreeOfParallelism getDOPServers() const = 0;

 private:
  Operator *parent;
};

class UnaryOperator : public Operator {
 public:
  UnaryOperator(Operator *const child) : Operator(), child(child) {}
  ~UnaryOperator() override { LOG(INFO) << "Collapsing unary operator"; }

  virtual Operator *const getChild() const { return child; }
  void setChild(Operator *const child) { this->child = child; }

  DeviceType getDeviceType() const override {
    return getChild()->getDeviceType();
  }

  DegreeOfParallelism getDOP() const override { return getChild()->getDOP(); }
  DegreeOfParallelism getDOPServers() const override {
    return getChild()->getDOPServers();
  }

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
  ~BinaryOperator() override { LOG(INFO) << "Collapsing binary operator"; }
  Operator *getLeftChild() const { return leftChild; }
  Operator *getRightChild() const { return rightChild; }
  void setLeftChild(Operator *leftChild) { this->leftChild = leftChild; }
  void setRightChild(Operator *rightChild) { this->rightChild = rightChild; }

  DeviceType getDeviceType() const override {
    auto dev = getLeftChild()->getDeviceType();
    assert(dev == getRightChild()->getDeviceType());
    return dev;
  }

  DegreeOfParallelism getDOP() const override {
    auto dop = getLeftChild()->getDOP();
    assert(dop == getRightChild()->getDOP());
    return dop;
  }

  DegreeOfParallelism getDOPServers() const override {
    auto dop = getLeftChild()->getDOPServers();
    assert(dop == getRightChild()->getDOPServers());
    return dop;
  }

 protected:
  Operator *leftChild;
  Operator *rightChild;
};

namespace experimental {
template <typename T>
class POperator : public T {
 public:
  using T::T;

  void consume(Context *const context, const OperatorState &childState) final {
    auto ctx = dynamic_cast<ParallelContext *>(context);
    assert(ctx);

    consume(ctx, childState);
  }

  virtual void consume(ParallelContext *context,
                       const OperatorState &childState) = 0;
};

class Operator : public POperator<::Operator> {
  using POperator::POperator;
};
class UnaryOperator : public POperator<::UnaryOperator> {
  using POperator::POperator;
};
class BinaryOperator : public POperator<::BinaryOperator> {
  using POperator::POperator;
};
}  // namespace experimental

#endif /* OPERATORS_HPP_ */
