#ifndef LECTURES_RESOURCES_H
#define LECTURES_RESOURCES_H

#include <httpserver.hpp>

using namespace httpserver;

namespace lc {

class lectures_resource : public http_resource
{
protected:
    // .../lectures/by-teacher/{id} -> JSON {lecture}
    std::shared_ptr<http_response> render_GET_lecturesByTeacher(const http_request &req);
    // .../lectures/by-group/{id} -> JSON {lecture}
    std::shared_ptr<http_response> render_GET_lecturesByGroup(const http_request &req);

    // .../lectures <- JSON {lecture}
    std::shared_ptr<http_response> render_PUT_lecture(const http_request &req);
    // .../lectures/{id}
    std::shared_ptr<http_response> render_DELETE_lecture(const http_request &req);
};
// GET /lectures/by-teacher/{id}
class lectures_by_teacher_resource : public lectures_resource {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_lecturesByTeacher(req);
    }
};

// GET /lectures/by-group/{id}
class lectures_by_group_resource : public lectures_resource {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_lecturesByGroup(req);
    }
};

// PUT /lectures
class create_lecture_resource : public lectures_resource {
public:
    std::shared_ptr<http_response> render_PUT(const http_request& req) override {
        return render_PUT_lecture(req);
    }
};

// DELETE /lectures/{id}
class delete_lecture_resource : public lectures_resource {
public:
    std::shared_ptr<http_response> render_DELETE(const http_request& req) override {
        return render_DELETE_lecture(req);
    }
};

}
#endif // LECTURES_RESOURCES_H
