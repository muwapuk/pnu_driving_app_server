#ifndef USERS_RESOURCES_H
#define USERS_RESOURCES_H

#include <httpserver.hpp>

using namespace httpserver;

class users_resources : public http_resource
{
public:
    std::shared_ptr<http_response> render_GET(const http_request &req);
    std::shared_ptr<http_response> render_PUT(const http_request &req);
    std::shared_ptr<http_response> render(const http_request &req);
};

#endif // USERS_RESOURCES_H
