#include "lectures_resources.h"

#include "appdatabase.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"

using json = nlohmann::json;

std::shared_ptr<http_response> lectures_resource::render_GET_lecturesByTeacher(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    json j_requestBody;
    string requestLogin;
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_requestBody)
        || !JsonConverter::jsonValueToString(j_requestBody, "login",  requestLogin))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    if ((requestLogin != loginAndPerms->first
         && loginAndPerms->second != User::STUDENT)
        || loginAndPerms->second != User::SUPERUSER)
        return shared_ptr<http_response>(new string_response("Forbidden", 403));
    return buildStudentGetPracticesResponse(requestLogin);
}
std::shared_ptr<http_response> lectures_resource::render_GET_lecturesByGroup(const http_request &req)
{

}
std::shared_ptr<http_response> lectures_resource::render_PUT_lecture(const http_request &req)
{

}
std::shared_ptr<http_response> lectures_resource::render_DELETE_lecture(const http_request &req)
{

}
