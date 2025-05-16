#include "authentication_resourses.h"
#include "appdatabase.h"
#include "jsonconverter.h"
#include <iostream>


std::shared_ptr<http_response> auth::signIn_resource::render_GET(const http_request& req) {
    bool reload_nonce = false;
    std::string token;

    int signResult = signIn(req, reload_nonce, token);

    if (signResult == 1) {
        return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("User not found!", "", MY_OPAQUE, false));
    }
    if(signResult == 2) {
        return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("Incorrect password!", "", MY_OPAQUE, reload_nonce));
    }
    auto responce = std::shared_ptr<string_response>(new string_response("SUCCESS", 200, "text/plain"));
    responce->with_header("token", token);
    return responce;
}

std::shared_ptr<http_response> auth::signUp_resource::render_POST(const http_request& req) {
    std::string token;

    token = req.get_header("token");
}



int auth::signUp(const http_request &req)
{
    User newUser;
    nlohmann::json j;

    if(!JsonConverter::jsonStringToJson(std::string(req.get_content()), j)) {
        return false;
    }
    try {
        newUser.name = j["name"];
        newUser.login = j["login"];
        newUser.password = j["password"];
    } catch (nlohmann::json::exception e) {
        return 2;
    }
    newUser.permissions = User::STUDENT;

    if(!AppDB()->addUser(newUser)) {
        return 1;
    }

    return 0;
}

int auth::signIn(const http_request &req, bool &reload_nonce, std::string &token)
{
    if(!AppDB()->isUserExist(std::string(req.get_digested_user())))
        return 1;
    if(!checkPassword(req, reload_nonce)) {
        return 2;
    }

    token = genRandomString(64);
    AppDB()->addToken(token, std::string(req.get_digested_user()));

    return 0;
}


bool auth::checkPassword(const http_request &req, bool &reload_nonce)
{
    auto pass = AppDB()->getUserPassword(std::string(req.get_digested_user()));
    bool correctPass = true;
    if (!req.check_digest_auth("", *pass, 300, &reload_nonce)) {
        correctPass = false;
    }
    delete pass;

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

pair<string, User::Permissions> *auth::tokenToUserAndPermenissions(std::string token)
{
    return AppDB()->getLoginAndPermissionsByToken(token);
}
