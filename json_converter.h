#ifndef JSON_CONVERTER_H
#define JSON_CONVERTER_H

#include <string>
#include <sys/types.h>
#include <nlohmann/json.hpp>

#include "appdatabase_structs.h"

// Converts data from clients from json to the database specific structs and vice versa
class json_converter
{
public:
    static Question *jsonStringToQuestion(std::string &);
    static std::string *questionToJsonString(Question &);

    static User *jsonStringToUser(std::string &);
    static std::string *userToJsonString(User &);
};

#endif // JSON_CONVERTER_H
