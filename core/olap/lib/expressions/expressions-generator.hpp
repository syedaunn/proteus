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

#ifndef EXPRESSIONS_VISITOR_HPP_
#define EXPRESSIONS_VISITOR_HPP_

#include "common/common.hpp"
#include "lib/util/caching.hpp"
#include "lib/util/catalog.hpp"
#include "olap/plugins/plugins.hpp"
//===---------------------------------------------------------------------------===//
// "Visitor(s)" responsible for generating the appropriate code per Expression
// 'node'
//===---------------------------------------------------------------------------===//
class ExpressionGeneratorVisitor : public ExprVisitor {
 public:
  ExpressionGeneratorVisitor(Context *const context,
                             const OperatorState &currState)
      : context(context), currState(currState), activeRelation("") {}
  ExpressionGeneratorVisitor(Context *const context,
                             const OperatorState &currState,
                             string activeRelation)
      : context(context),
        currState(currState),
        activeRelation(activeRelation) {}
  ProteusValue visit(const expressions::IntConstant *e);
  ProteusValue visit(const expressions::Int64Constant *e);
  ProteusValue visit(const expressions::DateConstant *e);
  ProteusValue visit(const expressions::FloatConstant *e);
  ProteusValue visit(const expressions::BoolConstant *e);
  ProteusValue visit(const expressions::StringConstant *e);
  ProteusValue visit(const expressions::DStringConstant *e);
  ProteusValue visit(const expressions::InputArgument *e);
  ProteusValue visit(const expressions::RecordProjection *e);
  /*
   * XXX How is nullptr propagated? What if one of the attributes is nullptr;
   * XXX Did not have to test it yet -> Applicable to output only
   */
  ProteusValue visit(const expressions::RecordConstruction *e);
  ProteusValue visit(const expressions::IfThenElse *e);
  // XXX Do binary operators require explicit handling of nullptr?
  ProteusValue visit(const expressions::EqExpression *e);
  ProteusValue visit(const expressions::NeExpression *e);
  ProteusValue visit(const expressions::GeExpression *e);
  ProteusValue visit(const expressions::GtExpression *e);
  ProteusValue visit(const expressions::LeExpression *e);
  ProteusValue visit(const expressions::LtExpression *e);
  ProteusValue visit(const expressions::AddExpression *e);
  ProteusValue visit(const expressions::SubExpression *e);
  ProteusValue visit(const expressions::MultExpression *e);
  ProteusValue visit(const expressions::DivExpression *e);
  ProteusValue visit(const expressions::ModExpression *e);
  ProteusValue visit(const expressions::AndExpression *e);
  ProteusValue visit(const expressions::OrExpression *e);
  ProteusValue visit(const expressions::ProteusValueExpression *e);
  ProteusValue visit(const expressions::MinExpression *e);
  ProteusValue visit(const expressions::MaxExpression *e);
  ProteusValue visit(const expressions::HashExpression *e);
  ProteusValue visit(const expressions::RefExpression *e);
  ProteusValue visit(const expressions::AssignExpression *e);
  ProteusValue visit(const expressions::NegExpression *e);
  ProteusValue visit(const expressions::ExtractExpression *e);
  ProteusValue visit(const expressions::TestNullExpression *e);
  ProteusValue visit(const expressions::CastExpression *e);

  ProteusValue visit(const expressions::ShiftLeftExpression *e);
  ProteusValue visit(const expressions::LogicalShiftRightExpression *e);
  ProteusValue visit(const expressions::ArithmeticShiftRightExpression *e);
  ProteusValue visit(const expressions::XORExpression *e);

  /**
   *
   */

  void setActiveRelation(string relName) { activeRelation = relName; }
  string getActiveRelation(string relName) { return activeRelation; }

 private:
  Context *const context;
  const OperatorState &currState;

  string activeRelation;

  /* Plugins are responsible for this action */
  // ProteusValue retrieveValue(CacheInfo info, Plugin *pg);
};

static_assert(!std::is_abstract<ExpressionGeneratorVisitor>(),
              "ExpressionGeneratorVisitor should be non-abstract");

#endif /* EXPRESSIONS_VISITOR_HPP_ */