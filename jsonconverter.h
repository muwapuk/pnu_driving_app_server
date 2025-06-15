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

    static Result jsonStringToJsonObject(const std::string &str, nlohmann::json &j);
    static Result jsonObjectToJsonString(nlohmann::json &j, std::string &str);
    static void mergeJsonObjects(nlohmann::json &j, const nlohmann::json &j_patch);

    // Standart types
    static Result jsonValueToString(const nlohmann::json &j, std::string key, std::string &out);
    static Result stringToJsonValue(std::string key, const std::string &obj, nlohmann::json &j);

    static Result jsonValueToInt(const nlohmann::json &j, std::string key, int &out);
    static Result intToJsonValue(std::string key, int obj, nlohmann::json &j);

    // Question
    static Result jsonToQuestion(nlohmann::json &, Question &);
    static Result questionToJson(const Question &, nlohmann::json &);

    // User
    // static Result jsonToUser(const nlohmann::json &, User &);
    // static Result userToJson(const User &, nlohmann::json &);

    // Lecture
    static Result jsonToLecture(nlohmann::json &, Lecture &);
    static Result lectureToJson(Lecture &, nlohmann::json &);

};

#endif // JSONCONVERTER_H
