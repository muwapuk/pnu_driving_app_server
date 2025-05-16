#ifndef USERS_RESOURCES_H
#define USERS_RESOURCES_H

#include <httpserver.hpp>

using namespace httpserver;
namespace ur {

enum UserAttributes {
    UNDEFINED,
    NAME,
    PERMISSIONS,
    BLANK
};


class users_resources : public http_resource
{
    std::shared_ptr<http_response> getUser(std::string login);
    std::shared_ptr<http_response> getUsersPage(int page);
    std::shared_ptr<http_response> getUserAttribute(std::string login, UserAttributes);
    std::shared_ptr<http_response> getUsersAttributesPage(UserAttributes, int page);
public:
    std::shared_ptr<http_response> render_GET(const http_request &req);
    std::shared_ptr<http_response> render_PUT(const http_request &req);
    std::shared_ptr<http_response> render(const http_request &req);
};

}
#endif // USERS_RESOURCES_H
