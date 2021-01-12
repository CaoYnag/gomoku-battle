#include "config.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
using namespace spes::conf;
using namespace std;
using json = nlohmann::json;

namespace spes::conf
{
    class JsonConf;
}

Conf::~Conf(){}

int Conf::GetInt(const string& key) const
{
    return atoi(Get(key).c_str());
}

double Conf::GetDouble(const string& key) const
{
    return atof(Get(key).c_str());
}

bool Conf::GetBool(const string& key) const
{
    string&& v = Get(key);
    transform(v.begin(), v.end(), v.begin(), ::tolower);
    return v == "true";
}

ConfArray::~ConfArray()
{}

int ConfArray::GetInt(int idx) const
{
    return atoi(Get(idx).c_str());
}

double ConfArray::GetDouble(int idx) const
{
    return atof(Get(idx).c_str());
}

bool ConfArray::GetBool(int idx) const
{
    string&& v = Get(idx);
    transform(v.begin(), v.end(), v.begin(), ::tolower);
    return v == "true";
}


class spes::conf::JsonConf : public spes::conf::Conf, public spes::conf::ConfArray
{
    json _json;
public:
    JsonConf(const string& file)
    {
        ifstream is(file.c_str());
        is >> _json;
        is.close();
    }
    // x is useless, but when call JsonConf(json[key]), Compiler will first match JsonConf(string)
    // so add a useless var to force compiler use this constructor
    JsonConf(const json& j, bool token = true) : _json(j)
    {}
    virtual ~JsonConf()
    {}

    /* interfaces of spes::conf::Conf */
    virtual string Get(const string& key) const
    {
        return _json[key];
    }
    virtual int GetInt(const std::string& key) const
    {
        return _json[key];
    }
    virtual double GetDouble(const std::string& key) const
    {
        return _json[key];
    }
    virtual bool GetBool(const std::string& key) const
    {
        return _json[key];
    }
    virtual std::shared_ptr<Conf> GetConf(const std::string& key) const
    {
        return std::make_shared<JsonConf>(_json[key], true);
    }
    virtual std::shared_ptr<ConfArray> GetArray(const std::string& key) const
    {
        return std::make_shared<JsonConf>(_json[key], true);
    }

    /* interfaces of spes::conf::ConfArray */
    virtual string Get(int idx) const
    {
        return _json[idx];
    }
    virtual int GetInt(int idx) const
    {
        return _json[idx];
    }
    virtual double GetDouble(int idx) const
    {
        return _json[idx];
    }
    virtual bool GetBool(int idx) const
    {
        return _json[idx];
    }
    virtual int length() const
    {
        return _json.size();
    }
    virtual std::shared_ptr<Conf> GetConf(int idx) const
    {
        return std::make_shared<JsonConf>(_json[idx]);
    }
    virtual std::shared_ptr<ConfArray> GetArray(int idx) const
    {
        return std::make_shared<JsonConf>(_json[idx]);
    }
};

std::shared_ptr<Conf> spes::conf::get(const string& file)
{
    std::filesystem::path f(file);
    if(f.empty())
    {
        cerr << "Conf file is empty: " << file << endl;
        return nullptr;
    }
    string suffix = f.extension().generic_string();
    if(suffix == ".json")
        return std::make_shared<JsonConf>(file);
    return nullptr;
}

std::shared_ptr<ConfArray> spes::conf::get_array(const string& file)
{
    std::filesystem::path f(file);
    if(f.empty())
    {
        cerr << "Conf file is empty: " << file << endl;
        return nullptr;
    }
    string suffix = f.extension().generic_string();
    if(suffix == ".json")
        return std::make_shared<JsonConf>(file);
    return nullptr;
}
