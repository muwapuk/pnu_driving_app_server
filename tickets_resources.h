#ifndef TICKETS_RESOURCES_H
#define TICKETS_RESOURCES_H

#include <httpserver.hpp>

using namespace httpserver;

class hello_world_resource : public http_resource
{
public:
    std::shared_ptr<http_response> render(const http_request& req) {
        return std::shared_ptr<http_response>(new string_response("Hello: " + std::string(req.get_arg("name"))));
    }
};

class tickets_resources
{
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) {
        return std::shared_ptr<http_response>(new string_response("Hello: " + std::string(req.get_arg("name"))));
    }
    std::shared_ptr<http_response> render_PUT(const http_request& req) {
        return std::shared_ptr<http_response>(new string_response("Hello: " + std::string(req.get_arg("name"))));
    }
    std::shared_ptr<http_response> render(const http_request& req) {
        return std::shared_ptr<http_response>(new string_response("Hello: " + std::string(req.get_arg("name"))));
    }
};

#endif // TICKETS_RESOURCES_H
