#pragma once
#include <string>
#include <memory>

namespace spes::conf
{
    class Conf;
    class ConfArray;

    std::shared_ptr<Conf> get(const std::string& file);
    std::shared_ptr<ConfArray> get_array(const std::string& file);
}

class spes::conf::Conf
{
public:
    virtual ~Conf();
    virtual std::string Get(const std::string& key) const = 0;
    virtual int GetInt(const std::string& key) const;
    virtual double GetDouble(const std::string& key) const;
    virtual bool GetBool(const std::string& key) const;

    virtual std::shared_ptr<ConfArray> GetArray(const std::string& key) const = 0;
    virtual std::shared_ptr<Conf> GetConf(const std::string& key) const = 0;
};

class spes::conf::ConfArray
{
public:
    virtual ~ConfArray();

    virtual std::string Get(int idx) const = 0;
    virtual int GetInt(int idx) const;
    virtual double GetDouble(int idx) const;
    virtual bool GetBool(int idx) const;
    virtual int length() const = 0;

    virtual std::shared_ptr<ConfArray> GetArray(int idx) const = 0;
    virtual std::shared_ptr<Conf> GetConf(int idx) const = 0;
};
