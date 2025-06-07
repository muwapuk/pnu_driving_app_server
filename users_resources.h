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
public:
   //  std::shared_ptr<http_response> render_GET(const http_request &req);
   // // std::shared_ptr<http_response> render_DELETE(const http_request &req);
   //  std::shared_ptr<http_response> render_PATCH(const http_request &req);
   //  std::shared_ptr<http_response> render(const http_request &);
};

}
#endif // USERS_RESOURCES_H
