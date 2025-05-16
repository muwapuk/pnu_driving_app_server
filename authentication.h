#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <httpserver.hpp>
#include <appdatabase_structs.h>
using namespace httpserver;

#define MY_OPAQUE "73e123a167237bb3423634af23c3ad6786fac7ed"

namespace auth {


std::string genRandomString(const int len);

///
/// signUp - creates user.
/// req - request.
/// return:
///     0 - user created,
///     1 - user exist,
///     2 - bad json format
///
int signUp(const http_request &req);

///
/// signIn - generates token with linked permissions for requestor if authorized.
/// return:
///     0 - user authorized,
///     1 - user not exist,
///     2 - incorrect password
///
int signIn(const http_request &req, bool &reload_nonce, std::string &token);

///
/// signOut - removes user ip from remembered
/// req - request
/// return:
///

bool checkPassword(const http_request &req, bool &reload_nonce);

User::Permissions getPermissionsByToken(std::string token);

}

/*
    bool reload_nonce = false;
    if (authenticate(req, reload_nonce))
        return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL", "", MY_OPAQUE, reload_nonce));
*/

#endif // AUTHENTICATION_H
