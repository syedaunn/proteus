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

#include "common/common.hpp"
#include "util/raw-context.hpp"
#include "operators/scan.hpp"
#include "operators/select.hpp"
#include "operators/join.hpp"
#include "operators/unnest.hpp"
#include "operators/outer-unnest.hpp"
#include "operators/print.hpp"
#include "operators/root.hpp"
#include "operators/reduce.hpp"
#include "operators/nest.hpp"
#include "plugins/csv-plugin.hpp"
#include "plugins/binary-row-plugin.hpp"
#include "plugins/binary-col-plugin.hpp"
#include "plugins/json-jsmn-plugin.hpp"
#include "values/expressionTypes.hpp"
#include "expressions/binary-operators.hpp"
#include "expressions/expressions.hpp"
#include "expressions/expressions-hasher.hpp"

void scanJsmn();
void selectionJsmn();

void scanCSV();
void selectionCSV();
void joinQueryRelational();

void unnestJsmn();
void unnestJsmnDeeper();

void recordProjectionsJSON();

void scanJsmnInterpreted();
void unnestJsmnInterpreted();
void unnestJsmnChildrenInterpreted();
void unnestJsmnFiltering();
void readJSONObjectInterpreted();
void readJSONListInterpreted();

void outerUnnest();
void outerUnnestNull1();

void scanCSVBoolean();

void cidrQuery3();
void cidrQueryCount();
void cidrQueryWarm(int ageParam, int volParam);
void cidrBin();
void cidrBinStrConstant();
void cidrBinStr();

void ifThenElse();

//Hashing microbenchmarks
void hashTests();
void hashConstants();
void hashBinaryExpressions();
void hashIfThenElse();


void reduceNumeric();
void reduceBoolean();

//Producing actual output (JSON)
//Only JSON plugin made 'flusher-aware' atm
void reduceListObject();
void reduceListInt();
void reduceListIntCSV();
void reduceListRecordConstruction();
void reduceListRecordOriginal();
//Does not work yet - need explicit deserialization / serialization
void reduceListRecordOriginalCSV();

void nest();

//Extra plugins
void columnarQueryCount();
void columnarQuerySum();

/**
 * Benchmarking against a colstore
 */
void columnarMax1();
void columnarMax2();
void columnarMax3();
void columnarJoin1();


template<class T>
inline void my_hash_combine(std::size_t& seed, const T& v)
{
	boost::hash<T> hasher;
	seed ^= hasher(v);
}

int main(int argc, char* argv[])
{

	// Initialize Google's logging library.
	google::InitGoogleLogging(argv[0]);
	LOG(INFO)<< "Object-based operators";
	LOG(INFO)<< "Executing selection query";

//	scanCSV();
//	selectionCSV();
//	joinQueryRelational();
//	scanJsmn();
//	selectionJsmn();
//	recordProjectionsJSON();

	//scanJsmnInterpreted();
	//unnestJsmnChildrenInterpreted();
	//readJSONObjectInterpreted();
	//readJSONListInterpreted();

//	scanCSVBoolean();
//	reduceNumeric();
//	reduceBoolean();
//	ifThenElse();

	/* This query (3) takes a bit more time */
//	cidrQuery3();
//	cidrQueryCount();
//	cidrBinStr();
//	cidrQueryWarm(41,5);
	//100 queries
	//	for(int ageParam = 40; ageParam < 50; ageParam++)	{
	//		for(int volParam = 1; volParam <= 10; volParam++)	{
	//			cidrQueryWarm(ageParam,volParam);
	//		}
	//	}

//	unnestJsmn();
//	unnestJsmnDeeper();
//	unnestJsmnFiltering();
//	outerUnnest();
//	outerUnnestNull1();

//	hashConstants();
//	hashBinaryExpressions();
//	hashIfThenElse();

//	recordProjectionsJSON();

	//XXX: all these use same output file
//	reduceListInt();
//	reduceListObject();
//	reduceListRecordConstruction();
//	reduceListIntCSV();
//	reduceListRecordOriginal();

//	outerUnnest();
//	nest();

//	columnarQueryCount();
//	columnarQuerySum();

//	cout << "Max 1: " << endl;
//	columnarMax1();
//	cout << "Max 2: " << endl;
//	columnarMax2();
//	cout << "Max 3: " << endl;
//	columnarMax3();
	cout << "Join 1: " << endl;
	columnarJoin1();
}

void hashConstants()	{
	RawContext ctx = RawContext("HashConstants");
	RawCatalog& catalog = RawCatalog::getInstance();

	Root rootOp = Root(NULL);
	map<RecordAttribute, RawValueMemory> varPlaceholder;
	OperatorState statePlaceholder = OperatorState(rootOp, varPlaceholder);
	ExpressionHasherVisitor hasher = ExpressionHasherVisitor(&ctx,
			statePlaceholder);

	int inputInt = 1400;
	double inputFloat = 1300.5;
	bool inputBool = true;
	string inputString = string("1400");

	expressions::IntConstant* val_int = new expressions::IntConstant(inputInt);
	hasher.visit(val_int);
	expressions::FloatConstant* val_float = new expressions::FloatConstant(inputFloat);
	hasher.visit(val_float);
	expressions::BoolConstant* val_bool = new expressions::BoolConstant(inputBool);
	hasher.visit(val_bool);
	expressions::StringConstant* val_string = new expressions::StringConstant(inputString);
	hasher.visit(val_string);
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Non-generated counterparts
	boost::hash<int> hasherInt;
	boost::hash<double> hasherFloat;
	boost::hash<bool> hasherBool;
	boost::hash<string> hasherString;

	cout<<"[Int - not generated:] "<<hasherInt(inputInt)<<endl;
	cout<<"[Float - not generated:] "<<hasherFloat(inputFloat)<<endl;
	cout<<"[Float - not generated:] "<<hasherFloat(1.300500e+03)<<endl;

	cout<<"[Bool - not generated:] "<<hasherBool(inputBool)<<endl;
	cout<<"[String - not generated:] "<<hasherString(inputString)<<endl;
}

void hashBinaryExpressions()	{
	RawContext ctx = RawContext("HashBinaryExpressions");
	RawCatalog& catalog = RawCatalog::getInstance();

	Root rootOp = Root(NULL);
	map<RecordAttribute, RawValueMemory> varPlaceholder;
	OperatorState statePlaceholder = OperatorState(rootOp, varPlaceholder);
	ExpressionHasherVisitor hasher = ExpressionHasherVisitor(&ctx,
			statePlaceholder);

	int inputInt = 1400;
	double inputFloat = 1300.5;
	bool inputBool = true;
	string inputString = string("1400");

	expressions::IntConstant* val_int = new expressions::IntConstant(inputInt);
	expressions::FloatConstant* val_float = new expressions::FloatConstant(inputFloat);
	expressions::BoolConstant* val_bool = new expressions::BoolConstant(inputBool);
	expressions::StringConstant* val_string = new expressions::StringConstant(inputString);

	expressions::NeExpression* bool_ne = new expressions::NeExpression(new BoolType(),val_int,val_int);
	expressions::EqExpression* bool_eq = new expressions::EqExpression(new BoolType(),val_float,val_float);
	expressions::AddExpression* int_add = new expressions::AddExpression(new IntType(),val_int,val_int);
	expressions::MultExpression* float_mult = new expressions::MultExpression(new FloatType(),val_float,val_float);

	hasher.visit(bool_ne);
	hasher.visit(bool_eq);
	hasher.visit(int_add);
	hasher.visit(float_mult);

	ctx.prepareFunction(ctx.getGlobalFunction());

	//Non-generated counterparts
	boost::hash<int> hasherInt;
	boost::hash<double> hasherFloat;
	boost::hash<bool> hasherBool;

	cout<<"[Bool - not generated:] " << hasherBool(inputInt != inputInt) << endl;
	cout<<"[Bool - not generated:] " << hasherBool(inputFloat == inputFloat) << endl;
	cout<<"[Int - not generated:] " << hasherInt(inputInt + inputInt) << endl;
	cout<<"[Float - not generated:] "<< hasherFloat(inputFloat * inputFloat);
}

void hashIfThenElse()	{
	RawContext ctx = RawContext("HashIfThenElse");
	RawCatalog& catalog = RawCatalog::getInstance();

	Root rootOp = Root(NULL);
	map<RecordAttribute, RawValueMemory> varPlaceholder;
	OperatorState statePlaceholder = OperatorState(rootOp, varPlaceholder);
	ExpressionHasherVisitor hasher = ExpressionHasherVisitor(&ctx,
			statePlaceholder);

	int inputInt = 1400;
	double inputFloat = 1300.5;
	bool inputBool = true;
	string inputString = string("1400");

	expressions::IntConstant* val_int = new expressions::IntConstant(inputInt);
	expressions::FloatConstant* val_float = new expressions::FloatConstant(inputFloat);
	expressions::BoolConstant* val_bool = new expressions::BoolConstant(inputBool);
	expressions::StringConstant* val_string = new expressions::StringConstant(inputString);

	expressions::EqExpression* bool_eq = new expressions::EqExpression(new BoolType(),val_float,val_float);
	expressions::AddExpression* int_add = new expressions::AddExpression(new IntType(),val_int,val_int);
	expressions::SubExpression* int_sub = new expressions::SubExpression(new IntType(),val_int,val_int);

	expressions::Expression* ifElse = new expressions::IfThenElse(new BoolType(),bool_eq,int_add,int_sub);
	ifElse->accept(hasher);

	ctx.prepareFunction(ctx.getGlobalFunction());

	//Non-generated counterparts
	boost::hash<int> hasherInt;

	int toHash = inputFloat == inputFloat ? inputInt + inputInt : inputInt - inputInt;
	cout<<"[Int - not generated:] " << hasherInt(toHash) << endl;
}

void hashTests()
{
	/**
	 * HASHER TESTS
	 */
//    boost::hash<int> hasher;
//    cout << hasher(15) << endl;
//    boost::hash<string> hasherStr;
//    cout << hasherStr("15") << endl;
	boost::hash<int> hasher;
	size_t seed = 0;

	boost::hash_combine(seed, 15);
	boost::hash_combine(seed, 20);
	boost::hash_combine(seed, 29);
	cout << "Seed 1: " << seed << endl;

	seed = 0;
	size_t seedPartial = 0;
	boost::hash_combine(seed, 15);

	boost::hash_combine(seedPartial, 20);
	boost::hash_combine(seedPartial, 29);
	boost::hash_combine(seed, seedPartial);
	cout << "Seed 2: " << seed << endl;

	seed = 0;
	my_hash_combine(seed, 20);
	my_hash_combine(seed, 25);
	cout << "Seed A: " << seed << endl;

	seed = 0;
	my_hash_combine(seed, 25);
	my_hash_combine(seed, 20);
	cout << "Seed B: " << seed << endl;
}

void unnestJsmnInterpreted()
{
	RawContext ctx = RawContext("testFunction-unnestJSON-jsmn");

	string fname = string("inputs/jsmnDeeperObjects.json");

	IntType intType = IntType();
	string c1Name = string("c1");
	RecordAttribute c1 = RecordAttribute(1, fname, c1Name, &intType);
	string c2Name = string("c2");
	RecordAttribute c2 = RecordAttribute(2, fname, c2Name, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&c1);
	attsNested.push_back(&c2);
	RecordType nested = RecordType(attsNested);

	string attrName = string("a");
	string attrName2 = string("b");
	string attrName3 = string("c");
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &intType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &intType);
	RecordAttribute attr3 = RecordAttribute(3, fname, attrName3, &nested);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);

	list<string> path;
	path.insert(path.begin(), attrName3);
	pg.unnestObjectsInterpreted(path);

	pg.finish();
}

void scanJsmnInterpreted()
{
	RawContext ctx = RawContext("testFunction-ScanJSON-jsmn");

	string fname = string("jsmn.json");

	string attrName = string("a");
	string attrName2 = string("b");
	IntType attrType = IntType();
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &attrType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &attrType);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);

	list<string> path;
	path.insert(path.begin(), attrName2);
	list<ExpressionType*> types;
	types.insert(types.begin(), &attrType);
	pg.scanObjectsInterpreted(path, types);

	pg.finish();
}

void readJSONObjectInterpreted()
{
	RawContext ctx = RawContext("testFunction-ReadJSONObject");

	string fname = string("inputs/jsmnDeeper.json");

	IntType intType = IntType();
	string c1Name = string("c1");
	RecordAttribute c1 = RecordAttribute(1, fname, c1Name, &intType);
	string c2Name = string("c2");
	RecordAttribute c2 = RecordAttribute(2, fname, c2Name, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&c1);
	attsNested.push_back(&c2);
	RecordType nested = RecordType(attsNested);

	string attrName = string("a");
	string attrName2 = string("b");
	string attrName3 = string("c");
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &intType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &intType);
	RecordAttribute attr3 = RecordAttribute(3, fname, attrName3, &nested);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);
	atts.push_back(&attr3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);

	list<string> path;
	path.insert(path.begin(), attrName3);
	list<ExpressionType*> types;
	types.insert(types.begin(), &nested);
	pg.scanObjectsEagerInterpreted(path, types);

	pg.finish();
}

void readJSONListInterpreted()
{
	RawContext ctx = RawContext("testFunction-ReadJSONObject");

	string fname = string("inputs/jsmnDeeper2.json");

	IntType intType = IntType();

	ListType nested = ListType(intType);

	string attrName = string("a");
	string attrName2 = string("b");
	string attrName3 = string("c");
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &intType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &intType);
	RecordAttribute attr3 = RecordAttribute(3, fname, attrName3, &nested);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);
	atts.push_back(&attr3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);

	list<string> path;
	path.insert(path.begin(), attrName3);
	list<ExpressionType*> types;
	types.insert(types.begin(), &nested);
	pg.scanObjectsEagerInterpreted(path, types);

	pg.finish();
}

void unnestJsmnChildrenInterpreted()
{
	RawContext ctx = RawContext("testFunction-unnestJSON");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/employees.json");

	IntType intType = IntType();
	StringType stringType = StringType();

	string childName = string("name");
	RecordAttribute child1 = RecordAttribute(1, fname, childName, &stringType);
	string childAge = string("age");
	RecordAttribute child2 = RecordAttribute(1, fname, childAge, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&child1);
	RecordType nested = RecordType(attsNested);
	ListType nestedCollection = ListType(nested);

	string empName = string("name");
	RecordAttribute emp1 = RecordAttribute(1, fname, empName, &stringType);
	string empAge = string("age");
	RecordAttribute emp2 = RecordAttribute(2, fname, empAge, &intType);
	string empChildren = string("children");
	RecordAttribute emp3 = RecordAttribute(3, fname, empChildren,
			&nestedCollection);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&emp1);
	atts.push_back(&emp2);
	atts.push_back(&emp3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);

	list<string> path;
	path.insert(path.begin(), empChildren);
	pg.unnestObjectsInterpreted(path);

	pg.finish();
}

void unnestJsmn()
{

	RawContext ctx = RawContext("testFunction-unnestJSON");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/employees.json");

	IntType intType = IntType();
	StringType stringType = StringType();

	string childName = string("name");
	RecordAttribute child1 = RecordAttribute(1, fname, childName, &stringType);
	string childAge = string("age");
	RecordAttribute child2 = RecordAttribute(1, fname, childAge, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&child1);
	RecordType nested = RecordType(attsNested);
	ListType nestedCollection = ListType(nested);

	string empName = string("name");
	RecordAttribute emp1 = RecordAttribute(1, fname, empName, &stringType);
	string empAge = string("age");
	RecordAttribute emp2 = RecordAttribute(2, fname, empAge, &intType);
	string empChildren = string("children");
	RecordAttribute emp3 = RecordAttribute(3, fname, empChildren,
			&nestedCollection);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&emp1);
	atts.push_back(&emp2);
	atts.push_back(&emp3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	RecordAttribute proj1 = RecordAttribute(fname, empChildren);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(proj1);

	expressions::Expression* inputArg = new expressions::InputArgument(&inner,
			0, projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			&nestedCollection, inputArg, emp3);
	string nestedName = "c";
	Path path = Path(nestedName, proj);

	expressions::Expression* lhs = new expressions::BoolConstant(true);
	expressions::Expression* rhs = new expressions::BoolConstant(true);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), lhs, rhs);

	Unnest unnestOp = Unnest(predicate, path, &scan);
	scan.setParent(&unnestOp);

	//New record type:
	string originalRecordName = "e";
	RecordAttribute recPrev = RecordAttribute(1, fname, originalRecordName,
			&inner);
	RecordAttribute recUnnested = RecordAttribute(2, fname, nestedName,
			&nested);
	list<RecordAttribute*> attsUnnested = list<RecordAttribute*>();
	attsUnnested.push_back(&recPrev);
	attsUnnested.push_back(&recUnnested);
	RecordType unnestedType = RecordType(attsUnnested);

	//PRINT
	//a bit redundant, but 'new record construction can, in principle, cause new aliases
	projections.push_back(recPrev);
	projections.push_back(recUnnested);
	Function* debugInt = ctx.getFunction("printi");
	expressions::Expression* nestedArg = new expressions::InputArgument(
			&unnestedType, 0, projections);

	RecordAttribute toPrint = RecordAttribute(-1, fname + "." + empChildren,
			childAge, &intType);

	expressions::RecordProjection* projToPrint =
			new expressions::RecordProjection(&intType, nestedArg, toPrint);
	Print printOp = Print(debugInt, projToPrint, &unnestOp);
	unnestOp.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

/**
 * Query (approx.):
 * for(x <- employees, y <- x.children) yield y.age
 */
void outerUnnest()
{
	RawContext ctx = RawContext("testFunction-outerUnnestJSON");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/employees.json");

	IntType intType = IntType();
	//FloatType floatType = FloatType();
	StringType stringType = StringType();

	string childName = string("name");
	RecordAttribute child1 = RecordAttribute(1, fname, childName, &stringType);
	string childAge = string("age");
	RecordAttribute child2 = RecordAttribute(1, fname, childAge, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&child1);
	RecordType nested = RecordType(attsNested);
	ListType nestedCollection = ListType(nested);

	string empName = string("name");
	RecordAttribute emp1 = RecordAttribute(1, fname, empName, &stringType);
	string empAge = string("age");
	RecordAttribute emp2 = RecordAttribute(2, fname, empAge, &intType);
	string empChildren = string("children");
	RecordAttribute emp3 = RecordAttribute(3, fname, empChildren,
			&nestedCollection);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&emp1);
	atts.push_back(&emp2);
	atts.push_back(&emp3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	RecordAttribute proj1 = RecordAttribute(fname, empChildren);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(proj1);
	expressions::Expression* inputArg = new expressions::InputArgument(&inner,
			0, projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			&nestedCollection, inputArg, emp3);
	string nestedName = "c";
	Path path = Path(nestedName, proj);

	expressions::Expression* lhs = new expressions::BoolConstant(true);
	expressions::Expression* rhs = new expressions::BoolConstant(true);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), lhs, rhs);

	OuterUnnest unnestOp = OuterUnnest(predicate, path, &scan);
	scan.setParent(&unnestOp);

	//New record type:
	string originalRecordName = "e";
	RecordAttribute recPrev = RecordAttribute(1, fname, originalRecordName,
			&inner);
	RecordAttribute recUnnested = RecordAttribute(2, fname, nestedName,
			&nested);
	list<RecordAttribute*> attsUnnested = list<RecordAttribute*>();
	attsUnnested.push_back(&recPrev);
	attsUnnested.push_back(&recUnnested);
	RecordType unnestedType = RecordType(attsUnnested);

	//PRINT
	//a bit redundant, but 'new record construction can, in principle, cause new aliases
	projections.push_back(recPrev);
	projections.push_back(recUnnested);
	Function* debugInt = ctx.getFunction("printi");
	expressions::Expression* nestedArg = new expressions::InputArgument(
			&unnestedType, 0, projections);

	RecordAttribute toPrint = RecordAttribute(-1, fname + "." + empChildren,
			childAge, &intType);

	expressions::RecordProjection* projToPrint =
			new expressions::RecordProjection(&intType, nestedArg, toPrint);
	Print printOp = Print(debugInt, projToPrint, &unnestOp);
	unnestOp.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

/**
 * Query (approx.):
 * for(x <- employees, y <- x.children) yield y.age
 * [ One of the employees has no children in this example ]
 *
 * Final result not indicative of proper behavior;
 * A nest operator must follow the call(s) of outer unnest
 */
void outerUnnestNull1()
{

	RawContext ctx = RawContext("outerUnnestNull1");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/employeesHeterogeneous.json");

	IntType intType = IntType();
	//FloatType floatType = FloatType();
	StringType stringType = StringType();

	string childName = string("name");
	RecordAttribute child1 = RecordAttribute(1, fname, childName, &stringType);
	string childAge = string("age");
	RecordAttribute child2 = RecordAttribute(1, fname, childAge, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&child1);
	RecordType nested = RecordType(attsNested);
	ListType nestedCollection = ListType(nested);

	string empName = string("name");
	RecordAttribute emp1 = RecordAttribute(1, fname, empName, &stringType);
	string empAge = string("age");
	RecordAttribute emp2 = RecordAttribute(2, fname, empAge, &intType);
	string empChildren = string("children");
	RecordAttribute emp3 = RecordAttribute(3, fname, empChildren,
			&nestedCollection);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&emp1);
	atts.push_back(&emp2);
	atts.push_back(&emp3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	RecordAttribute proj1 = RecordAttribute(fname, empChildren);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(proj1);

	expressions::Expression* inputArg = new expressions::InputArgument(&inner,
			0, projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			&nestedCollection, inputArg, emp3);
	string nestedName = "c";
	Path path = Path(nestedName, proj);

	expressions::Expression* lhs = new expressions::BoolConstant(true);
	expressions::Expression* rhs = new expressions::BoolConstant(true);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), lhs, rhs);

	OuterUnnest unnestOp = OuterUnnest(predicate, path, &scan);
	scan.setParent(&unnestOp);

	//New record type:
	string originalRecordName = "e";
	RecordAttribute recPrev = RecordAttribute(1, fname, originalRecordName,
			&inner);
	RecordAttribute recUnnested = RecordAttribute(2, fname, nestedName,
			&nested);
	list<RecordAttribute*> attsUnnested = list<RecordAttribute*>();
	attsUnnested.push_back(&recPrev);
	attsUnnested.push_back(&recUnnested);
	RecordType unnestedType = RecordType(attsUnnested);

	//PRINT
	//a bit redundant, but 'new record construction can, in principle, cause new aliases
	projections.push_back(recPrev);
	projections.push_back(recUnnested);
	Function* debugInt = ctx.getFunction("printi");
	expressions::Expression* nestedArg = new expressions::InputArgument(
			&unnestedType, 0, projections);

	RecordAttribute toPrint = RecordAttribute(-1, fname + "." + empChildren,
			childAge, &intType);

	expressions::RecordProjection* projToPrint =
			new expressions::RecordProjection(&intType, nestedArg, toPrint);
	Print printOp = Print(debugInt, projToPrint, &unnestOp);
	unnestOp.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

/**
 * 1st test for Nest.
 * Not including Reduce (yet)
 * Even not considering absence of Reduce,
 * I don't think such a physical plan can occur through rewrites
 *
 * XXX Not working / tested
 */
void nest()
{
	RawContext ctx = RawContext("testFunction-nestJSON");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/employees.json");

	IntType intType = IntType();
	//FloatType floatType = FloatType();
	StringType stringType = StringType();

	string childName = string("name");
	RecordAttribute child1 = RecordAttribute(1, fname, childName, &stringType);
	string childAge = string("age");
	RecordAttribute child2 = RecordAttribute(1, fname, childAge, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&child1);
	attsNested.push_back(&child2);
	RecordType nested = RecordType(attsNested);
	ListType nestedCollection = ListType(nested);

	string empName = string("name");
	RecordAttribute emp1 = RecordAttribute(1, fname, empName, &stringType);
	string empAge = string("age");
	RecordAttribute emp2 = RecordAttribute(2, fname, empAge, &intType);
	string empChildren = string("children");
	RecordAttribute emp3 = RecordAttribute(3, fname, empChildren,
			&nestedCollection);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&emp1);
	atts.push_back(&emp2);
	atts.push_back(&emp3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	/**
	 * SCAN
	 */
	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	/**
	 * OUTER UNNEST
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	RecordAttribute proj1 = RecordAttribute(fname, empChildren);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(proj1);
	expressions::Expression* inputArg = new expressions::InputArgument(&inner,
			0, projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			&nestedCollection, inputArg, emp3);
	string nestedName = "c";
	Path path = Path(nestedName, proj);

	expressions::Expression* lhs = new expressions::BoolConstant(true);
	expressions::Expression* rhs = new expressions::BoolConstant(true);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), lhs, rhs);

	OuterUnnest unnestOp = OuterUnnest(predicate, path, &scan);
	scan.setParent(&unnestOp);

	//New record type:
	//XXX Makes no sense to come up with new bindingNames w/o having a way to eval. them
	//and ADD them in existing bindings!!!
	string originalRecordName = "e";
	RecordAttribute recPrev = RecordAttribute(1, fname, originalRecordName,
			&inner);
	RecordAttribute recUnnested = RecordAttribute(2, fname, nestedName,
			&nested);
	list<RecordAttribute*> attsUnnested = list<RecordAttribute*>();
	attsUnnested.push_back(&recPrev);
	attsUnnested.push_back(&recUnnested);
	RecordType unnestedType = RecordType(attsUnnested);

	/**
	 * NEST
	 */
	//Output (e): SUM(children.age)
	//Have to model nested type too
	projections.push_back(recPrev);
	projections.push_back(recUnnested);
	expressions::Expression* nestedArg =
			new expressions::InputArgument(&unnestedType, 0, projections);
	RecordAttribute toOutput = RecordAttribute(-1, fname + "." + empChildren,
				childAge, &intType);
	expressions::RecordProjection* nestOutput =
				new expressions::RecordProjection(&intType, nestedArg, toOutput);

	//Predicate (p): Ready from before

	//Grouping (f):
	list<expressions::InputArgument> f;
	expressions::InputArgument f_arg = *(expressions::InputArgument*) inputArg;
	f.push_back(f_arg);
	//Specified inputArg

	//What to discard if null (g):
	//Ignoring for now

	//What to materialize (payload)
	//just currently active tuple ids should be enough
	vector<RecordAttribute*> whichFields;

	vector<materialization_mode> outputModes;
	Materializer* mat = new Materializer(whichFields, outputModes);

	char nestLabel[] = "nest_001";
	Nest nestOp = Nest(SUM, nestOutput,
			 predicate, f,
			 f, &unnestOp,
			 nestLabel, *mat);
	unnestOp.setParent(&nestOp);

	//ROOT
	Root rootOp = Root(&nestOp);
	nestOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

void unnestJsmnDeeper()
{
	RawContext ctx = RawContext("testFunction-unnestJSONDeeper");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/employeesDeeper.json");

	IntType intType = IntType();
	StringType stringType = StringType();

	/**
	 * SCHEMA
	 */
	string ages = string("ages");
	ListType childrenAgesType = ListType(intType);
	RecordAttribute childrenAges = RecordAttribute(1, fname, ages,
			&childrenAgesType);
	list<RecordAttribute*> attsChildren = list<RecordAttribute*>();
	attsChildren.push_back(&childrenAges);
	RecordType children = RecordType(attsChildren);

	string empName = string("name");
	RecordAttribute emp1 = RecordAttribute(1, fname, empName, &stringType);
	string empAge = string("age");
	RecordAttribute emp2 = RecordAttribute(2, fname, empAge, &intType);
	string empChildren = string("children");
	RecordAttribute emp3 = RecordAttribute(3, fname, empChildren, &children);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&emp1);
	atts.push_back(&emp2);
	atts.push_back(&emp3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	/**
	 * SCAN
	 */
	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	/**
	 * UNNEST
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	RecordAttribute proj1 = RecordAttribute(fname, empChildren);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(proj1);

	expressions::Expression* inputArg = new expressions::InputArgument(&inner,
			0, projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			&children, inputArg, emp3);
	expressions::RecordProjection* projDeeper =
			new expressions::RecordProjection(&childrenAgesType, proj,
					childrenAges);
	string nestedName = "c";
	Path path = Path(nestedName, projDeeper);

	expressions::Expression* lhs = new expressions::BoolConstant(true);
	expressions::Expression* rhs = new expressions::BoolConstant(true);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), lhs, rhs);

	Unnest unnestOp = Unnest(predicate, path, &scan);
	scan.setParent(&unnestOp);

	//New record type:
	string originalRecordName = "e";
	RecordAttribute recPrev = RecordAttribute(1, fname, originalRecordName,
			&inner);
	RecordAttribute recUnnested = RecordAttribute(2, fname, nestedName,
			&intType);
	list<RecordAttribute*> attsUnnested = list<RecordAttribute*>();
	attsUnnested.push_back(&recPrev);
	attsUnnested.push_back(&recUnnested);
	RecordType unnestedType = RecordType(attsUnnested);

	//PRINT
	Function* debugInt = ctx.getFunction("printi");
	projections.push_back(recPrev);
	projections.push_back(recUnnested);
	expressions::Expression* nestedArg = new expressions::InputArgument(
			&unnestedType, 0, projections);

	RecordAttribute toPrint = RecordAttribute(2,
			fname + "." + empChildren + "." + ages, activeLoop, &intType);

	expressions::RecordProjection* projToPrint =
			new expressions::RecordProjection(&intType, nestedArg, toPrint);
	Print printOp = Print(debugInt, projToPrint, &unnestOp);
	unnestOp.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

void unnestJsmnFiltering()
{
	RawContext ctx = RawContext("testFunction-unnestJSONFiltering");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/employees.json");

	IntType intType = IntType();
	StringType stringType = StringType();

	string childName = string("name");
	RecordAttribute child1 = RecordAttribute(1, fname, childName, &stringType);
	string childAge = string("age");
	RecordAttribute child2 = RecordAttribute(1, fname, childAge, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&child1);
	RecordType nested = RecordType(attsNested);
	ListType nestedCollection = ListType(nested);

	string empName = string("name");
	RecordAttribute emp1 = RecordAttribute(1, fname, empName, &stringType);
	string empAge = string("age");
	RecordAttribute emp2 = RecordAttribute(2, fname, empAge, &intType);
	string empChildren = string("children");
	RecordAttribute emp3 = RecordAttribute(3, fname, empChildren,
			&nestedCollection);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&emp1);
	atts.push_back(&emp2);
	atts.push_back(&emp3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	/**
	 * UNNEST
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	RecordAttribute proj1 = RecordAttribute(fname, empChildren);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(proj1);
	expressions::Expression* inputArg = new expressions::InputArgument(&inner,
			0, projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			&stringType, inputArg, emp3);
	string nestedName = "c";
	Path path = Path(nestedName, proj);

	//New record type as the result of unnest:
	string originalRecordName = "e";
	RecordAttribute recPrev = RecordAttribute(1, fname, originalRecordName,
			&inner);
	RecordAttribute recUnnested = RecordAttribute(2, fname, nestedName,
			&nested);
	list<RecordAttribute*> attsUnnested = list<RecordAttribute*>();
	attsUnnested.push_back(&recPrev);
	attsUnnested.push_back(&recUnnested);
	RecordType unnestedType = RecordType(attsUnnested);

	expressions::Expression* nestedArg = new expressions::InputArgument(
			&unnestedType, 0, projections);
	RecordAttribute toFilter = RecordAttribute(-1, fname + "." + empChildren,
			childAge, &intType);
	expressions::RecordProjection* projToFilter =
			new expressions::RecordProjection(&intType, nestedArg, toFilter);
	expressions::Expression* lhs = projToFilter;
	expressions::Expression* rhs = new expressions::IntConstant(20);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);

	Unnest unnestOp = Unnest(predicate, path, &scan);
	scan.setParent(&unnestOp);

	//PRINT
	Function* debugInt = ctx.getFunction("printi");
	projections.push_back(recPrev);
	projections.push_back(recUnnested);
	expressions::Expression* finalArg = new expressions::InputArgument(
			&unnestedType, 0, projections);
	expressions::RecordProjection* finalArgProj =
			new expressions::RecordProjection(&intType, nestedArg, toFilter);

	Print printOp = Print(debugInt, finalArgProj, &unnestOp);
	unnestOp.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());
	pg.finish();
	catalog.clear();
}

void scanCSV()
{
	RawContext ctx = RawContext("testFunction-ScanCSV");
	RawCatalog& catalog = RawCatalog::getInstance();

	/**
	 * SCAN
	 */
	string filename = string("inputs/sailors.csv");
	PrimitiveType* intType = new IntType();
	PrimitiveType* floatType = new FloatType();
	PrimitiveType* stringType = new StringType();
	RecordAttribute* sid = new RecordAttribute(1, filename, string("sid"),
			intType);
	RecordAttribute* sname = new RecordAttribute(2, filename, string("sname"),
			stringType);
	RecordAttribute* rating = new RecordAttribute(3, filename, string("rating"),
			intType);
	RecordAttribute* age = new RecordAttribute(3, filename, string("age"),
			floatType);

	list<RecordAttribute*> attrList;
	attrList.push_back(sid);
	attrList.push_back(sname);
	attrList.push_back(rating);
	attrList.push_back(age);

	RecordType rec1 = RecordType(attrList);

	vector<RecordAttribute*> whichFields;
	whichFields.push_back(sid);
	whichFields.push_back(age);

	CSVPlugin* pg = new CSVPlugin(&ctx, filename, rec1, whichFields);
	catalog.registerPlugin(filename, pg);
	Scan scan = Scan(&ctx, *pg);

	/**
	 * ROOT
	 */
	Root rootOp = Root(&scan);
	scan.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pg->finish();
	catalog.clear();
}

void selectionCSV()
{

	RawContext ctx = RawContext("testFunction-ScanCSV");
	RawCatalog& catalog = RawCatalog::getInstance();

	/**
	 * SCAN
	 */
	string filename = string("inputs/sailors.csv");
	PrimitiveType* intType = new IntType();
	PrimitiveType* floatType = new FloatType();
	PrimitiveType* stringType = new StringType();
	RecordAttribute* sid = new RecordAttribute(1, filename, string("sid"),
			intType);
	RecordAttribute* sname = new RecordAttribute(2, filename, string("sname"),
			stringType);
	RecordAttribute* rating = new RecordAttribute(3, filename, string("rating"),
			intType);
	RecordAttribute* age = new RecordAttribute(3, filename, string("age"),
			floatType);

	list<RecordAttribute*> attrList;
	attrList.push_back(sid);
	attrList.push_back(sname);
	attrList.push_back(rating);
	attrList.push_back(age);

	RecordType rec1 = RecordType(attrList);

	vector<RecordAttribute*> whichFields;
	whichFields.push_back(sid);
	whichFields.push_back(age);

	CSVPlugin* pg = new CSVPlugin(&ctx, filename, rec1, whichFields);
	catalog.registerPlugin(filename, pg);
	Scan scan = Scan(&ctx, *pg);

	/**
	 * SELECT
	 */
	RecordAttribute projTuple = RecordAttribute(filename, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(*sid);
	projections.push_back(*age);

	expressions::Expression* lhsArg = new expressions::InputArgument(intType, 0,
			projections);
	expressions::Expression* lhs = new expressions::RecordProjection(intType,
			lhsArg, *sid);
	expressions::Expression* rhs = new expressions::IntConstant(40);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);

	Select sel = Select(predicate, &scan);
	scan.setParent(&sel);

	//PRINT
	Function* debugFloat = ctx.getFunction("printFloat");
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			floatType, lhsArg, *age);
	Print printOp = Print(debugFloat, proj, &sel);
	sel.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pg->finish();
	catalog.clear();
}

void joinQueryRelational()
{
	RawContext ctx = RawContext("testFunction-JoinCSV");
	RawCatalog& catalog = RawCatalog::getInstance();

	/**
	 * SCAN1
	 */
	string filename = string("inputs/input.csv");
	PrimitiveType* intType = new IntType();
	RecordAttribute* attr1 = new RecordAttribute(1, filename, string("att1"),
			intType);
	RecordAttribute* attr2 = new RecordAttribute(2, filename, string("att2"),
			intType);
	RecordAttribute* attr3 = new RecordAttribute(3, filename, string("att3"),
			intType);
	list<RecordAttribute*> attrList;
	attrList.push_back(attr1);
	attrList.push_back(attr2);
	attrList.push_back(attr3);

	RecordType rec1 = RecordType(attrList);
	vector<RecordAttribute*> whichFields;
	whichFields.push_back(attr1);
	whichFields.push_back(attr2);

	CSVPlugin* pg = new CSVPlugin(&ctx, filename, rec1, whichFields);
	catalog.registerPlugin(filename, pg);
	Scan scan = Scan(&ctx, *pg);

	/**
	 * SELECT
	 */
	RecordAttribute projTupleL = RecordAttribute(filename, activeLoop);
	list<RecordAttribute> projectionsL = list<RecordAttribute>();
	projectionsL.push_back(projTupleL);
	projectionsL.push_back(*attr1);
	projectionsL.push_back(*attr2);
	expressions::Expression* lhsArg = new expressions::InputArgument(intType, 0,
			projectionsL);
	expressions::Expression* lhs = new expressions::RecordProjection(intType,
			lhsArg, *attr1);
	expressions::Expression* rhs = new expressions::IntConstant(555);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);
	Select sel = Select(predicate, &scan);
	scan.setParent(&sel);

	LOG(INFO)<<"Left: "<<&sel;

	/**
	 * SCAN2
	 */
	string filename2 = string("inputs/input2.csv");
	RecordAttribute* attr1_f2 = new RecordAttribute(1, filename2,
			string("att1"), intType);
	RecordAttribute* attr2_f2 = new RecordAttribute(2, filename2,
			string("att2"), intType);
	RecordAttribute* attr3_f2 = new RecordAttribute(3, filename2,
			string("att3"), intType);

	list<RecordAttribute*> attrList2;
	attrList2.push_back(attr1_f2);
	attrList2.push_back(attr1_f2);
	attrList2.push_back(attr1_f2);
	RecordType rec2 = RecordType(attrList2);

	vector<RecordAttribute*> whichFields2;
	whichFields2.push_back(attr1_f2);
	whichFields2.push_back(attr2_f2);

	CSVPlugin* pg2 = new CSVPlugin(&ctx, filename2, rec2, whichFields2);
	catalog.registerPlugin(filename2, pg2);
	Scan scan2 = Scan(&ctx, *pg2);
	LOG(INFO)<<"Right:"<<&scan2;

	RecordAttribute projTupleR = RecordAttribute(filename2, activeLoop);
	list<RecordAttribute> projectionsR = list<RecordAttribute>();
	projectionsR.push_back(projTupleR);
	projectionsR.push_back(*attr1_f2);
	projectionsR.push_back(*attr2_f2);

	/**
	 * JOIN
	 */
	expressions::Expression* leftArg = new expressions::InputArgument(intType,
			0, projectionsL);
	expressions::Expression* left = new expressions::RecordProjection(intType,
			leftArg, *attr2);
	expressions::Expression* rightArg = new expressions::InputArgument(intType,
			1, projectionsR);
	expressions::Expression* right = new expressions::RecordProjection(intType,
			rightArg, *attr2_f2);
	expressions::BinaryExpression* joinPred = new expressions::EqExpression(
			new BoolType(), left, right);
	vector<materialization_mode> outputModes;
	outputModes.insert(outputModes.begin(), EAGER);
	outputModes.insert(outputModes.begin(), EAGER);
	Materializer* mat = new Materializer(whichFields, outputModes);

	char joinLabel[] = "join1";
	Join join = Join(joinPred, sel, scan2, joinLabel, *mat);
	sel.setParent(&join);
	scan2.setParent(&join);

	//PRINT
	Function* debugInt = ctx.getFunction("printi");
	//To be 100% correct, this proj should be over a new InputArg that only exposes the new bindings
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			new IntType(), leftArg, *attr1);
	Print printOp = Print(debugInt, proj, &join);
	join.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pg->finish();
	pg2->finish();
	catalog.clear();
}

void scanJsmn()
{
	RawContext ctx = RawContext("testFunction-ScanJSON-jsmn");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("jsmn.json");

	string attrName = string("a");
	string attrName2 = string("b");
	IntType attrType = IntType();
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &attrType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &attrType);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);

	Scan scan = Scan(&ctx, pg);

	//ROOT
	Root rootOp = Root(&scan);
	scan.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

void selectionJsmn()
{
	RawContext ctx = RawContext("testFunction-ScanJSON-jsmn");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("jsmn.json");

	string attrName = string("a");
	string attrName2 = string("b");
	IntType attrType = IntType();
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &attrType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &attrType);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);

	Scan scan = Scan(&ctx, pg);

	/**
	 * SELECT
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(attr);
	projections.push_back(attr2);

	expressions::Expression* lhsArg = new expressions::InputArgument(&attrType,
			0, projections);
	expressions::Expression* lhs = new expressions::RecordProjection(&attrType,
			lhsArg, attr2);
	expressions::Expression* rhs = new expressions::IntConstant(5);

	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);

	Select sel = Select(predicate, &scan);
	scan.setParent(&sel);

	/**
	 * PRINT
	 */
	Function* debugInt = ctx.getFunction("printi");
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			&attrType, lhsArg, attr);
	Print printOp = Print(debugInt, proj, &sel);
	sel.setParent(&printOp);

	/**
	 * ROOT
	 */
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

void reduceListInt()
{
	RawContext ctx = RawContext("testFunction-Reduce-FlushListInt");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/jsmnDeeper.json");

	IntType intType = IntType();

	string c1Name = string("c1");
	RecordAttribute c1 = RecordAttribute(1, fname, c1Name, &intType);
	string c2Name = string("c2");
	RecordAttribute c2 = RecordAttribute(2, fname, c2Name, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&c1);
	attsNested.push_back(&c2);
	RecordType nested = RecordType(attsNested);

	string attrName = string("a");
	string attrName2 = string("b");
	string attrName3 = string("c");
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &intType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &intType);
	RecordAttribute attr3 = RecordAttribute(3, fname, attrName3, &nested);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);
	atts.push_back(&attr3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	/**
	 * SCAN
	 */
	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	/**
	 * REDUCE
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(attr);
	projections.push_back(attr2);

	expressions::Expression* arg = new expressions::InputArgument(&inner, 0,
			projections);
	expressions::Expression* outputExpr = new expressions::RecordProjection(
			&intType, arg, attr);

	expressions::Expression* lhs = new expressions::RecordProjection(&intType,
			arg, attr2);
	expressions::Expression* rhs = new expressions::IntConstant(43.0);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);
	Reduce reduce = Reduce(UNION, outputExpr, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	reduce.produce();
	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

void reduceListIntCSV()
{
	RawContext ctx = RawContext("testFunction-Reduce-FlushListInt");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/sailors.csv");
	PrimitiveType* intType = new IntType();
	PrimitiveType* floatType = new FloatType();
	PrimitiveType* stringType = new StringType();
	RecordAttribute* sid = new RecordAttribute(1, fname, string("sid"),
			intType);
	RecordAttribute* sname = new RecordAttribute(2, fname, string("sname"),
			stringType);
	RecordAttribute* rating = new RecordAttribute(3, fname, string("rating"),
			intType);
	RecordAttribute* age = new RecordAttribute(4, fname, string("age"),
			floatType);

	list<RecordAttribute*> attrList;
	attrList.push_back(sid);
	attrList.push_back(sname);
	attrList.push_back(rating);
	attrList.push_back(age);

	RecordType rec1 = RecordType(attrList);

	vector<RecordAttribute*> whichFields;
	whichFields.push_back(sid);
	whichFields.push_back(age);

	CSVPlugin* pg = new CSVPlugin(&ctx, fname, rec1, whichFields);
	catalog.registerPlugin(fname, pg);
	Scan scan = Scan(&ctx, *pg);

	/**
	 * REDUCE
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(*sid);
	projections.push_back(*age);

	expressions::Expression* arg = new expressions::InputArgument(&rec1, 0,
			projections);
	expressions::Expression* outputExpr = new expressions::RecordProjection(
			intType, arg, *sid);

	expressions::Expression* lhs = new expressions::RecordProjection(intType,
			arg, *sid);
	expressions::Expression* rhs = new expressions::IntConstant(70);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);
	Reduce reduce = Reduce(UNION, outputExpr, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	reduce.produce();
	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg->finish();
	catalog.clear();
}

void reduceListObject()
{
	RawContext ctx = RawContext("testFunction-Reduce-FlushListObject");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/jsmnDeeper.json");

	IntType intType = IntType();

	string c1Name = string("c1");
	RecordAttribute c1 = RecordAttribute(1, fname, c1Name, &intType);
	string c2Name = string("c2");
	RecordAttribute c2 = RecordAttribute(2, fname, c2Name, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&c1);
	attsNested.push_back(&c2);
	RecordType nested = RecordType(attsNested);

	string attrName = string("a");
	string attrName2 = string("b");
	string attrName3 = string("c");
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &intType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &intType);
	RecordAttribute attr3 = RecordAttribute(3, fname, attrName3, &nested);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);
	atts.push_back(&attr3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	/**
	 * SCAN
	 */
	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	/**
	 * REDUCE
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(attr2);
	projections.push_back(attr3);

	expressions::Expression* arg = new expressions::InputArgument(&inner, 0,
			projections);
	expressions::Expression* outputExpr = new expressions::RecordProjection(
			&nested, arg, attr3);

	expressions::Expression* lhs = new expressions::RecordProjection(&intType,
			arg, attr2);
	expressions::Expression* rhs = new expressions::IntConstant(43.0);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);
	Reduce reduce = Reduce(UNION, outputExpr, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	reduce.produce();
	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

void reduceListRecordConstruction()
{
	RawContext ctx = RawContext("testFunction-Reduce-FlushListRecordConstruction");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/jsmnDeeper.json");

	IntType intType = IntType();

	string c1Name = string("c1");
	RecordAttribute c1 = RecordAttribute(1, fname, c1Name, &intType);
	string c2Name = string("c2");
	RecordAttribute c2 = RecordAttribute(2, fname, c2Name, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&c1);
	attsNested.push_back(&c2);
	RecordType nested = RecordType(attsNested);

	string attrName = string("a");
	string attrName2 = string("b");
	string attrName3 = string("c");
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &intType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &intType);
	RecordAttribute attr3 = RecordAttribute(3, fname, attrName3, &nested);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);
	atts.push_back(&attr3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	/**
	 * SCAN
	 */
	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	/**
	 * REDUCE
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(attr2);
	projections.push_back(attr3);

	expressions::Expression* arg = new expressions::InputArgument(&inner, 0,
			projections);

	//Preparing output:
	//Preparing new record type
	list<RecordAttribute*> newAttsTypes = list<RecordAttribute*>();
	newAttsTypes.push_back(&attr2);
	newAttsTypes.push_back(&attr3);
	RecordType newRecType = RecordType(newAttsTypes);

	//Preparing outputExpr (-> a new record construction!)
	expressions::Expression* newAttrExpr1 = new expressions::RecordProjection(
			&nested, arg, attr3);
	expressions::Expression* newAttrExpr2 = new expressions::RecordProjection(
				&intType, arg, attr2);
	expressions::AttributeConstruction attrExpr1 = expressions::AttributeConstruction("scalar", newAttrExpr1);
	expressions::AttributeConstruction attrExpr2 = expressions::AttributeConstruction("object", newAttrExpr2);
	list<expressions::AttributeConstruction> newAtts = list<expressions::AttributeConstruction>();
	newAtts.push_back(attrExpr1);
	newAtts.push_back(attrExpr2);
	expressions::RecordConstruction newRec = expressions::RecordConstruction(&newRecType,newAtts);

	//Preparing predicate of reduce
	expressions::Expression* lhs = new expressions::RecordProjection(&intType,
			arg, attr2);
	expressions::Expression* rhs = new expressions::IntConstant(43.0);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);

	//Actual operator called
	Reduce reduce = Reduce(UNION, &newRec, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	reduce.produce();
	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

/**
 * We need an intermediate internal representation to deserialize to
 * before serializing back.
 *
 * Example: How would one convert from CSV to JSON?
 * Can't just crop CSV entries and flush as JSON
 */
void reduceListRecordOriginal()
{
	RawContext ctx = RawContext("testFunction-Reduce-FlushListRecordOriginal");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/jsmnDeeper.json");

	IntType intType = IntType();

	string c1Name = string("c1");
	RecordAttribute c1 = RecordAttribute(1, fname, c1Name, &intType);
	string c2Name = string("c2");
	RecordAttribute c2 = RecordAttribute(2, fname, c2Name, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&c1);
	attsNested.push_back(&c2);
	RecordType nested = RecordType(attsNested);

	string attrName = string("a");
	string attrName2 = string("b");
	string attrName3 = string("c");
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &intType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &intType);
	RecordAttribute attr3 = RecordAttribute(3, fname, attrName3, &nested);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);
	atts.push_back(&attr3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	/**
	 * SCAN
	 */
	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	/**
	 * REDUCE
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(attr2);
	projections.push_back(attr3);

	expressions::Expression* arg = new expressions::InputArgument(&inner, 0,
			projections);

	//Preparing predicate of reduce
	expressions::Expression* lhs = new expressions::RecordProjection(&intType,
			arg, attr2);
	expressions::Expression* rhs = new expressions::IntConstant(43.0);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);

	//Actual operator called
	Reduce reduce = Reduce(UNION, arg, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	reduce.produce();
	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}


void reduceListRecordOriginalCSV()
{
	RawContext ctx = RawContext("testFunction-Reduce-FlushListRecordOriginalCSV");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/sailors.csv");
	PrimitiveType* intType = new IntType();
	PrimitiveType* floatType = new FloatType();
	PrimitiveType* stringType = new StringType();
	RecordAttribute* sid = new RecordAttribute(1, fname, string("sid"),
			intType);
	RecordAttribute* sname = new RecordAttribute(2, fname, string("sname"),
			stringType);
	RecordAttribute* rating = new RecordAttribute(3, fname, string("rating"),
			intType);
	RecordAttribute* age = new RecordAttribute(4, fname, string("age"),
			floatType);

	list<RecordAttribute*> attrList;
	attrList.push_back(sid);
	attrList.push_back(sname);
	attrList.push_back(rating);
	attrList.push_back(age);

	RecordType rec1 = RecordType(attrList);

	vector<RecordAttribute*> whichFields;
	whichFields.push_back(sid);
	whichFields.push_back(age);

	CSVPlugin* pg = new CSVPlugin(&ctx, fname, rec1, whichFields);
	catalog.registerPlugin(fname, pg);
	Scan scan = Scan(&ctx, *pg);

	/**
	 * REDUCE
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(*sid);
	projections.push_back(*age);

	expressions::Expression* arg = new expressions::InputArgument(&rec1, 0,
			projections);

	expressions::Expression* lhs = new expressions::RecordProjection(intType,
			arg, *sid);
	expressions::Expression* rhs = new expressions::IntConstant(70);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);
	Reduce reduce = Reduce(UNION, arg, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	reduce.produce();
	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg->finish();
	catalog.clear();
}

void recordProjectionsJSON()
{
	RawContext ctx = RawContext("testFunction-ScanJSON-jsmn");
	RawCatalog& catalog = RawCatalog::getInstance();

	string fname = string("inputs/jsmnDeeper.json");

	IntType intType = IntType();

	string c1Name = string("c1");
	RecordAttribute c1 = RecordAttribute(1, fname, c1Name, &intType);
	string c2Name = string("c2");
	RecordAttribute c2 = RecordAttribute(2, fname, c2Name, &intType);
	list<RecordAttribute*> attsNested = list<RecordAttribute*>();
	attsNested.push_back(&c1);
	attsNested.push_back(&c2);
	RecordType nested = RecordType(attsNested);

	string attrName = string("a");
	string attrName2 = string("b");
	string attrName3 = string("c");
	RecordAttribute attr = RecordAttribute(1, fname, attrName, &intType);
	RecordAttribute attr2 = RecordAttribute(2, fname, attrName2, &intType);
	RecordAttribute attr3 = RecordAttribute(3, fname, attrName3, &nested);

	list<RecordAttribute*> atts = list<RecordAttribute*>();
	atts.push_back(&attr);
	atts.push_back(&attr2);
	atts.push_back(&attr3);

	RecordType inner = RecordType(atts);
	ListType documentType = ListType(inner);

	jsmn::JSONPlugin pg = jsmn::JSONPlugin(&ctx, fname, &documentType);
	catalog.registerPlugin(fname, &pg);
	Scan scan = Scan(&ctx, pg);

	/**
	 * SELECT
	 */
	RecordAttribute projTuple = RecordAttribute(fname, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(attr);
	projections.push_back(attr2);
	projections.push_back(attr3);
	expressions::Expression* lhsArg = new expressions::InputArgument(&inner, 0,
			projections);
	expressions::Expression* lhs_ = new expressions::RecordProjection(&nested,
			lhsArg, attr3);
	expressions::Expression* lhs = new expressions::RecordProjection(&intType,
			lhs_, c2);
	expressions::Expression* rhs = new expressions::IntConstant(110);

	//obj.c.c2 > 110 --> Only 1 must qualify
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);

	Select sel = Select(predicate, &scan);
	scan.setParent(&sel);

	/**
	 * PRINT
	 */
	Function* debugInt = ctx.getFunction("printi");
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			&intType, lhsArg, attr);
	Print printOp = Print(debugInt, proj, &sel);
	sel.setParent(&printOp);

	/**
	 * ROOT
	 */
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg.finish();
	catalog.clear();
}

void reduceNumeric()
{
	RawContext ctx = RawContext("reduceNumeric");
	RawCatalog& catalog = RawCatalog::getInstance();

	//SCAN1
	string filename = string("inputs/sailors.csv");
	PrimitiveType* intType = new IntType();
	PrimitiveType* floatType = new FloatType();
	PrimitiveType* stringType = new StringType();
	RecordAttribute* sid = new RecordAttribute(1, filename, string("sid"),
			intType);
	RecordAttribute* sname = new RecordAttribute(2, filename, string("sname"),
			stringType);
	RecordAttribute* rating = new RecordAttribute(3, filename, string("rating"),
			intType);
	RecordAttribute* age = new RecordAttribute(4, filename, string("age"),
			floatType);

	list<RecordAttribute*> attrList;
	attrList.push_back(sid);
	attrList.push_back(sname);
	attrList.push_back(rating);
	attrList.push_back(age);

	RecordType rec1 = RecordType(attrList);

	vector<RecordAttribute*> whichFields;
	whichFields.push_back(sid);
	whichFields.push_back(age);

	CSVPlugin* pg = new CSVPlugin(&ctx, filename, rec1, whichFields);
	catalog.registerPlugin(filename, pg);
	Scan scan = Scan(&ctx, *pg);

	/**
	 * REDUCE
	 */
	RecordAttribute projTuple = RecordAttribute(filename, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(*sid);
	projections.push_back(*age);

	expressions::Expression* arg = new expressions::InputArgument(&rec1, 0,
			projections);
	expressions::Expression* outputExpr = new expressions::RecordProjection(
			intType, arg, *sid);

	expressions::Expression* lhs = new expressions::RecordProjection(floatType,
			arg, *age);
	expressions::Expression* rhs = new expressions::FloatConstant(40.0);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);
//	Reduce reduce = Reduce(SUM, outputExpr, predicate, &scan, &ctx);
//	Reduce reduce = Reduce(MULTIPLY, outputExpr, predicate, &scan, &ctx);
	Reduce reduce = Reduce(MAX, outputExpr, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	reduce.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg->finish();
	catalog.clear();
}

void scanCSVBoolean()
{
	RawContext ctx = RawContext("ScanCSVBoolean");
	RawCatalog& catalog = RawCatalog::getInstance();

	//SCAN1
	string filename = string("inputs/bills.csv");
	PrimitiveType* intType = new IntType();
	PrimitiveType* boolType = new BoolType();
	PrimitiveType* stringType = new StringType();
	RecordAttribute* category = new RecordAttribute(1, filename,
			string("category"), stringType);
	RecordAttribute* amount = new RecordAttribute(2, filename, string("amount"),
			intType);
	RecordAttribute* isPaid = new RecordAttribute(3, filename, string("isPaid"),
			boolType);

	list<RecordAttribute*> attrList;
	attrList.push_back(category);
	attrList.push_back(amount);
	attrList.push_back(isPaid);

	RecordType rec1 = RecordType(attrList);

	vector<RecordAttribute*> whichFields;
	whichFields.push_back(isPaid);

	CSVPlugin* pg = new CSVPlugin(&ctx, filename, rec1, whichFields);
	catalog.registerPlugin(filename, pg);
	Scan scan = Scan(&ctx, *pg);

	//PRINT
	Function* debugBoolean = ctx.getFunction("printBoolean");
	RecordAttribute projTuple = RecordAttribute(filename, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(*isPaid);
	expressions::Expression* arg = new expressions::InputArgument(&rec1, 0,
			projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			boolType, arg, *isPaid);
	Print printOp = Print(debugBoolean, proj, &scan);
	scan.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pg->finish();
	catalog.clear();
}
//
void reduceBoolean()
{
	RawContext ctx = RawContext("reduceAnd");
	RawCatalog& catalog = RawCatalog::getInstance();

	/**
	 * SCAN
	 */
	string filename = string("inputs/bills.csv");
	PrimitiveType* intType = new IntType();
	PrimitiveType* boolType = new BoolType();
	PrimitiveType* stringType = new StringType();
	RecordAttribute* category = new RecordAttribute(1, filename,
			string("category"), stringType);
	RecordAttribute* amount = new RecordAttribute(2, filename, string("amount"),
			intType);
	RecordAttribute* isPaid = new RecordAttribute(3, filename, string("isPaid"),
			boolType);

	list<RecordAttribute*> attrList;
	attrList.push_back(category);
	attrList.push_back(amount);
	attrList.push_back(isPaid);

	RecordType rec1 = RecordType(attrList);

	vector<RecordAttribute*> whichFields;
	whichFields.push_back(amount);
	whichFields.push_back(isPaid);

	CSVPlugin* pg = new CSVPlugin(&ctx, filename, rec1, whichFields);
	catalog.registerPlugin(filename, pg);
	Scan scan = Scan(&ctx, *pg);

	/**
	 * REDUCE
	 */
	RecordAttribute projTuple = RecordAttribute(filename, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(*amount);
	projections.push_back(*isPaid);
	expressions::Expression* arg = new expressions::InputArgument(&rec1, 0,
			projections);
	expressions::Expression* outputExpr = new expressions::RecordProjection(
			boolType, arg, *isPaid);

	expressions::Expression* lhs = new expressions::RecordProjection(intType,
			arg, *amount);
	expressions::Expression* rhs = new expressions::IntConstant(1400);
	expressions::Expression* predicate = new expressions::GtExpression(
			new BoolType(), lhs, rhs);
	Reduce reduce = Reduce(AND, outputExpr, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	reduce.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg->finish();
	catalog.clear();
}

void cidrBin()
{

	bool shortRun = false;
	string filenameBin = string("inputs/CIDR15/example.bin");

	RawContext ctx = RawContext("CIDR-QueryBin");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* intType = new IntType();

	int fieldCount = 1;
	list<RecordAttribute*> attrListBin;
	while (fieldCount <= 5)
	{
		RecordAttribute* attr = NULL;

		stringstream ss;
		ss << fieldCount;
		string attrname = ss.str();

		attr = new RecordAttribute(fieldCount++, filenameBin, attrname,
				intType);
		attrListBin.push_back(attr);
	}
	printf("Schema Ingested\n");

	RecordType recBin = RecordType(attrListBin);
	vector<RecordAttribute*> whichFieldsBin;
	RecordAttribute *field2 = new RecordAttribute(2, filenameBin, "field2",
			intType);
	RecordAttribute *field4 = new RecordAttribute(4, filenameBin, "field4",
			intType);
	whichFieldsBin.push_back(field2);
	whichFieldsBin.push_back(field4);

	BinaryRowPlugin *pgBin = new BinaryRowPlugin(&ctx, filenameBin, recBin,
			whichFieldsBin);
	catalog.registerPlugin(filenameBin, pgBin);
	Scan scanBin = Scan(&ctx, *pgBin);

	//PRINT
	Function* debugInt = ctx.getFunction("printi");

	RecordAttribute projTuple = RecordAttribute(filenameBin, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(*field2);
	projections.push_back(*field4);
	expressions::Expression* arg = new expressions::InputArgument(&recBin, 0,
			projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			intType, arg, *field4);
	Print printOp = Print(debugInt, proj, &scanBin);
	scanBin.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pgBin->finish();
	catalog.clear();
}

void cidrBinStrConstant()
{

	bool shortRun = false;
	//File schema: 5 integer fields
	string filenameBin = string("inputs/CIDR15/example.bin");

	RawContext ctx = RawContext("CIDR-QueryBinStrCons");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* intType = new IntType();

	int fieldCount = 1;
	list<RecordAttribute*> attrListBin;
	while (fieldCount <= 5)
	{
		RecordAttribute* attr = NULL;

		stringstream ss;
		ss << fieldCount;
		string attrname = ss.str();

		attr = new RecordAttribute(fieldCount++, filenameBin, attrname,
				intType);
		attrListBin.push_back(attr);
	}
	printf("Schema Ingested\n");

	RecordType recBin = RecordType(attrListBin);
	vector<RecordAttribute*> whichFieldsBin;
	RecordAttribute *field2 = new RecordAttribute(2, filenameBin, "field2",
			intType);
	RecordAttribute *field4 = new RecordAttribute(4, filenameBin, "field4",
			intType);
	whichFieldsBin.push_back(field2);
	whichFieldsBin.push_back(field4);

	BinaryRowPlugin *pgBin = new BinaryRowPlugin(&ctx, filenameBin, recBin,
			whichFieldsBin);
	catalog.registerPlugin(filenameBin, pgBin);
	Scan scanBin = Scan(&ctx, *pgBin);

	/**
	 * SELECT
	 */
	string const1 = string("test2");
	string const2 = string("test2");
	expressions::Expression* arg1str = new expressions::StringConstant(const1);
	expressions::Expression* arg2str = new expressions::StringConstant(const2);
	expressions::Expression* selPredicate = new expressions::EqExpression(
			new BoolType(), arg1str, arg2str);
	Select selStr = Select(selPredicate, &scanBin);
	scanBin.setParent(&selStr);

	/**
	 * PRINT
	 */
	Function* debugInt = ctx.getFunction("printi");

	RecordAttribute projTuple = RecordAttribute(filenameBin, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(*field2);
	projections.push_back(*field4);
	expressions::Expression* arg = new expressions::InputArgument(&recBin, 0,
			projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			intType, arg, *field4);
	Print printOp = Print(debugInt, proj, &selStr);
	selStr.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pgBin->finish();
	catalog.clear();
}

void cidrBinStr()
{

	bool shortRun = false;
	//File schema: 2 integers - 1 char field of size 5 - 2 integers
	string filenameBin = string("inputs/CIDR15/exampleStr.bin");

	RawContext ctx = RawContext("CIDR-QueryBinStr");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* intType = new IntType();
	PrimitiveType* stringType = new StringType();

	int fieldCount = 1;
	list<RecordAttribute*> attrListBin;
	while (fieldCount <= 5)
	{
		RecordAttribute* attr = NULL;

		stringstream ss;
		ss << fieldCount;
		string attrname = ss.str();
		if (fieldCount != 3)
		{
			attr = new RecordAttribute(fieldCount++, filenameBin, attrname,
					intType);
		}
		else
		{
			attr = new RecordAttribute(fieldCount++, filenameBin, attrname,
					stringType);
		}
		attrListBin.push_back(attr);
	}
	printf("Schema Ingested\n");

	RecordType recBin = RecordType(attrListBin);
	vector<RecordAttribute*> whichFieldsBin;
	RecordAttribute *field3 = new RecordAttribute(3, filenameBin, "field3",
			stringType);
	RecordAttribute *field4 = new RecordAttribute(4, filenameBin, "field4",
			intType);
	whichFieldsBin.push_back(field3);
	whichFieldsBin.push_back(field4);

	BinaryRowPlugin *pgBin = new BinaryRowPlugin(&ctx, filenameBin, recBin,
			whichFieldsBin);
	catalog.registerPlugin(filenameBin, pgBin);
	Scan scanBin = Scan(&ctx, *pgBin);

	/**
	 * SELECT
	 */
	RecordAttribute projTuple = RecordAttribute(filenameBin, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(*field3);
	projections.push_back(*field4);
	expressions::Expression* lhsArg = new expressions::InputArgument(&recBin, 0,
			projections);
	expressions::RecordProjection* lhsProj = new expressions::RecordProjection(
			stringType, lhsArg, *field3);

	string constStr = string("ALZHM");
	expressions::Expression* arg2str = new expressions::StringConstant(
			constStr);
	expressions::Expression* selPredicate = new expressions::EqExpression(
			new BoolType(), lhsProj, arg2str);
	Select selStr = Select(selPredicate, &scanBin);
	scanBin.setParent(&selStr);

	/**
	 * PRINT
	 */
	Function* debugInt = ctx.getFunction("printi");
	expressions::Expression* arg = new expressions::InputArgument(&recBin, 0,
			projections);
	expressions::RecordProjection* proj = new expressions::RecordProjection(
			intType, arg, *field4);
	Print printOp = Print(debugInt, proj, &selStr);
	selStr.setParent(&printOp);

	//ROOT
	Root rootOp = Root(&printOp);
	printOp.setParent(&rootOp);
	rootOp.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pgBin->finish();
	catalog.clear();
}

/**
 * SELECT COUNT(*)
 * FROM clinical, genetic
 * WHERE clinical.rid = genetic_part1.iid AND age > 50;
 */
void cidrQuery3()
{

	bool shortRun = true;
	string filenameClinical = string("inputs/CIDR15/clinical.csv");
	string filenameGenetic = string("inputs/CIDR15/genetic.csv");
	if (shortRun)
	{
		filenameClinical = string("inputs/CIDR15/clinical10.csv");
		filenameGenetic = string("inputs/CIDR15/genetic10.csv");
	}

	RawContext ctx = RawContext("CIDR-Query3");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* stringType = new StringType();
	PrimitiveType* intType = new IntType();
	PrimitiveType* doubleType = new FloatType();

	/**
	 * SCAN1
	 */
	ifstream fsClinicalSchema("inputs/CIDR15/attrs_clinical_vertical.csv");
	string line2;
	int fieldCount2 = 0;
	list<RecordAttribute*> attrListClinical;

	while (getline(fsClinicalSchema, line2))
	{
		RecordAttribute* attr = NULL;
		if (fieldCount2 < 2)
		{
			attr = new RecordAttribute(fieldCount2 + 1, filenameClinical, line2,
					intType);
		}
		else if (fieldCount2 >= 4)
		{
			attr = new RecordAttribute(fieldCount2 + 1, filenameClinical, line2,
					doubleType);
		}
		else
		{
			attr = new RecordAttribute(fieldCount2 + 1, filenameClinical, line2,
					stringType);
		}
		attrListClinical.push_back(attr);
		fieldCount2++;
	}
	RecordType recClinical = RecordType(attrListClinical);
	vector<RecordAttribute*> whichFieldsClinical;
	RecordAttribute* rid = new RecordAttribute(1, filenameClinical, "RID",
			intType);
	RecordAttribute* age = new RecordAttribute(1, filenameClinical, "Age",
			intType);
	whichFieldsClinical.push_back(rid);
	whichFieldsClinical.push_back(age);

	CSVPlugin* pgClinical = new CSVPlugin(&ctx, filenameClinical, recClinical,
			whichFieldsClinical);
	catalog.registerPlugin(filenameClinical, pgClinical);
	Scan scanClinical = Scan(&ctx, *pgClinical);

	//SELECT
	RecordAttribute projTupleClinical = RecordAttribute(filenameClinical,
			activeLoop);
	list<RecordAttribute> projectionsClinical = list<RecordAttribute>();
	projectionsClinical.push_back(projTupleClinical);
	projectionsClinical.push_back(*rid);
	projectionsClinical.push_back(*age);

	expressions::Expression* argClinical = new expressions::InputArgument(
			&recClinical, 0, projectionsClinical);
	expressions::RecordProjection* clinicalAge =
			new expressions::RecordProjection(intType, argClinical, *age);
	expressions::Expression* rhs = new expressions::IntConstant(50);
	expressions::Expression* selPredicate = new expressions::GtExpression(
			new BoolType(), clinicalAge, rhs);
	Select selClinical = Select(selPredicate, &scanClinical);
	scanClinical.setParent(&selClinical);

	/**
	 * SCAN2
	 */
	ifstream fsGeneticSchema("inputs/CIDR15/attrs_genetic_vertical.csv");
	string line;
	int fieldCount = 0;
	list<RecordAttribute*> attrListGenetic;
	while (getline(fsGeneticSchema, line))
	{
		RecordAttribute* attr = NULL;
		if (fieldCount != 0)
		{
			attr = new RecordAttribute(fieldCount + 1, filenameGenetic, line,
					intType);
		}
		else
		{
			attr = new RecordAttribute(fieldCount + 1, filenameGenetic, line,
					stringType);
		}
		attrListGenetic.push_back(attr);
		fieldCount++;
	}

	RecordType recGenetic = RecordType(attrListGenetic);
	vector<RecordAttribute*> whichFieldsGenetic;
	RecordAttribute* iid = new RecordAttribute(2, filenameGenetic, "IID",
			intType);
	whichFieldsGenetic.push_back(iid);

	CSVPlugin* pgGenetic = new CSVPlugin(&ctx, filenameGenetic, recGenetic,
			whichFieldsGenetic);
	catalog.registerPlugin(filenameGenetic, pgGenetic);
	Scan scanGenetic = Scan(&ctx, *pgGenetic);

	/**
	 *  JOIN
	 */
	expressions::RecordProjection* argClinicalProj =
			new expressions::RecordProjection(intType, argClinical, *rid);

	RecordAttribute projTupleGenetic = RecordAttribute(filenameGenetic,
			activeLoop);
	list<RecordAttribute> projectionsGenetic = list<RecordAttribute>();
	projectionsGenetic.push_back(projTupleGenetic);
	projectionsGenetic.push_back(*iid);
	expressions::Expression* argGenetic = new expressions::InputArgument(
			&recGenetic, 0, projectionsGenetic);
	expressions::RecordProjection* argGeneticProj =
			new expressions::RecordProjection(intType, argGenetic, *iid);

	expressions::BinaryExpression* joinPred = new expressions::EqExpression(
			new BoolType(), argClinicalProj, argGeneticProj);
	vector<materialization_mode> outputModes;
	outputModes.insert(outputModes.begin(), EAGER);
	outputModes.insert(outputModes.begin(), EAGER);
	Materializer* mat = new Materializer(whichFieldsClinical, outputModes);

	char joinLabel[] = "joinPatients";
	Join join = Join(joinPred, selClinical, scanGenetic, joinLabel, *mat);
	selClinical.setParent(&join);
	scanGenetic.setParent(&join);

	//	//PRINT
	//	Function* debugInt = ctx.getFunction("printi");
	//	Print printOp = Print(debugInt,argClinicalProj,&join);
	//	join.setParent(&printOp);
	//
	//	//ROOT
	//	Root rootOp = Root(&printOp);
	//	printOp.setParent(&rootOp);
	//	rootOp.produce();

	/**
	 * REDUCE
	 * (COUNT)
	 */
	expressions::Expression* outputExpr = new expressions::IntConstant(1);
	//expressions::RecordProjection* outputExpr = new expressions::RecordProjection(intType,argClinical,*rid);

	expressions::Expression* val_true = new expressions::BoolConstant(1);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), val_true, val_true);
	Reduce reduce = Reduce(SUM, outputExpr, predicate, &join, &ctx);
	//Reduce reduce = Reduce(MAX, outputExpr, predicate, &join, &ctx);
	join.setParent(&reduce);

	reduce.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pgGenetic->finish();
	pgClinical->finish();
	catalog.clear();
}

void cidrQueryCount()
{

	bool shortRun = false;
	string filenameGenetic = string("inputs/CIDR15/genetic.csv");
	if (shortRun)
	{
		filenameGenetic = string("inputs/CIDR15/genetic10.csv");
	}

	RawContext ctx = RawContext("CIDR-Query3");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* stringType = new StringType();
	PrimitiveType* intType = new IntType();
	PrimitiveType* doubleType = new FloatType();

	/**
	 * SCAN2
	 */
	ifstream fsGeneticSchema("inputs/CIDR15/attrs_genetic_vertical.csv");
	string line;
	int fieldCount = 0;
	list<RecordAttribute*> attrListGenetic;
	while (getline(fsGeneticSchema, line))
	{
		RecordAttribute* attr = NULL;
		if (fieldCount != 0)
		{
			attr = new RecordAttribute(fieldCount + 1, filenameGenetic, line,
					intType);
		}
		else
		{
			attr = new RecordAttribute(fieldCount + 1, filenameGenetic, line,
					stringType);
		}
		attrListGenetic.push_back(attr);
		fieldCount++;
	}
	printf("Schema Ingested\n");

	RecordType recGenetic = RecordType(attrListGenetic);
	vector<RecordAttribute*> whichFieldsGenetic;
	RecordAttribute* iid = new RecordAttribute(2, filenameGenetic, "IID",
			intType);
	whichFieldsGenetic.push_back(iid);

	CSVPlugin* pgGenetic = new CSVPlugin(&ctx, filenameGenetic, recGenetic,
			whichFieldsGenetic);
	catalog.registerPlugin(filenameGenetic, pgGenetic);
	Scan scanGenetic = Scan(&ctx, *pgGenetic);

	/**
	 * REDUCE
	 * (COUNT)
	 */
	expressions::Expression* outputExpr = new expressions::IntConstant(1);
	expressions::Expression* val_true = new expressions::BoolConstant(1);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), val_true, val_true);
	Reduce reduce = Reduce(SUM, outputExpr, predicate, &scanGenetic, &ctx);
	scanGenetic.setParent(&reduce);

	//Run function
	struct timespec t0, t1;
	clock_gettime(CLOCK_REALTIME, &t0);
	reduce.produce();
	ctx.prepareFunction(ctx.getGlobalFunction());
	clock_gettime(CLOCK_REALTIME, &t1);
	printf("Execution took %f seconds\n", diff(t0, t1));

	//Close all open files & clear
	pgGenetic->finish();
	catalog.clear();
}

/**
 * SELECT MAX(Olfactory_L_1691_Vol)
 * FROM clinical, genetic_part1, regions_part6
 * WHERE clinical.rid = genetic_part1.iid AND
 *       clinical.rid = regions_part6.iid AND
 *       age > 44 AND city = 'Lausa'      AND
 *       genetic_part1.FID = 'ALZHM'      AND
 *       Olfactory_L_1691_Vol <= 2;
 */

void cidrQueryWarm(int ageParam, int volParam)
{
//	int ageParam = 44;
//	int volParam = 2;

	string filenameClinical = string("inputs/CIDR15/clinicalToConvert.bin");
	string filenameGenetic = string("inputs/CIDR15/geneticToConvert.bin");
	string filenameRegions = string("inputs/CIDR15/regionsToConvert.bin");

	RawContext ctx = RawContext("CIDR-QueryWarm");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* stringType = new StringType();
	PrimitiveType* intType = new IntType();
	PrimitiveType* doubleType = new FloatType();

	/**
	 * SCAN (Clinical)
	 */
	RecordAttribute *rid = new RecordAttribute(1, filenameClinical, "rid",
			intType);
	RecordAttribute *age = new RecordAttribute(2, filenameClinical, "age",
			intType);
	RecordAttribute *city = new RecordAttribute(3, filenameClinical, "city",
			stringType);
	list<RecordAttribute*> attrListClinical;
	attrListClinical.push_back(rid);
	attrListClinical.push_back(age);
	attrListClinical.push_back(city);
	RecordType recClinical = RecordType(attrListClinical);

	vector<RecordAttribute*> whichFieldsClinical;
	whichFieldsClinical.push_back(rid);
	whichFieldsClinical.push_back(age);
	whichFieldsClinical.push_back(city);

	BinaryRowPlugin *pgClinical = new BinaryRowPlugin(&ctx, filenameClinical,
			recClinical, whichFieldsClinical);

	catalog.registerPlugin(filenameClinical, pgClinical);
	Scan scanClinical = Scan(&ctx, *pgClinical);

	/**
	 * SELECT
	 */
	RecordAttribute projTupleClinical = RecordAttribute(filenameClinical,
			activeLoop);
	list<RecordAttribute> projectionsClinical = list<RecordAttribute>();
	projectionsClinical.push_back(projTupleClinical);
	projectionsClinical.push_back(*rid);
	projectionsClinical.push_back(*age);
	projectionsClinical.push_back(*city);
	expressions::Expression* argClinical = new expressions::InputArgument(
			&recClinical, 0, projectionsClinical);
	expressions::RecordProjection* clinicalAge =
			new expressions::RecordProjection(intType, argClinical, *age);
	expressions::Expression* rhsAge = new expressions::IntConstant(ageParam);
	expressions::Expression* selPredicate1 = new expressions::GtExpression(
			new BoolType(), clinicalAge, rhsAge);

	expressions::RecordProjection* clinicalCity =
			new expressions::RecordProjection(stringType, argClinical, *city);
	string cityName = string("Lausa");
	expressions::Expression* rhsCity = new expressions::StringConstant(
			cityName);
	expressions::Expression* selPredicate2 = new expressions::EqExpression(
			new BoolType(), clinicalCity, rhsCity);

	expressions::Expression* selPredicate = new expressions::AndExpression(
			new BoolType(), selPredicate1, selPredicate2);

	Select selClinical = Select(selPredicate, &scanClinical);
	scanClinical.setParent(&selClinical);

	/**
	 * SCAN (Regions)
	 */
	RecordAttribute *iidRegions = new RecordAttribute(1, filenameRegions, "iid",
			intType);
	RecordAttribute *vol = new RecordAttribute(2, filenameRegions, "vol",
			intType);
	list<RecordAttribute*> attrListRegions;
	attrListRegions.push_back(iidRegions);
	attrListRegions.push_back(vol);
	RecordType recRegions = RecordType(attrListRegions);

	vector<RecordAttribute*> whichFieldsRegions;
	whichFieldsRegions.push_back(iidRegions);
	whichFieldsRegions.push_back(vol);

	BinaryRowPlugin *pgRegions = new BinaryRowPlugin(&ctx, filenameRegions,
			recRegions, whichFieldsRegions);

	catalog.registerPlugin(filenameRegions, pgRegions);
	Scan scanRegions = Scan(&ctx, *pgRegions);

	/**
	 * SELECT
	 */
	RecordAttribute projTupleRegions = RecordAttribute(filenameRegions,
			activeLoop);
	list<RecordAttribute> projectionsRegions = list<RecordAttribute>();
	projectionsRegions.push_back(projTupleRegions);
	projectionsRegions.push_back(*iidRegions);
	projectionsRegions.push_back(*vol);
	expressions::Expression* argRegions = new expressions::InputArgument(
			&recRegions, 0, projectionsRegions);
	expressions::RecordProjection* regionsVol =
			new expressions::RecordProjection(intType, argRegions, *vol);
	expressions::Expression* rhsVol = new expressions::IntConstant(volParam);
	expressions::Expression* selPredicateRegions =
			new expressions::LeExpression(new BoolType(), regionsVol, rhsVol);
	Select selRegions = Select(selPredicateRegions, &scanRegions);
	scanRegions.setParent(&selRegions);

	/**
	 *  JOIN
	 *  clinical JOIN regions
	 */
	expressions::RecordProjection* argClinicalId =
			new expressions::RecordProjection(intType, argClinical, *rid);
	expressions::RecordProjection* argRegionsId =
			new expressions::RecordProjection(intType, argRegions, *iidRegions);
	expressions::BinaryExpression* joinPredClinical =
			new expressions::EqExpression(new BoolType(), argClinicalId,
					argRegionsId);
	//Don't need fields from left side
	vector<materialization_mode> outputModes;
	vector<RecordAttribute*> whichFieldsJoin;
	Materializer* mat = new Materializer(whichFieldsJoin, outputModes);

	char joinLabel[] = "joinClinicalRegions";
	Join joinClinicalRegions = Join(joinPredClinical, selClinical, scanRegions,
			joinLabel, *mat);
	selClinical.setParent(&joinClinicalRegions);
	selRegions.setParent(&joinClinicalRegions);

	/**
	 * SCAN (Genetic)
	 */
	RecordAttribute *fid = new RecordAttribute(1, filenameGenetic, "fid",
			stringType);
	RecordAttribute *iid = new RecordAttribute(2, filenameGenetic, "iid",
			intType);
	list<RecordAttribute*> attrListGenetic;
	attrListGenetic.push_back(fid);
	attrListGenetic.push_back(iid);
	RecordType recGenetic = RecordType(attrListGenetic);

	vector<RecordAttribute*> whichFieldsGenetic;
	whichFieldsGenetic.push_back(fid);
	whichFieldsGenetic.push_back(iid);

	BinaryRowPlugin *pgGenetic = new BinaryRowPlugin(&ctx, filenameGenetic,
			recGenetic, whichFieldsGenetic);

	catalog.registerPlugin(filenameGenetic, pgGenetic);
	Scan scanGenetic = Scan(&ctx, *pgGenetic);

	/**
	 * SELECT
	 */
	RecordAttribute projTupleGenetic = RecordAttribute(filenameGenetic,
			activeLoop);
	list<RecordAttribute> projectionsGenetic = list<RecordAttribute>();
	projectionsGenetic.push_back(projTupleGenetic);
	projectionsGenetic.push_back(*fid);
	projectionsGenetic.push_back(*iid);
	expressions::Expression* argGenetic = new expressions::InputArgument(
			&recGenetic, 0, projectionsGenetic);
	expressions::RecordProjection* geneticFid =
			new expressions::RecordProjection(stringType, argGenetic, *fid);
	string fidName = string("ALZHM");
	expressions::Expression* rhsFid = new expressions::StringConstant(fidName);
	expressions::Expression* selPredicateGenetic =
			new expressions::EqExpression(new BoolType(), geneticFid, rhsFid);
	Select selGenetic = Select(selPredicateGenetic, &scanGenetic);
	scanGenetic.setParent(&selGenetic);

	/**
	 *  JOIN
	 *  intermediate JOIN genetic
	 */

	expressions::RecordProjection* argGeneticId =
			new expressions::RecordProjection(intType, argGenetic, *iid);
	expressions::BinaryExpression* joinPredGenetic =
			new expressions::EqExpression(new BoolType(), argRegionsId,
					argGeneticId);
	vector<materialization_mode> outputModes2;
	outputModes2.insert(outputModes.begin(), EAGER);
	vector<RecordAttribute*> whichFieldsJoin2;
	whichFieldsJoin2.push_back(vol);
	Materializer* mat2 = new Materializer(whichFieldsJoin2, outputModes2);

	char joinLabel2[] = "joinGenetic";
	Join joinGenetic = Join(joinPredGenetic, joinClinicalRegions, scanGenetic,
			joinLabel2, *mat2);
	joinClinicalRegions.setParent(&joinGenetic);
	selGenetic.setParent(&joinGenetic);

//	//PRINT
//	Function* debugInt = ctx.getFunction("printi");
//	expressions::RecordProjection* argGeneticProj =
//			new expressions::RecordProjection(intType, argRegions, *vol);
//
//	Print printOp = Print(debugInt, argGeneticProj, &joinGenetic);
//	joinGenetic.setParent(&printOp);
//
//	//ROOT
//	Root rootOp = Root(&printOp);
//	printOp.setParent(&rootOp);
//	rootOp.produce();

	/**
	 * REDUCE
	 * (MAX)
	 */
	expressions::RecordProjection* outputExpr =
			new expressions::RecordProjection(intType, argRegions, *vol);
	expressions::Expression* val_true = new expressions::BoolConstant(1);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), val_true, val_true);
	Reduce reduce = Reduce(MAX, outputExpr, predicate, &joinGenetic, &ctx);
	joinGenetic.setParent(&reduce);

	reduce.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pgGenetic->finish();
	pgRegions->finish();
	pgClinical->finish();
	catalog.clear();
}

void ifThenElse()	{
	RawContext ctx = RawContext("ifThenElseExpr");
	RawCatalog& catalog = RawCatalog::getInstance();

	//SCAN1
	string filename = string("inputs/bills.csv");
	PrimitiveType* intType = new IntType();
	PrimitiveType* boolType = new BoolType();
	PrimitiveType* stringType = new StringType();
	RecordAttribute* category = new RecordAttribute(1,filename,string("category"),stringType);
	RecordAttribute* amount = new RecordAttribute(2,filename,string("amount"),intType);
	RecordAttribute* isPaid = new RecordAttribute(3,filename,string("isPaid"),boolType);

	list<RecordAttribute*> attrList;
	attrList.push_back(category);
	attrList.push_back(amount);
	attrList.push_back(isPaid);

	RecordType rec1 = RecordType(attrList);

	vector<RecordAttribute*> whichFields;
	whichFields.push_back(amount);

	CSVPlugin* pg = new CSVPlugin(&ctx,filename, rec1, whichFields);
	catalog.registerPlugin(filename,pg);
	Scan scan = Scan(&ctx,*pg);

	/**
	 * REDUCE
	 */
	RecordAttribute projTuple = RecordAttribute(filename, activeLoop);
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(projTuple);
	projections.push_back(*amount);

	expressions::Expression* arg 	= new expressions::InputArgument(&rec1,0,projections);
	expressions::Expression* ifLhs  = new expressions::RecordProjection(boolType,arg,*amount);
	expressions::Expression* ifRhs  = new expressions::IntConstant(200);
	expressions::Expression* ifCond = new expressions::GtExpression(boolType,ifLhs,ifRhs);

	expressions::Expression* trueCons  = new expressions::BoolConstant(true);
	expressions::Expression* falseCons = new expressions::BoolConstant(false);
	expressions::Expression* ifElse = new expressions::IfThenElse(boolType,ifCond,trueCons,falseCons);

	expressions::Expression* predicate = new expressions::EqExpression(new BoolType(),trueCons,trueCons);
	Reduce reduce = Reduce(AND, ifElse, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	reduce.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	pg->finish();
	catalog.clear();
}

void columnarQueryCount()
{

	string filenamePrefix = string("inputs/CIDR15/regionsToConvert");

	RawContext ctx = RawContext("columnar-count");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* stringType = new StringType();
	PrimitiveType* intType = new IntType();
	PrimitiveType* doubleType = new FloatType();

	string line;
	int fieldCount = 0;
	list<RecordAttribute*> attrListRegions;

	RecordAttribute *pid, *rid;
	pid = new RecordAttribute(++fieldCount, filenamePrefix, "pid",intType);
	rid = new RecordAttribute(++fieldCount, filenamePrefix, "rid",intType);
	attrListRegions.push_back(pid);
	attrListRegions.push_back(rid);

	RecordType recRegions = RecordType(attrListRegions);
	vector<RecordAttribute*> whichFieldsRegions;

	whichFieldsRegions.push_back(pid);
	whichFieldsRegions.push_back(rid);

	BinaryColPlugin *pgRegions = new BinaryColPlugin(&ctx, filenamePrefix, recRegions,
			whichFieldsRegions);
	catalog.registerPlugin(filenamePrefix, pgRegions);
	Scan scanRegions = Scan(&ctx, *pgRegions);

	/**
	 * REDUCE
	 * (COUNT)
	 */
	expressions::Expression* outputExpr = new expressions::IntConstant(1);
	expressions::Expression* val_true = new expressions::BoolConstant(1);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), val_true, val_true);
	Reduce reduce = Reduce(SUM, outputExpr, predicate, &scanRegions, &ctx);
	scanRegions.setParent(&reduce);

	//Run function
	struct timespec t0, t1;
	clock_gettime(CLOCK_REALTIME, &t0);
	reduce.produce();
	ctx.prepareFunction(ctx.getGlobalFunction());
	clock_gettime(CLOCK_REALTIME, &t1);
	printf("Execution took %f seconds\n", diff(t0, t1));

	//Close all open files & clear
	pgRegions->finish();
	catalog.clear();
}

void columnarQuerySum()
{
	string filenamePrefix = string("inputs/CIDR15/regionsToConvert");

	RawContext ctx = RawContext("columnar-count");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* stringType = new StringType();
	PrimitiveType* intType = new IntType();
	PrimitiveType* doubleType = new FloatType();

	string line;
	int fieldCount = 1;
	list<RecordAttribute*> attrListRegions;

	RecordAttribute *pid, *rid;
	pid = new RecordAttribute(fieldCount++, filenamePrefix, "rid",intType);
	rid = new RecordAttribute(fieldCount, filenamePrefix, "pid",intType);
	attrListRegions.push_back(pid);
	attrListRegions.push_back(rid);

	RecordType recRegions = RecordType(attrListRegions);
	vector<RecordAttribute*> whichFieldsRegions;

	whichFieldsRegions.push_back(pid);
	whichFieldsRegions.push_back(rid);

	BinaryColPlugin *pgRegions = new BinaryColPlugin(&ctx, filenamePrefix, recRegions,
			whichFieldsRegions);
	catalog.registerPlugin(filenamePrefix, pgRegions);
	Scan scanRegions = Scan(&ctx, *pgRegions);

	/**
	 * REDUCE
	 * (SUM)
	 */
	list<RecordAttribute> projections = list<RecordAttribute>();
	RecordAttribute projTupleRegions = RecordAttribute(filenamePrefix,activeLoop);
	projections.push_back(projTupleRegions);
	projections.push_back(*pid);
	projections.push_back(*rid);
	expressions::Expression* arg 	= new expressions::InputArgument(&recRegions,0,projections);
	expressions::Expression* projRid  = new expressions::RecordProjection(intType,arg,*pid);

	expressions::Expression* val_true = new expressions::BoolConstant(1);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), val_true, val_true);
	Reduce reduce = Reduce(SUM, projRid, predicate, &scanRegions, &ctx);
	scanRegions.setParent(&reduce);

	//Run function
	struct timespec t0, t1;
	clock_gettime(CLOCK_REALTIME, &t0);
	reduce.produce();
	ctx.prepareFunction(ctx.getGlobalFunction());
	clock_gettime(CLOCK_REALTIME, &t1);
	printf("Execution took %f seconds\n", diff(t0, t1));

	//Close all open files & clear
	pgRegions->finish();
	catalog.clear();
}

void columnarMax1()
{
	string filenamePrefix = string("/cloud_store/manosk/data/vida-engine/synthetic/100m-30cols-fixed");

	RawContext ctx = RawContext("columnarMax1");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* intType = new IntType();

	string line;
	int fieldCount = 1;
	list<RecordAttribute*> attrListRegions;

	ostringstream sstream;
	RecordAttribute *toProject = NULL;
	for(int i = 1; i <= 30; i++)	{
		sstream.str("");
		sstream << "field" << fieldCount;
		RecordAttribute *field_i = new RecordAttribute(fieldCount, filenamePrefix,sstream.str().c_str(),intType);
		attrListRegions.push_back(field_i);

		if(fieldCount == 10)	{
			toProject = field_i;
		}
		fieldCount++;
	}

	RecordType recRegions = RecordType(attrListRegions);
	vector<RecordAttribute*> whichFieldsRegions;

	whichFieldsRegions.push_back(toProject);

	BinaryColPlugin *pgRegions = new BinaryColPlugin(&ctx, filenamePrefix, recRegions,
			whichFieldsRegions);
	catalog.registerPlugin(filenamePrefix, pgRegions);
	Scan scanRegions = Scan(&ctx, *pgRegions);

	/**
	 * REDUCE
	 * (MAX)
	 */
	list<RecordAttribute> projections = list<RecordAttribute>();
	RecordAttribute projTupleRegions = RecordAttribute(filenamePrefix,activeLoop);
	projections.push_back(projTupleRegions);
	projections.push_back(*toProject);
	expressions::Expression* arg 	= new expressions::InputArgument(&recRegions,0,projections);
	expressions::Expression* projRid  = new expressions::RecordProjection(intType,arg,*toProject);

	expressions::Expression* val_true = new expressions::BoolConstant(1);
	expressions::Expression* predicate = new expressions::EqExpression(
			new BoolType(), val_true, val_true);
	Reduce reduce = Reduce(MAX, projRid, predicate, &scanRegions, &ctx);
	scanRegions.setParent(&reduce);

	//Run function
	struct timespec t0, t1;
	clock_gettime(CLOCK_REALTIME, &t0);
	reduce.produce();
	ctx.prepareFunction(ctx.getGlobalFunction());
	clock_gettime(CLOCK_REALTIME, &t1);
	printf("Execution took %f seconds\n", diff(t0, t1));

	//Close all open files & clear
	pgRegions->finish();
	catalog.clear();
}

void columnarMax2()
{
	string filenamePrefix = string("/cloud_store/manosk/data/vida-engine/synthetic/100m-30cols-fixed");

	RawContext ctx = RawContext("columnarMax2");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* intType = new IntType();

	string line;
	int fieldCount = 1;
	list<RecordAttribute*> attrListRegions;

	ostringstream sstream;
	RecordAttribute *selectProj = NULL;
	RecordAttribute *toProject = NULL;
	for(int i = 1; i <= 30; i++)	{
		sstream.str("");
		sstream << "field" << fieldCount;
		RecordAttribute *field_i = new RecordAttribute(fieldCount, filenamePrefix,sstream.str().c_str(),intType);
		attrListRegions.push_back(field_i);

		if(fieldCount == 10)	{
			toProject = field_i;
		}

		if(fieldCount == 20)	{
			selectProj = field_i;
		}
		fieldCount++;
	}

	RecordType recInts = RecordType(attrListRegions);
	vector<RecordAttribute*> whichFields;

	whichFields.push_back(toProject);
	whichFields.push_back(selectProj);

	BinaryColPlugin *pgColumnar = new BinaryColPlugin(&ctx, filenamePrefix, recInts,
			whichFields);
	catalog.registerPlugin(filenamePrefix, pgColumnar);
	Scan scan = Scan(&ctx, *pgColumnar);

	/**
	 * "SELECT"
	 */
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(*toProject);
	projections.push_back(*selectProj);

	expressions::Expression* lhsArg = new expressions::InputArgument(&recInts, 0,
		projections);
	expressions::Expression* lhs = new expressions::RecordProjection(intType,
		lhsArg, *selectProj);
	expressions::Expression* rhs = new expressions::IntConstant(100000000);
	expressions::Expression* predicate = new expressions::LtExpression(
		new BoolType(), lhs, rhs);

	/**
	 * REDUCE
	 * (MAX)
	 */

	expressions::Expression* proj  = new expressions::RecordProjection(intType,lhsArg,*toProject);

	Reduce reduce = Reduce(MAX, proj, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	//Run function
	struct timespec t0, t1;
	clock_gettime(CLOCK_REALTIME, &t0);
	reduce.produce();
	ctx.prepareFunction(ctx.getGlobalFunction());
	clock_gettime(CLOCK_REALTIME, &t1);
	printf("Execution took %f seconds\n", diff(t0, t1));

	//Close all open files & clear
	pgColumnar->finish();
	catalog.clear();
}

void columnarMax3()
{
	string filenamePrefix = string("/cloud_store/manosk/data/vida-engine/synthetic/100m-30cols-fixed");

	RawContext ctx = RawContext("columnarMax3");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* intType = new IntType();

	string line;
	int fieldCount = 1;
	list<RecordAttribute*> attrListRegions;

	ostringstream sstream;
	RecordAttribute *selectProj1 = NULL;
	RecordAttribute *selectProj2 = NULL;
	RecordAttribute *toProject = NULL;
	for(int i = 1; i <= 30; i++)	{
		sstream.str("");
		sstream << "field" << fieldCount;
		RecordAttribute *field_i = new RecordAttribute(fieldCount, filenamePrefix,sstream.str().c_str(),intType);
		attrListRegions.push_back(field_i);

		if(fieldCount == 10)	{
			toProject = field_i;
		}

		if(fieldCount == 15)	{
			selectProj2 = field_i;
		}

		if(fieldCount == 20)	{
			selectProj1 = field_i;
		}
		fieldCount++;
	}

	RecordType recInts = RecordType(attrListRegions);
	vector<RecordAttribute*> whichFields;

	whichFields.push_back(toProject);
	whichFields.push_back(selectProj1);
	whichFields.push_back(selectProj2);

	BinaryColPlugin *pgColumnar = new BinaryColPlugin(&ctx, filenamePrefix, recInts,
			whichFields);
	catalog.registerPlugin(filenamePrefix, pgColumnar);
	Scan scan = Scan(&ctx, *pgColumnar);

	/**
	 * "SELECT"
	 */
	list<RecordAttribute> projections = list<RecordAttribute>();
	projections.push_back(*toProject);
	projections.push_back(*selectProj1);
	projections.push_back(*selectProj2);

	expressions::Expression* lhsArg = new expressions::InputArgument(&recInts, 0,
		projections);
	expressions::Expression* lhs1 = new expressions::RecordProjection(intType,
		lhsArg, *selectProj1);
	expressions::Expression* rhs1 = new expressions::IntConstant(100000000);
	expressions::Expression* predicate1 = new expressions::LtExpression(
		new BoolType(), lhs1, rhs1);

	expressions::Expression* lhs2 = new expressions::RecordProjection(intType,
			lhsArg, *selectProj2);
	expressions::Expression* rhs2a = new expressions::IntConstant(200000000);
	expressions::Expression* predicate2a = new expressions::LtExpression(
			new BoolType(), lhs2, rhs2a);

	expressions::Expression* rhs2b = new expressions::IntConstant(100000000);
	expressions::Expression* predicate2b = new expressions::GtExpression(
				new BoolType(), lhs2, rhs2b);

	expressions::Expression* predicate2 = new expressions::AndExpression(
					new BoolType(), predicate2a, predicate2b);
	expressions::Expression* predicate = new expressions::AndExpression(
						new BoolType(), predicate1, predicate2);
	/**
	 * REDUCE
	 * (MAX)
	 */

	expressions::Expression* proj  = new expressions::RecordProjection(intType,lhsArg,*toProject);

	Reduce reduce = Reduce(MAX, proj, predicate, &scan, &ctx);
	scan.setParent(&reduce);

	//Run function
	struct timespec t0, t1;
	clock_gettime(CLOCK_REALTIME, &t0);
	reduce.produce();
	ctx.prepareFunction(ctx.getGlobalFunction());
	clock_gettime(CLOCK_REALTIME, &t1);
	printf("Execution took %f seconds\n", diff(t0, t1));

	//Close all open files & clear
	pgColumnar->finish();
	catalog.clear();
}

void columnarJoin1()
{
	RawContext ctx = RawContext("columnarJoin1");
	RawCatalog& catalog = RawCatalog::getInstance();
	PrimitiveType* intType = new IntType();

//	string filenamePrefixLeft = string("/cloud_store/manosk/data/vida-engine/synthetic/100m-30cols-fixed-shuffled");
//	string filenamePrefixRight = string("/cloud_store/manosk/data/vida-engine/synthetic/500m-30cols-fixed");

	string filenamePrefixLeft = string("/cloud_store/manosk/data/vida-engine/synthetic/100-30cols-fixed");
	string filenamePrefixRight = string("/cloud_store/manosk/data/vida-engine/synthetic/500-30cols-fixed");

	/**
	 * SCAN1
	 */

	int fieldCount = 1;
	list<RecordAttribute*> attrListLeft;

	ostringstream sstream;
	RecordAttribute *selectProjLeft1 = NULL;
	RecordAttribute *selectProjLeft2 = NULL;
	RecordAttribute *toProjectLeft = NULL;
	for (int i = 1; i <= 30; i++)
	{
		sstream.str("");
		sstream << "field" << fieldCount;
		RecordAttribute *field_i = new RecordAttribute(fieldCount,
				filenamePrefixLeft, sstream.str().c_str(), intType);
		attrListLeft.push_back(field_i);

		if (fieldCount == 10)
		{
			toProjectLeft = field_i;
		}

//		if (fieldCount == 15)
//		{
//			selectProj2 = field_i;
//		}

		if (fieldCount == 20)
		{
			selectProjLeft1 = field_i;
		}
		fieldCount++;
	}

	RecordType recIntsLeft = RecordType(attrListLeft);
	vector<RecordAttribute*> whichFieldsLeft;

	whichFieldsLeft.push_back(toProjectLeft);
	whichFieldsLeft.push_back(selectProjLeft1);
//	whichFieldsLeft.push_back(selectProjLeft2);

	BinaryColPlugin *pgColumnarLeft = new BinaryColPlugin(&ctx, filenamePrefixLeft,
			recIntsLeft, whichFieldsLeft);
	catalog.registerPlugin(filenamePrefixLeft, pgColumnarLeft);
	Scan scanLeft = Scan(&ctx, *pgColumnarLeft);

	/**
	 * SELECT
	 */
	list<RecordAttribute> projectionsLeft = list<RecordAttribute>();
	projectionsLeft.push_back(*toProjectLeft);
	projectionsLeft.push_back(*selectProjLeft1);

	expressions::Expression* lhsArg = new expressions::InputArgument(&recIntsLeft, 0,
		projectionsLeft);
	expressions::Expression* lhs = new expressions::RecordProjection(intType,
		lhsArg, *selectProjLeft1);
	expressions::Expression* rhs = new expressions::IntConstant(100000000);
	expressions::Expression* predicate = new expressions::LtExpression(
		new BoolType(), lhs, rhs);
	Select sel = Select(predicate, &scanLeft);
	scanLeft.setParent(&sel);

	LOG(INFO)<<"Left: "<<&sel;

	/**
	 * SCAN2
	 */

	fieldCount = 1;
	list<RecordAttribute*> attrListRight;

	RecordAttribute *selectProjRight1 = NULL;
	RecordAttribute *selectProjRight2 = NULL;
	RecordAttribute *toProjectRight = NULL;
	for (int i = 1; i <= 30; i++)
	{
		sstream.str("");
		sstream << "field" << fieldCount;
		RecordAttribute *field_i = new RecordAttribute(fieldCount,
				filenamePrefixRight, sstream.str().c_str(), intType);
		attrListRight.push_back(field_i);

		if (fieldCount == 10)
		{
			toProjectRight = field_i;
		}

		//		if (fieldCount == 15)
		//		{
		//			selectProj2 = field_i;
		//		}

		if (fieldCount == 20)
		{
			selectProjRight1 = field_i;
		}
		fieldCount++;
	}

	RecordType recIntsRight = RecordType(attrListRight);
	vector<RecordAttribute*> whichFieldsRight;

	whichFieldsRight.push_back(toProjectRight);
	whichFieldsRight.push_back(selectProjRight1);
	//	whichFields.push_back(selectProjRight2);

	BinaryColPlugin *pgColumnarRight = new BinaryColPlugin(&ctx,
			filenamePrefixRight, recIntsRight, whichFieldsRight);
	catalog.registerPlugin(filenamePrefixRight, pgColumnarRight);
	Scan scanRight = Scan(&ctx, *pgColumnarRight);

	list<RecordAttribute> projectionsRight = list<RecordAttribute>();
	projectionsRight.push_back(*toProjectRight);
	projectionsRight.push_back(*selectProjRight1);

	/**
	 * JOIN
	 */
	expressions::Expression* leftJoinArg = new expressions::InputArgument(intType,
			0, projectionsLeft);
	expressions::Expression* left = new expressions::RecordProjection(intType,
			leftJoinArg, *toProjectLeft);
	expressions::Expression* rightJoinArg = new expressions::InputArgument(intType,
			1, projectionsRight);
	expressions::Expression* right = new expressions::RecordProjection(intType,
			rightJoinArg, *toProjectRight);
	expressions::BinaryExpression* joinPred = new expressions::EqExpression(
			new BoolType(), left, right);
	vector<materialization_mode> outputModes;
	outputModes.insert(outputModes.begin(), EAGER);
	outputModes.insert(outputModes.begin(), EAGER);
	Materializer* mat = new Materializer(whichFieldsLeft, outputModes);

	char joinLabel[] = "join1";
	Join join = Join(joinPred, sel, scanRight, joinLabel, *mat);
	sel.setParent(&join);
	scanRight.setParent(&join);

	/**
	 * REDUCE
	 * (MAX)
	 */

	expressions::RecordProjection* outputExpr =
		new expressions::RecordProjection(intType, rightJoinArg, *selectProjLeft1);
	expressions::Expression* val_true = new expressions::BoolConstant(1);
	expressions::Expression* reducePredicate = new expressions::EqExpression(
		new BoolType(), val_true, val_true);
	Reduce reduce = Reduce(MAX, outputExpr, reducePredicate, &join, &ctx);
	join.setParent(&reduce);

	reduce.produce();

	//Run function
	ctx.prepareFunction(ctx.getGlobalFunction());

	//Close all open files & clear
	pgColumnarLeft->finish();
	pgColumnarRight->finish();
	catalog.clear();
}
