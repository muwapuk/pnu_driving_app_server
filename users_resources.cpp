#include "users_resources.h"
#include "appdatabase.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"

#define HEADER_RESPONSE

using json = nlohmann::json;
using namespace ur;

// .../users/self-id -> JSON {id}
std::shared_ptr<http_response>
users_resources::render_GET_selfId(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));

    json j_response;
    j_response["id"] = uidAndPerms->first;

    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_response, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}
// .../users/{user-id|[0-9]+}/name -> JSON {name}
std::shared_ptr<http_response>
users_resources::render_GET_userName(const http_request &req) {
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));

    int id = std::stoi(req.get_arg("user-id"));

    json j_response;
    j_response["id"] = id;
    auto name = AppDB().getUserName(id);
    if (!name)
        return shared_ptr<http_response>(new string_response("User not found", 404));
    j_response["name"] = *name;

    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_response, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}
// .../users/students/{student-id|[0-9]+}/group -> JSON {group-id, group-name}
std::shared_ptr<http_response>
users_resources::render_GET_studentGroup(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));

    int studentId = std::stoi(req.get_arg("student-id"));

    json j_response;
    int groupId;
    if(!(groupId = AppDB().getGroupIdByStudent(studentId)))
        return shared_ptr<http_response>(new string_response("Student not found", 404));
    j_response["group-id"] = groupId;
    j_response["group-name"] = *AppDB().getGroupName(groupId);

    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_response, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}
