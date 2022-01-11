#include "HttpUtility.h"
#include <sstream>

std::string UrlUtil::urlDecode(const std::string& urlEncodedString)
{
    std::stringstream input(urlEncodedString);
    std::string decodedString = "";

    while (!input.eof())
    {
        int c = input.get();

        if (c < 0 || c == '&')
            break;

        if (c == '+')
        {
            decodedString.push_back(' ');
        }
        else if (c == '%')
        {
            char octalCode[3];
            octalCode[0] = (char)input.get();
            octalCode[1] = (char)input.get();
            octalCode[2] = 0;
            int ch = std::stoi(octalCode, 0, 16);
            decodedString.push_back(ch);
        }
        else
            decodedString.push_back(c);
    }

    return std::move(decodedString);
}

const std::string& QueryParameter::firstValue() const
{
    if (valueCount() == 0)
        //Question 3: I am really not sure of the appropriate exception to throw here...
        throw std::runtime_error("Attempted to obtain first value of query string parameter that does not have any values");

    return values_[0];
}

QueryString::QueryString(std::istream& input)
{
    //Example:  param1=value1&param2=value2

    while (!input.eof() && input.peek() > 0)
    {
        //Should eat "param1="
        auto name = parseName(input);
        //Should eat value1&
        std::string value = parseValue(input);

        auto foundItr = parameters_.find(name);
        if (foundItr == parameters_.end())
        {
            auto newParam = std::make_unique<QueryParameter>(name);
            if (value.size() > 0)
            {
                newParam->addValue(value);
            }
            parameters_.emplace(name, std::move(newParam)).first;
        }
        else
            (*foundItr).second->addValue(value);
    }
}

std::vector<std::string> QueryString::getParamNames() const
{
    std::vector<std::string> names;
    for (auto const& pair : parameters_)
        names.push_back(pair.second->name());

    return std::move(names);
}

const QueryParameter* QueryString::getParam(const std::string& name) const
{
    auto itr = parameters_.find(name);
    return itr == parameters_.end() ? nullptr : (*itr).second.get();
};

std::string QueryString::parseName(std::istream& input)
{
    std::string name = "";

    while (!input.eof() && input.peek() != '=')
        name.push_back(input.get());

    //Eat the '='
    if (!input.eof())
        input.get();

    return name;
}

std::string QueryString::parseValue(std::istream& input)
{
    std::string urlEncodedValue;

    int c = input.get();
    while (c > 0 && c != '&')
    {
        urlEncodedValue.push_back(c);
        c = input.get();
    }

    if (urlEncodedValue.size() == 0)
        return "";

    std::string decodedValue = std::move(UrlUtil::urlDecode(urlEncodedValue));
    return std::move(decodedValue);
}

void Replies::reply_successed_text(mg_connection* connection, std::string message)
{
    mg_http_reply(
        connection,
        200,
        "Content-Type: application/text\r\n",
        message.c_str());
}

void Replies::reply_successed_json(mg_connection* connection, std::string json)
{
    mg_http_reply(
        connection,
        200,
        "Content-Type: application/json\r\n",
        json.c_str());
}

void Replies::reply_bad_request(mg_connection* connection, std::string message)
{
    mg_http_reply(
        connection,
        400,
        "Content-Type: application/text\r\n",
        message.c_str());
}
