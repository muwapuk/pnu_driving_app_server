#include "lectures_resources.h"

#include "appdatabase.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"

using json = nlohmann::json;

using namespace lc;

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
shared_ptr<http_response> lectures_resource::render_POST_lecture(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second != User::TEACHER)
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    Lecture lecture;
    json j_lecture;
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_lecture))
        return shared_ptr<http_response>(new string_response("Bad request!", 400));
    if (!JsonConverter::jsonToLecture(j_lecture, lecture))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    if (uidAndPerms->first != lecture.teacherId)
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    lecture.teacherId = uidAndPerms->first;
    if(AppDB().insertLecture(lecture))
        return shared_ptr<http_response>(new string_response("Lecture already exist!", 409));

    return shared_ptr<http_response>(new string_response("SUCCESS"));
}
// .../lectures/{id}
shared_ptr<http_response> lectures_resource::render_DELETE_lecture(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    int requestId = std::stoi(req.get_arg("id"));

    auto lecture = AppDB().getLecture(requestId);
    if (!lecture)
        return shared_ptr<http_response>(new string_response("Lecture not found!", 404));
    if (uidAndPerms->second == User::SUPERUSER
    || lecture->teacherId == uidAndPerms->first) {
        AppDB().deletePracticeSlot(requestId);
    } else {
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));
    }
    return shared_ptr<http_response>(new string_response("SUCCESS"));
}

