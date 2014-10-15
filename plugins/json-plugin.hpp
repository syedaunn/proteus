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

#include "plugins/plugins.hpp"

namespace semi_index	{

class JSONPlugin	: public Plugin {
public:
	JSONPlugin(RawContext* const context, string& file, vector<RecordAttribute*>* fieldsToSelect, vector<RecordAttribute*>* fieldsToProject);
	~JSONPlugin();
	void init();
	void generate(const RawOperator& producer);
	void finish();
	virtual string& getName() { return fname; }



private:
	JSONHelper* helper;
	vector<RecordAttribute*>* attsToSelect;
	vector<RecordAttribute*>* attsToProject;
	string& fname;

	//Code-generation-related
	//Used to store memory positions of offset, buf and filesize in the generated code
	std::map<std::string, AllocaInst*> NamedValuesJSON;
	RawContext* const context;

	//Assumes a semi-index has been pre-built during construction of JSONHelper
	void scanJSON(const RawOperator& producer, Function* debug);
};

}
