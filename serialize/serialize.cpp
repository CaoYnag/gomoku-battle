#include "serialize.h"
#include <nlohmann/json.hpp>
using namespace rttr;
using namespace std;
using namespace nlohmann;


void serialize_recur(const instance& obj, json& j);
bool write_value(const variant& v, json& j, const char* name = nullptr);

template<class T>
void write_json(json& j, const T& value, const char* name = nullptr)
{
	if (name)
		j[name] = value;
	else
		j.emplace_back(value);
}

bool write_atomic_types(const type& t, const variant& v, json& j, const char* name = nullptr)
{
	if (t.is_arithmetic())
	{
		if (t == type::get<bool>())
			write_json(j, v.to_bool(), name);
		else if (t == type::get<char>())
			write_json(j, v.to_bool(), name);
		else if (t == type::get<int8_t>())
			write_json(j, v.to_int8(), name);
		else if (t == type::get<int16_t>())
			write_json(j, v.to_int16(), name);
		else if (t == type::get<int32_t>())
			write_json(j, v.to_int32(), name);
		else if (t == type::get<int64_t>())
			write_json(j, v.to_int64(), name);
		else if (t == type::get<uint8_t>())
			write_json(j, v.to_uint8(), name);
		else if (t == type::get<uint16_t>())
			write_json(j, v.to_uint16(), name);
		else if (t == type::get<uint32_t>())
			write_json(j, v.to_uint32(), name);
		else if (t == type::get<uint64_t>())
			write_json(j, v.to_uint64(), name);
		else if (t == type::get<float>())
			write_json(j, v.to_double(), name);
		else if (t == type::get<double>())
			write_json(j, v.to_double(), name);

		return true;
	}
	else if (t.is_enumeration())
	{
		bool ok = false;
		auto result = v.to_string(&ok);
		if (ok)
			write_json(j, v.to_string(), name);
		else
		{
			ok = false;
			auto value = v.to_uint64(&ok);
			if (ok)
				write_json(j, value, name);
			else
				write_json(j, nullptr, name);
		}

		return true;
	}
	else if (t == type::get<std::string>())
	{
		write_json(j, v.to_string(), name);
		return true;
	}

	return false;
}

bool write_array(const variant_sequential_view& view, json& j)
{
	json tmp;
	for (const auto& item : view)
	{
		if (item.is_sequential_container())
		{
			json sub;
			write_array(item.create_sequential_view(), sub);
			write_json(tmp, sub);
		}
		else
		{
			const variant& wrapped = item.extract_wrapped_value();
			type vtype = wrapped.get_type();

			if (vtype.is_arithmetic() || vtype == type::get<std::string>() || vtype.is_enumeration())
				write_atomic_types(vtype, wrapped, tmp);
			else
				serialize_recur(wrapped, tmp);
		}
		j.emplace_back(tmp);
	}
	return true;
}

static void write_associative_container(const variant_associative_view& view, json& j, const char* name = nullptr)
{
	auto key_type = view.get_key_type();
	if (view.is_key_only_type())
	{
		for (auto& item : view)
			write_value(item.first, j, name);
	}
	else if (key_type.is_arithmetic() || key_type.is_enumeration() || key_type == type::get<std::string>())
	{
		json sub;
		for (auto& item : view)
		{
			bool ok = false;
			string key = item.first.convert<string>(&ok);
			if (!ok) continue;
			write_value(item.second, sub, key.c_str());
			write_json(j, sub, name);
		}
		write_json(j, sub, name);
	}
	else
	{
		json arr;
		for (auto& item : view)
		{
			json sub;
			// NOTICE here, convert map to {'key': 'key name', 'value':'values'}
			write_value(item.first, sub, io::serialize::ASSOCIATE_KEY);
			write_value(item.second, sub, io::serialize::ASSOCIATE_VALUE);
			write_json(arr, sub);
		}
		write_json(j, arr, name);
	}
}

bool write_value(const variant& v, json& j, const char* name)
{
	auto vtype = v.get_type();
	auto rt = vtype.is_wrapper() ? vtype.get_wrapped_type() : vtype;
	bool is_pointer = vtype.is_pointer() || vtype == type::get<std::shared_ptr<string>>();
	bool is_wrapper = rt != vtype;
	auto rv = is_wrapper ? v.extract_wrapped_value() : v;

	if (is_pointer) printf("got pointer %s.\n", rt.get_name()); // TODO
	if (write_atomic_types(rt, rv, j, name));
	else if (rv.is_sequential_container())
	{
		json arr;
		write_array(rv.create_sequential_view(), arr);
		write_json(j, arr, name);
	}
	else if (rv.is_associative_container())
		write_associative_container(rv.create_associative_view(), j, name);
	else
	{
		auto cprops = rt.get_properties();
		if (cprops.empty())
		{
			bool ok = false;
			auto text = rv.to_string(&ok);
			write_json(j, text, name);
			if(!ok) return false;
		}
		else
		{
			json sub;
			serialize_recur(rv, sub);
			write_json(j, sub, name);
		}
	}
	return true;
}

void serialize_recur(const instance& obj, json& j)
{
	instance o = obj.get_type().get_raw_type().is_wrapper() ? obj.get_wrapped_instance() : obj;
	auto props = o.get_derived_type().get_properties();
	for (auto prop : props)
	{
		if (prop.get_metadata("NO_SERIALIZE")) continue;
		variant value = prop.get_value(o);
		if (!value) continue;

		const auto name = prop.get_name();
		write_value(value, j, name.data());
	}
}

std::string io::serialize::serialize(const instance& obj, int dump)
{
	if (!obj.is_valid()) return string();
	json j;
	serialize_recur(obj, j);
	return j.dump(dump);
}