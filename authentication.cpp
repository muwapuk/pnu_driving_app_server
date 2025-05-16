#include "authentication.h"
#include "appdatabase.h"
#include "jsonconverter.h"


std::shared_ptr<http_response> auth::authorization_resource::render_POST(const http_request& req) {
    bool reload_nonce = false;
    std::string token;
    int signResult = signIn(req, reload_nonce, token);

    if (signResult == 1) {
        return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL", "", MY_OPAQUE, true));
    }
    else {

        if(!req.check_digest_auth("test@example.com", "mypass", 300, reload_nonce)) {
            return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL", "test@example.com", MY_OPAQUE, reload_nonce));
        }
    }
    return std::shared_ptr<string_response>(new string_response("SUCCESS", 200, "text/plain"));
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
    User::Permissions perm = AppDB()->getUserPermissions(std::string(req.get_digested_user()));
    if(perm == User::NONE)
        return 1;

    std::string(req.get_digested_user());
    std::string(req.get_requestor());

    if(checkPassword(req, reload_nonce)) {
        return 2;
    }

    token = genRandomString(64);
    AppDB()->addToken(token, std::string(req.get_digested_user()), perm);

    return 0;
}



bool auth::checkPassword(const http_request &req, bool &reload_nonce)
{
    auto pass = AppDB()->getUserPassword(std::string(req.get_digested_user()));
    bool check = true;
    if (!req.check_digest_auth("", *pass, 300, &reload_nonce)) {
        check = false;
    }
    delete pass;

    return check;
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

User::Permissions auth::getPermissionsByToken(std::string token)
{
    return AppDB()->getPermissionsByToken(token);
}
