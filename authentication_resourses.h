#ifndef AUTHENTICATION_RESOURSES_H
#define AUTHENTICATION_RESOURSES_H

#include "appdatabase.h"
#include <httpserver.hpp>
#include <appdatabase_structs.h>
using namespace httpserver;

#define MY_OPAQUE "73e123a167237bb3423634af23c3ad6786fac7ed"

namespace auth {

class signIn_resource : public httpserver::http_resource {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req);
};

class signUp_resource : public httpserver::http_resource {
public:
    std::shared_ptr<http_response> render_POST(const http_request& req);
};

std::string genRandomString(const int len);

///
/// signUp - creates user.
/// req - request.
/// return:
///     0 - user created,
///     1 - user exist,
///     2 - bad json format
int signUp(const http_request &req);

///
/// signIn - generates token with linked permissions for requestor if authorized.
/// return:
///     0 - user authorized,
///     1 - user not exist,
///     2 - incorrect password
int signIn(const http_request &req, bool &reload_nonce, std::string &token);

bool checkPassword(const http_request &req, bool &reload_nonce);

std::shared_ptr<pair<string, User::Permissions>> tokenToUserAndPermenissions(std::string token);

} // namespace end


#endif // AUTHENTICATION_RESOURSES_H
