#include "serialize.h"
#include <nlohmann/json.hpp>
#include <string>
using namespace rttr;
using namespace nlohmann;

void deserialize_recur(instance, const json&);

variant extract_basic_types(const json& jv)
{
	/*
	 null,             ///< null value
	object,           ///< object (unordered set of name/value pairs)
	array,            ///< array (ordered collection of values)
	string,           ///< string value
	boolean,          ///< boolean value
	number_integer,   ///< number value (signed integer)
	number_unsigned,  ///< number value (unsigned integer)
	number_float,     ///< number value (floating-point)
	discarded         ///< discarded by the the parser callback function
	*/
	variant ret;
	switch (jv.type())
	{
	case json::value_t::string: 
	{
		std::string str = jv;
		ret = str;
	}break;
	case json::value_t::boolean: 
	{
		bool b = jv;
		ret = b;
	}break;
	case json::value_t::number_integer: 
	{
		int64_t i = jv;
		ret = i;
	}break;
	case json::value_t::number_unsigned: 
	{
		uint64_t u = jv;
		ret = u;
	}break;
	case json::value_t::number_float: 
	{
		double d = jv;
		ret = d;
	}break;
	default:
		break;
	}
	return ret;
}

void write_array_recur(variant_sequential_view& view, const json& j)
{
	auto size = j.size();
	view.set_size(size);
	const type vt = view.get_rank_type(1); // TODO why 1? indeed starts with 0?
	
	for (decltype(size) i = 0; i < size; ++i)
	{
		auto& jv = j[i];
		if (jv.is_array())
		{
			auto sub_arr_view = view.get_value(i).create_sequential_view();
			write_array_recur(sub_arr_view, jv);
		}
		else if (jv.is_object())
		{
			auto sub = view.get_value(i);
			auto wrapped = sub.extract_wrapped_value();
			deserialize_recur(sub, jv);
			view.set_value(i, sub);
		}
		else
		{
			auto var = extract_basic_types(jv);
			if (var.convert(vt)) view.set_value(i, var);
		}
	}
}

variant extract_value(const json& j, const type& t)
{
	auto rt = t.is_wrapper() ? t.get_wrapped_type() : t;
	auto value = extract_basic_types(j);
	const bool can_convert = value.convert((rttr::type)rt);
	if (!can_convert)
	{
		if (j.is_object())
		{
			constructor ctor = rt.get_constructor();
			for (auto& ictor : rt.get_constructors())
				if (ictor.get_instantiated_type() == rt)
					ctor = ictor; // ???

			value = ctor.invoke(); // ???
			deserialize_recur(value, j);
		}
	}
	return value;
}

void write_associate_recur(variant_associative_view& view, const json& j)
{
	auto size = j.size();
	auto key_type = view.get_key_type();
	auto value_type = view.get_value_type();

	json jkey, jvalue;
	if (view.is_key_only_type() || key_type.is_arithmetic() || key_type.is_enumeration() || key_type == type::get<std::string>())
	{
		for (auto it = j.begin(); it != j.end(); ++it)
		{
			variant key = extract_value(it.key(), key_type);
			variant value = extract_value(it.value(), value_type);
			if (key) // key must be valid
				view.insert(key, value);
		}
	}
	else // complex strcuted 
	{
		for (auto e : j)
		{
			variant key = extract_value(e[io::serialize::ASSOCIATE_KEY], key_type);
			variant value = extract_value(e[io::serialize::ASSOCIATE_VALUE], value_type);
			if (key && value)
				view.insert(key, value);
		}
	}
}

void deserialize_recur(instance obj, const json& j)
{
	instance o = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
	const auto props = o.get_derived_type().get_properties();
	
	for (auto prop : props)
	{
		auto jv = j[prop.get_name().data()];
		const type pt = prop.get_type();
		auto var = prop.get_value(o);

		if (pt.is_sequential_container())
		{
			auto view = var.create_sequential_view();
			write_array_recur(view, jv);
			prop.set_value(o, var);
		}
		else if (pt.is_associative_container())
		{
			auto view = var.create_associative_view();
			write_associate_recur(view, jv);
			prop.set_value(o, var);
		}
		else if (jv.is_object())
		{
			// var maybe a pointer here
			if(pt.is_pointer())
				var = pt.create();
			deserialize_recur(var, jv);
			prop.set_value(o, var);
		}
		else
		{
			var = extract_basic_types(jv);
			if (var.convert(pt))
				prop.set_value(o, var);
		}
	}
}

bool io::serialize::deserialize(const std::string& json_str, instance obj)
{
	try
	{
		auto j = json::parse(json_str);
		deserialize_recur(obj, j);
		return true;
	}
	catch (std::exception&)
	{
		return false;
	}
}