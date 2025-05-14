#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <httpserver.hpp>

using namespace httpserver;

#define MY_OPAQUE "73e123a167237bb3423634af23c3ad6786fac7ed"


bool authenticate(const httpserver::http_request &req, bool &reload_nonce);

bool isUserIpRemembered(const httpserver::http_request &req);
bool isUserExist(const httpserver::http_request &req);
bool checkPassword(const httpserver::http_request &req, bool &reload_nonce);

/*
    bool reload_nonce = false;
    if (authenticate(req, reload_nonce))
        return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL", "", MY_OPAQUE, reload_nonce));
*/

#endif // AUTHENTICATION_H
