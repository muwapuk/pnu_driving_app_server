#include "lectures_resources.h"

#include "appdatabase.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"

using json = nlohmann::json;

// .../lectures/by-teacher/{id}
shared_ptr<http_response> lectures_resource::render_GET_lecturesByTeacher(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return shared_ptr<http_response>(new string_response("Forbidden", 403));

    auto lectures = AppDB().getLecturesByTeacher(std::stoi(req.get_arg("id")));

    json j_lectures;
    for (auto &lecture : *lectures) {
        json j_lecture;
        JsonConverter::lectureToJson(lecture, j_lecture);
        j_lectures.push_back(j_lecture);
    }
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_lectures, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody));
}
// .../lectures/by-group/{id} -> JSON {lecture}
shared_ptr<http_response> lectures_resource::render_GET_lecturesByGroup(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return shared_ptr<http_response>(new string_response("Forbidden", 403));

    auto lectures = AppDB().getLecturesByGroup(std::stoi(req.get_arg("id")));

    json j_lectures;
    for (auto &lecture : *lectures) {
        json j_lecture;
        JsonConverter::lectureToJson(lecture, j_lecture);
        j_lectures.push_back(j_lecture);
    }
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_lectures, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody));
}
// .../lectures <- JSON {lecture}
shared_ptr<http_response> lectures_resource::render_PUT_lecture(const http_request &req)
{

}
// .../lectures/{id}
shared_ptr<http_response> lectures_resource::render_DELETE_lecture(const http_request &req)
{

}
