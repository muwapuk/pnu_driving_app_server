#include "authentication.h"
#include "appdatabase.h"

bool authenticate(const httpserver::http_request &req, bool &reload_nonce)
{
    if (!isUserIpRemembered(req)) {
        if (isUserExist(req)) {
            return false;
        }
        if (checkPassword(req, reload_nonce)) {
            return false;
        }
    }
    return true;
}

bool isUserIpRemembered(const httpserver::http_request &req)
{
    auto ip = AppDB()->getUserIp(std::string(req.get_digested_user()));
    if (ip != nullptr) {
        delete ip;
        return true;
    }
    return false;
}

bool isUserExist(const httpserver::http_request &req)
{
    auto user = AppDB()->getUser(std::string(req.get_digested_user()));
    if (user != nullptr) {
        delete user;
        return true;
    }
    return false;
}

bool checkPassword(const httpserver::http_request &req, bool &reload_nonce)
{
    auto pass = AppDB()->getUserPassword(std::string(req.get_digested_user()));
    bool check = true;

    if (!req.check_digest_auth("", *pass, 300, &reload_nonce)) {
        check = false;
    }
    delete pass;

    return check;
}
