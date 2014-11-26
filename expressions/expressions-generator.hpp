/*
	RAW -- High-performance querying over raw, never-seen-before data.

							Copyright (c) 2014
		Data Intensive Applications and Systems Labaratory (DIAS)
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
#include "plugins/plugins.hpp"
//===---------------------------------------------------------------------------===//
// "Visitor(s)" responsible for generating the appropriate code per Expression 'node'
//===---------------------------------------------------------------------------===//
class ExpressionGeneratorVisitor: public ExprVisitor
{
public:
	ExpressionGeneratorVisitor(RawContext* const context,
			const OperatorState& currState) :
			context(context), currState(currState),
			activeRelation("")											{}
	ExpressionGeneratorVisitor(RawContext* const context,
			const OperatorState& currState, string activeRelation) :
			context(context), currState(currState),
			activeRelation(activeRelation)								{}
	Value* visit(expressions::IntConstant *e);
	Value* visit(expressions::FloatConstant *e);
	Value* visit(expressions::BoolConstant *e);
	Value* visit(expressions::StringConstant *e);
	Value* visit(expressions::InputArgument *e);
	Value* visit(expressions::RecordProjection *e);
	Value* visit(expressions::EqExpression *e);
	Value* visit(expressions::NeExpression *e);
	Value* visit(expressions::GeExpression *e);
	Value* visit(expressions::GtExpression *e);
	Value* visit(expressions::LeExpression *e);
	Value* visit(expressions::LtExpression *e);
	Value* visit(expressions::AddExpression *e);
	Value* visit(expressions::SubExpression *e);
	Value* visit(expressions::MultExpression *e);
	Value* visit(expressions::DivExpression *e);
	Value* visit(expressions::AndExpression *e);
	Value* visit(expressions::OrExpression *e);
	Value* visit(expressions::IfThenElse *e);

	/**
	 * STUBS
	 */
	Value* visit(expressions::RecordConstruction *e) {
		string error_msg = string("[ExpressionGeneratorVisitor]: Not implemented yet");
		LOG(ERROR) << error_msg;
		throw runtime_error(error_msg);
	}

	/**
	 *
	 */

	void setActiveRelation(string relName)		{ activeRelation = relName; }
	string getActiveRelation(string relName)	{ return activeRelation; }
private:
	RawContext* const context;
	const OperatorState& currState;

	string activeRelation;

};

#endif /* EXPRESSIONS_VISITOR_HPP_ */
