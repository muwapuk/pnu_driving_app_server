#ifndef JSONCONVERTER_H
#define JSONCONVERTER_H

#include <string>
#include <sys/types.h>
#include <nlohmann/json.hpp>

#include "appdatabase_structs.h"

// Converts data from clients from json to the database specific structs and vice versa
class JsonConverter
{
public:
    enum Result {
        SUCCESS,
        PARSE_ERROR,
        CONVERTATION_ERROR
    };

    static Result stringToJson(const std::string &str, nlohmann::json &j);
    static Result jsonToJsonString(nlohmann::json &j, std::string &str);

    static Result jsonToQuestion(nlohmann::json &, Question &);
    static Result questionToJson(const Question &, nlohmann::json &);

    static Result jsonToUser(nlohmann::json &, User &);
    static Result userToJson(const User &, nlohmann::json &);

    // NOT IMPLEMENTED

    static Result stringToJson(std::string key, const std::string &obj, nlohmann::json &j);
    static Result intToJson(std::string key, int obj, nlohmann::json &j);

    static Result jsonObjectToString(nlohmann::json &j, std::string key, std::string &out);
    static Result jsonObjectToInt(nlohmann::json &j, std::string key, int &out);
};

#endif // JSONCONVERTER_H
