#include "def.h"
#include "psb_cc.h"

#include "psb_cc_null.h"
#include "psb_cc_array.h"
#include "psb_cc_integer.h"
#include "psb_cc_string.h"
#include "psb_cc_decimal.h"
#include "psb_cc_resource.h"
#include "psb_cc_collection.h"
#include "psb_cc_object.h"
#include "psb_cc_boolean.h"

psb_cc::psb_cc() :_entries(NULL)
{

}


psb_cc::~psb_cc()
{

}

bool psb_cc::cc(Json::Value &src)
{
	_entries = pack(src);
	_entries->compile();

	return true;
}

psb_cc_base* psb_cc::get_entries()
{
	return _entries;
}

uint32_t psb_cc::add_string(string value)
{
	for (uint32_t i = 0; i < _string_table.size(); i++)
	{
		if (value == _string_table[i]) {
			return i;
		}
	}

	_string_table.push_back(value);

	return _string_table.size() - 1;
}

uint32_t psb_cc::add_names(string value)
{
	for (uint32_t i = 0; i < _name_table.size(); i++)
	{
		if (value == _name_table[i]) {
			return i;
		}
	}

	_name_table.push_back(value);

	return _name_table.size() - 1;
}

psb_cc_base* psb_cc::pack(Json::Value& source_code)
{
	psb_cc_base *base = NULL;

	if (source_code.type() == Json::nullValue) {
		return new psb_cc_null();
	}

	if (source_code.type() == Json::stringValue) {
		std::string str = source_code.asString();
		if (str.find("#resource#") != -1) {
			uint32_t resource_index = 0;
			sscanf_s(str.c_str(), "#resource#%u", &resource_index);
			//cout << str  <<endl;
			return new psb_cc_resource(resource_index);
		}
		else {
			return new psb_cc_string(this, str);
		}
	}

	if (source_code.type() == Json::uintValue || source_code.type() == Json::intValue) {
		return new psb_cc_integer(source_code.asUInt64());
	}

	if (source_code.type() == Json::realValue) {
		//check is double value
		long double s = source_code.asDouble();
		s = fabs(s);
		s *= 100000;
		s -= (uint64_t)s;

		if (s == 0.0) {
			return new psb_cc_decimal(source_code.asFloat());
		}
		else {
			return new psb_cc_decimal(source_code.asDouble());
		}
	}

	if (source_code.type() == Json::arrayValue) {
		return new psb_cc_collection(this, source_code);
	}

	if (source_code.type() == Json::objectValue) {
		return new psb_cc_object(this, source_code);
	}

	if (source_code.type() == Json::booleanValue) {
		return new psb_cc_boolean(source_code.asBool());
	}

	return base;
}