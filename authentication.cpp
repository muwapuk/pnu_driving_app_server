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
    return AppDB()->getUserIp(std::string(req.get_digested_user()));
}

bool isUserExist(const httpserver::http_request &req)
{
    return AppDB()->getUser(std::string(req.get_digested_user()));
}

bool checkPassword(const httpserver::http_request &req, bool &reload_nonce)
{
    auto pass = AppDB()->getUserPassword(std::string(req.get_digested_user()));

    return !req.check_digest_auth("", *pass, 300, &reload_nonce) ? false : true;
}
