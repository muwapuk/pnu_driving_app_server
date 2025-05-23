#ifndef QUESTIONS_RESOURCES_H
#define QUESTIONS_RESOURCES_H

#include <httpserver.hpp>


using namespace httpserver;
namespace qr {


class questions_resource : public http_resource
{
public:

    // .../questions/by-theme
    // .../questions/categories/{category}/tickets/amount
    // .../questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/amount
    // .../questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/{question|[0-9]+}?attribute="..." -> json
    std::shared_ptr<http_response> render_GET(const http_request &req);

    // .../questions <- JSON
    std::shared_ptr<http_response> render_PUT(const http_request &req);

    // .../questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+}
    std::shared_ptr<http_response> render_DELETE(const http_request &req);

    // .../questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+} <- JSON
    std::shared_ptr<http_response> render_PATCH(const http_request &req);

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
