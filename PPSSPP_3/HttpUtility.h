#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

extern "C" {
#include "mongoose.h"
}

class UrlUtil final
{
public:
    static std::string urlDecode(const std::string& urlEncodedString);
};

class QueryParameter final
{
public:
    const std::string& name() const { return name_; }
    const std::string& firstValue() const;

    //Question 1: because this is a const&, this is not bad practice, correct?
    const std::vector<std::string>& allValues() const { return values_; }

    int valueCount() const { return values_.size(); }

    QueryParameter(const std::string name) : name_(std::move(name)) {   }

private:
    friend class QueryString;

    void addValue(std::string value) { values_.push_back(value); }

    std::string name_;
    std::vector<std::string> values_;
};

class QueryString final
{
public:
    QueryString(std::istream& input);

    int numParams() const { return parameters_.size(); }
    bool hasParam(const char* name) const { return parameters_.find(std::string(name)) != parameters_.end(); }

    std::vector<std::string> getParamNames() const;

    const QueryParameter* getParam(const std::string& name) const;
    const QueryParameter* getParam(const char* name) const { return getParam(std::string(name)); };

private:

    static std::string parseName(std::istream& input);
    static std::string parseValue(std::istream& input);

    std::map<std::string, std::unique_ptr<QueryParameter>> parameters_;
};

class Replies final
{
public:
    static void reply_successed_text(
        struct mg_connection* connection,
        std::string message);

    static void reply_successed_json(
        struct mg_connection* connection,
        std::string json);

    static void reply_bad_request(
        struct mg_connection* connection,
        std::string message);
};