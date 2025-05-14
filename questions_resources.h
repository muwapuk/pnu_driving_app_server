#ifndef QUESTIONS_RESOURCES_H
#define QUESTIONS_RESOURCES_H

#include <httpserver.hpp>


using namespace httpserver;
namespace qr {

enum Categories{
    AB,
    CD
};

class questions_resource : public http_resource
{
public:
    std::shared_ptr<http_response> render_GET(const http_request &req);
    std::shared_ptr<http_response> render_PUT(const http_request &req);
    std::shared_ptr<http_response> render_PATCH(const http_request &req);
    std::shared_ptr<http_response> render_POST(const http_request &req);
    std::shared_ptr<http_response> render(const http_request &req);


};

class questions_results_resource : public http_resource
{
public:
    std::shared_ptr<http_response> render_GET(const http_request &req);
    std::shared_ptr<http_response> render_PUT(const http_request &req);
    std::shared_ptr<http_response> render(const http_request &req);
};


}

#endif // QUESTIONS_RESOURCES_H
