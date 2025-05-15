#ifndef USERS_RESOURCES_H
#define USERS_RESOURCES_H

#include "appdatabase_structs.h"

#include <httpserver.hpp>

using namespace httpserver;
namespace ur {

int MAX_JSON_ARRAY_SIZE = 10;

enum UserAttributes {
    UNDEFINED,
    NAME,
    PASSWORD,
    PERMISSIONS,
    BLANK
};


class users_resources : public http_resource
{
    User *getUser(std::string user);
    std::vector<User> *getUsersPage(int page);
    User *getUserAttribute(std::string user, UserAttributes);
    std::vector<User> *getUsersAttributesPage(UserAttributes, int page);
public:
    std::shared_ptr<http_response> render_GET(const http_request &req);
    std::shared_ptr<http_response> render_PUT(const http_request &req);
    std::shared_ptr<http_response> render(const http_request &req);
};

}
#endif // USERS_RESOURCES_H
