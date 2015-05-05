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

#ifndef SYMANTEC_CONFIG_HPP_
#define SYMANTEC_CONFIG_HPP_

#include "common/common.hpp"
/* Constants and macros to be used by queries targetting dataset of spam emails */

//#define SYMANTEC_LOCAL
#ifndef SYMANTEC_LOCAL
#define SYMANTEC_SERVER
#endif

typedef struct dataset	{
	string path;
	RecordType recType;
	int linehint;
} dataset;


/* Crude schema, obtained from spams100.json */
void symantecSchema(map<string, dataset>& datasetCatalog) {
	IntType *intType = new IntType();
	FloatType *floatType = new FloatType();
	StringType *stringType = new StringType();

	dataset symantec;

	#ifdef SYMANTEC_LOCAL
	string path = string("inputs/json/spam/spams100.json");
	symantec.linehint = 100;
//	string path = string("inputs/json/spam/outliers.json");
//	symantec.linehint = 2;
	#endif
	#ifdef SYMANTEC_SERVER
	//string path = string("/cloud_store/manosk/data/vida-engine/symantec/spams_2013_06_09.json");
	//symantec.linehint = 55836625;

//	string path = string("/cloud_store/manosk/data/vida-engine/symantec/spams_60k.json");
//	symantec.linehint = 55834736;

//	string path = string("/cloud_store/manosk/data/vida-engine/symantec/spams1m.json");
//	symantec.linehint = 1000000;
	string path = string("/cloud_store/manosk/data/vida-engine/symantec/spams100k.json");
	symantec.linehint = 100000;

//	string path = string("/cloud_store/manosk/data/vida-engine/symantec/spams_head.json");
//	symantec.linehint = 100000;
	#endif
	symantec.path = path;

	list<RecordAttribute*> attsSymantec = list<RecordAttribute*>();

	/* "IP" : "83.149.45.128" */
	RecordAttribute *ip = new RecordAttribute(1, path, "IP", stringType);
	attsSymantec.push_back(ip);
	/* "_id" : { "$oid" : "4ebbd37d466e8b0b55000000" } */
	RecordAttribute *oid = new RecordAttribute(1, path, "$oid", stringType);
	list<RecordAttribute*> oidNested = list<RecordAttribute*>();
	oidNested.push_back(oid);
	RecordType *idRec = new RecordType(oidNested);
	RecordAttribute *_id = new RecordAttribute(2, path, "_id", idRec);
	attsSymantec.push_back(_id);
	/* "attach" : [ "whigmaleerie.jpg" ] (but tends to be empty) */
	ListType *attachList = new ListType(*stringType);
	RecordAttribute *attach = new RecordAttribute(3, path, "attach",
			attachList);
	attsSymantec.push_back(attach);
	/* "body_txt_a" : "blablabla" */
	RecordAttribute *body_txt_a = new RecordAttribute(4, path, "body_txt_a",
			stringType);
	attsSymantec.push_back(body_txt_a);
	/* "bot" : "Unclassified" */
	RecordAttribute *bot = new RecordAttribute(5, path, "bot", stringType);
	attsSymantec.push_back(bot);
	/* "charset" : "windows-1252" */
	RecordAttribute *charset = new RecordAttribute(6, path, "charset",
			stringType);
	attsSymantec.push_back(charset);
	/* "city" : "Ryazan" */
	RecordAttribute *city = new RecordAttribute(7, path, "city", stringType);
	attsSymantec.push_back(city);
	/* "classA" : "83" */
	RecordAttribute *classA = new RecordAttribute(8, path, "classA",
			stringType);
	attsSymantec.push_back(classA);
	/* "classB" : "83.149" */
	RecordAttribute *classB = new RecordAttribute(9, path, "classB",
			stringType);
	attsSymantec.push_back(classB);
	/* "classC" : "83.149" */
	RecordAttribute *classC = new RecordAttribute(10, path, "classC",
			stringType);
	attsSymantec.push_back(classC);
	/* "content_type" : [ "text/html", "text/plain" ] */
	ListType *contentList = new ListType(*stringType);
	RecordAttribute *content_type = new RecordAttribute(11, path,
			"content_type", contentList);
	attsSymantec.push_back(content_type);
	/* "country" : "Russian Federation" */
	RecordAttribute *country = new RecordAttribute(12, path, "country",
			stringType);
	attsSymantec.push_back(country);
	/* "country_code" : "RU" */
	RecordAttribute *country_code = new RecordAttribute(13, path,
			"country_code", stringType);
	attsSymantec.push_back(country_code);
	/* "cte" : "unknown" */
	RecordAttribute *cte = new RecordAttribute(14, path, "cte", stringType);
	attsSymantec.push_back(cte);
	/* "date" : { "$date" : 1285919417000 } */
	RecordAttribute *date_ = new RecordAttribute(1, path, "$date", floatType);
	list<RecordAttribute*> dateNested = list<RecordAttribute*>();
	dateNested.push_back(date_);
	RecordType *dateRec = new RecordType(dateNested);
	RecordAttribute *date = new RecordAttribute(15, path, "date", dateRec);
	attsSymantec.push_back(date);
	/* "day" : "2010-10-01" */
	RecordAttribute *day = new RecordAttribute(16, path, "day", stringType);
	attsSymantec.push_back(day);
	/* "from_domain" : "domain733674.com" */
	RecordAttribute *from_domain = new RecordAttribute(17, path, "from_domain",
			stringType);
	attsSymantec.push_back(from_domain);
	/* "host" : "airtelbroadband.in (but tends to be empty) */
	RecordAttribute *host = new RecordAttribute(18, path, "host", stringType);
	attsSymantec.push_back(host);
	/* "lang" : "english" */
	RecordAttribute *lang = new RecordAttribute(19, path, "lang", stringType);
	attsSymantec.push_back(lang);
	/* "lat" : 54.6197 */
	RecordAttribute *lat = new RecordAttribute(20, path, "lat", floatType);
	attsSymantec.push_back(lat);
	/* "long" : 39.74 */
	RecordAttribute *long_ = new RecordAttribute(21, path, "long", floatType);
	attsSymantec.push_back(long_);
	/* "rcpt_domain" : "domain555065.com" */
	RecordAttribute *rcpt_domain = new RecordAttribute(22, path, "rcpt_domain",
			stringType);
	attsSymantec.push_back(rcpt_domain);
	/* "size" : 3712 */
	RecordAttribute *size = new RecordAttribute(23, path, "size", intType);
	attsSymantec.push_back(size);
	/* "subject" : "LinkedIn Messages, 9/30/2010" */
	RecordAttribute *subject = new RecordAttribute(24, path, "subject",
			stringType);
	attsSymantec.push_back(subject);
	/* "uri" : [ "http://hetfonteintje.com/1.html" ] */
	ListType *uriList = new ListType(*stringType);
	RecordAttribute *uri = new RecordAttribute(25, path, "uri", uriList);
	attsSymantec.push_back(uri);
	/* "uri_domain" : [ "hetfonteintje.com" ] */
	ListType *domainList = new ListType(*stringType);
	RecordAttribute *domain = new RecordAttribute(26, path, "uriDomain",
			domainList);
	attsSymantec.push_back(domain);
	/* "uri_tld" : [ ".com" ] */
	ListType *tldList = new ListType(*stringType);
	RecordAttribute *uri_tld = new RecordAttribute(27, path, "uri_tld",
			tldList);
	attsSymantec.push_back(uri_tld);
	/* "x_p0f_detail" : "XP/2000" */
	RecordAttribute *x_p0f_detail = new RecordAttribute(28, path,
			"x_p0f_detail", stringType);
	attsSymantec.push_back(x_p0f_detail);
	/* "x_p0f_genre" : "Windows" */
	RecordAttribute *x_p0f_genre = new RecordAttribute(29, path, "x_p0f_genre",
			stringType);
	attsSymantec.push_back(x_p0f_genre);

	/* "x_p0f_signature" : "64380:116:1:48:M1460,N,N,S:." */
	RecordAttribute *x_p0f_signature = new RecordAttribute(29, path,
			"x_p0f_signature", stringType);
	attsSymantec.push_back(x_p0f_signature);

	RecordType symantecRec = RecordType(attsSymantec);
	symantec.recType = symantecRec;

	datasetCatalog["symantec"] = symantec;
}

#endif /* SYMANTEC_CONFIG_HPP_ */
