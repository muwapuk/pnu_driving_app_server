#ifndef LECTURES_RESOURCES_H
#define LECTURES_RESOURCES_H

#include <httpserver.hpp>

using namespace httpserver;

class lectures_resource : public http_resource
{
public:
    // .../lectures/by-teacher/{id} -> JSON {lecture}
    std::shared_ptr<http_response> render_GET_lecturesByTeacher(const http_request &req);
    // .../lectures/by-group/{id} -> JSON {lecture}
    std::shared_ptr<http_response> render_GET_lecturesByGroup(const http_request &req);

    // .../lectures <- JSON {lecture}
    std::shared_ptr<http_response> render_PUT_lecture(const http_request &req);
    // .../lectures/{id}
    std::shared_ptr<http_response> render_DELETE_lecture(const http_request &req);


    std::shared_ptr<http_response> render(const http_request &);
};

#endif // LECTURES_RESOURCES_H
