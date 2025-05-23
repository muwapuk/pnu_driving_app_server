#ifndef PRACTICES_RESOURCES_H
#define PRACTICES_RESOURCES_H

#include "appdatabase_structs.h"
#include <memory>
#include <httpserver.hpp>

using namespace httpserver;

namespace pr {

class practices_resource : public http_resource
{
    std::shared_ptr<http_response> practicesToResponse(std::vector<Practice>* practices);
public:
    // .../practices -> JSON
    // .../practices/by-student/{student} -> JSON
    // .../practices/by-teacher/{teacher} -> JSON
    // .../practices/by-time/from/{fromTime|[0-9]+}/to/{toTime|[0-9]+}" -> JSON
    std::shared_ptr<http_response> render_GET(const http_request &req);

    // .../practices <- JSON
    std::shared_ptr<http_response> render_PUT(const http_request &req);

    // .../practices/?teacher=<...>&time=<...>
    std::shared_ptr<http_response> render_DELETE(const http_request &req);

    // .../practices/?teacher=<...>&time=<...> <- JSON
    std::shared_ptr<http_response> render_PATCH(const http_request &req);

    std::shared_ptr<http_response> render(const http_request &);
};

}
#endif // PRACTICES_RESOURCES_H
