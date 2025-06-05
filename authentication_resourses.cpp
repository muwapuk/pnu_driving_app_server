#include "authentication_resourses.h"
#include "appdatabase.h"
#include "jsonconverter.h"
#include "appwebserver.h"

#include <iostream>


std::shared_ptr<http_response> auth::signIn_resource::render_GET(const http_request& req)
{
    AppWebserver::printDebug(req);

    bool reload_nonce = false;
    std::string token;

    int signResult = signIn(req, reload_nonce, token);

    if (signResult == 1) {
        return std::shared_ptr<basic_auth_fail_response>(new basic_auth_fail_response("User not found!", "", 401));
    }
    if(signResult == 2) {
        return std::shared_ptr<basic_auth_fail_response>(new basic_auth_fail_response("Incorrect password!", "", 401));
    }
    auto responce = std::shared_ptr<http_response>(new string_response("SUCCESS"));
    responce->with_header("token", token);
    return responce;
}

std::shared_ptr<http_response> auth::signUp_resource::render_POST(const http_request& req)
{
    AppWebserver::printDebug(req);

    int result = signUp(req);

    // Parse error
    if (result == 1) {
        return std::shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    }
    // Json struct error
    if (result == 2) {
        return std::shared_ptr<http_response>(new string_response("JSON struct does not match user struct!", 400));
    }
    // Insert error
    if(result == 3) {
        return std::shared_ptr<http_response>(new string_response("User already exist!", 409));
    }
    // Input data error
    if(result == 4) {
        return std::shared_ptr<http_response>(new string_response("Input error!", 400));
    }
    return std::shared_ptr<http_response>(new string_response("SUCCESS"));
}



int auth::signUp(const http_request &req)
{
    User newUser;
    std::string jsonStr = std::string(req.get_content());
    nlohmann::json json;

    if(JsonConverter::SUCCESS != JsonConverter::jsonStringToJsonObject(jsonStr, json)) {
        return 1;
    }
    try {
        newUser.name = json["name"];
        newUser.login = json["login"];
        newUser.password = json["password"];
    } catch (nlohmann::json::exception) {
        return 2;
    }
    if (newUser.name.size() <= 0
     || newUser.login.size() < 6
     || newUser.password.size() < 6) {
        return 4;
    }
    newUser.permissions = User::STUDENT;

    if(!AppDB().insertUser(newUser)) {
        return 3;
    }

    return 0;
}

int auth::signIn(const http_request &req, bool &reload_nonce, std::string &token)
{
    if(!AppDB().isUserExist(std::string(req.get_user())))
        return 1;
    if(!checkPassword(req, reload_nonce)) {
        return 2;
    }

    token = genRandomString(64);
    AppDB().addToken(token, std::string(req.get_user()));

    return 0;
}


bool auth::checkPassword(const http_request &req, bool &reload_nonce)
{
    auto pass = AppDB().getUserPassword(std::string(req.get_user()));
    bool correctPass = true;

    if (*pass != req.get_pass())
        correctPass = false;

    // DIGEST
    // if (!req.check_digest_auth("", *pass, 300, &reload_nonce)) {
    //     correctPass = false;
    // }

    return correctPass;
}

std::string auth::genRandomString(const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmpStr;
    tmpStr.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmpStr += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmpStr;
}

std::shared_ptr<pair<string, User::Permissions>> auth::tokenToUserAndPermenissions(std::string token)
{
    return AppDB().getLoginAndPermissionsByToken(token);
}
