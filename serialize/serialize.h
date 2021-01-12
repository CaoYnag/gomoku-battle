#pragma once
#include <rttr/type>
#include <string>
#include <stdexcept>

namespace io::serialize
{
	/*
	* serialze an obj to json
	*/
	std::string serialize(const rttr::instance&);
	/*
	* construct an obj from json
	*/
	bool deserialize(const std::string&, rttr::instance);

	class serialize_expt;

	extern const char* ASSOCIATE_KEY;
	extern const char* ASSOCIATE_VALUE;
};

class io::serialize::serialize_expt : public std::runtime_error
{};