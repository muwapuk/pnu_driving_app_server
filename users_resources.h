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
protected:
    // .../users/self-id -> JSON {id}
    std::shared_ptr<http_response> render_GET_selfId(const http_request &req);
    // .../users/{user-id|[0-9]+}/name -> JSON {name}
    std::shared_ptr<http_response> render_GET_userName(const http_request &req);
    // .../users/students/{student-id|[0-9]+}/group -> JSON {group-id, group-name}
    std::shared_ptr<http_response> render_GET_studentGroup(const http_request &req);

};

// GET .../users/self-id -> JSON {id}
class users_selfId_resource : public users_resources {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_selfId(req);
    }
};
// GET .../users/{user-id|[0-9]+}/name -> JSON {name}
class users_name_resource : public users_resources {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_userName(req);
    }
};
// GET .../users/students/{student-id|[0-9]+}/group -> JSON {group-id, group-name}
class users_studentGroup_resource : public users_resources {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_studentGroup(req);
    }
};

}
#endif // USERS_RESOURCES_H
