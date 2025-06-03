#include "practices_resources.h"
#include "appdatabase.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace pr;

using json = nlohmann::json;

// .../practices -> JSON
// .../practices/by-student/{student} -> JSON
// .../practices/by-teacher/{teacher} -> JSON
// .../practices/available-for-student/{studentLookingForPractice} -> JSON
// .../practices/by-time/from/{fromTime|[0-9]+}/to/{toTime|[0-9]+}" -> JSON
std::shared_ptr<http_response> practices_resource::render_GET(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return std::shared_ptr<http_response>(new string_response("Bad Token", 401));

    std::string method = req.get_path_piece(1);
    if (method == "") {
        auto practices = AppDB().getPractices();
        auto response = practicesToResponse(practices);
        delete practices;
        return response;
    } else if (!req.get_arg("student").values.empty()) {
        if (!(loginAndPerms->first == string(req.get_arg("student"))))
            return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

        auto practices = AppDB().getPracticesByStudent(req.get_arg("student"));
        auto response = practicesToResponse(practices);
        delete practices;
        return response;
    } else if(!req.get_arg("teacher").values.empty()) {
        if (!(loginAndPerms->first == string(req.get_arg("teacher"))))
            return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

        auto practices = AppDB().getPracticesByTeacher(req.get_arg("teacher"));
        auto response = practicesToResponse(practices);
        delete practices;
        return response;
    } else if(!req.get_arg("studentLookingForPractice").values.empty()) {
        if (!(loginAndPerms->first == string(req.get_arg("studentLookingForPractice"))))
            return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

        auto practices =
            AppDB().getPracticesByUserInstructor(req.get_arg("studentLookingForPractice"));
        auto response = practicesToResponse(practices);
        delete practices;
        return response;
    } else if (!req.get_arg("fromTime").values.empty()
              && req.get_arg("toTime").values.empty()) {
        int fromTime, toTime;
        try {
            fromTime = std::stoi(req.get_arg("fromTime"));
            toTime = std::stoi(req.get_arg("toTime"));
        } catch (std::exception &e) {
            return std::shared_ptr<http_response>(new string_response("Bad request!", 400));
        }

        auto practices = AppDB().getPracticesByTime(fromTime, toTime);
        if(!practices) {
            return std::shared_ptr<http_response>(new string_response("Database select error!", 500));
        }
        auto response = practicesToResponse(practices);
        delete practices;
        return response;
    } else {
        return std::shared_ptr<http_response>(new string_response("Server error!", 500));
    }
}

// .../practices <- JSON
std::shared_ptr<http_response> practices_resource::render_PUT(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return std::shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (!(loginAndPerms->second == User::TEACHER
       || loginAndPerms->second == User::SUPERUSER)
    ) return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    Practice newPractice;
    std::string jsonStr = std::string(req.get_content());
    json json;


    if(JsonConverter::SUCCESS != JsonConverter::jsonStringToJson(jsonStr, json))
        return std::shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    try {
        newPractice.teacher_login = json["teacher"];
        newPractice.thematic = json["thematic"];
        newPractice.date = json["date"];
    } catch (nlohmann::json::exception) {
        return std::shared_ptr<http_response>(new string_response("JSON struct does not match practice struct!", 400));
    }

    if (loginAndPerms->first != newPractice.teacher_login)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    newPractice.student_login = "";
    newPractice.car = "";

    if(AppDB().isUserExist(newPractice.teacher_login))
        return std::shared_ptr<http_response>(new string_response("Teacher does not exist!", 404));

    if(!AppDB().addPractice(newPractice))
        return std::shared_ptr<http_response>(new string_response("Practice already exist!", 409));


    return std::shared_ptr<http_response>(new string_response("SUCCESS"));
}

// .../practices/?teacher=<...>&time=<...>
std::shared_ptr<http_response> practices_resource::render_DELETE(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return std::shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (!(loginAndPerms->second == User::TEACHER
          || loginAndPerms->second == User::SUPERUSER)
    ) return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    std::string teacher;
    int time;
    if(!req.get_arg("teacher").values.empty())
        return std::shared_ptr<http_response>(new string_response("Bad request!", 400));
    teacher = req.get_arg("teacher");

    if (loginAndPerms->first != teacher)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    try {
        time = std::stoi(req.get_arg("time"));
    } catch (std::exception &e) {
        return std::shared_ptr<http_response>(new string_response("Bad request!", 400));
    }
    if(!AppDB().deletePractice(teacher, time))
        return std::shared_ptr<http_response>(new string_response("Practice does not exist!", 404));
    return std::shared_ptr<http_response>(new string_response("SUCCESS"));
}

// .../practices/?teacher=<...>&time=<...> <- JSON
std::shared_ptr<http_response> practices_resource::render_PATCH(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return std::shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (!(loginAndPerms->second == User::TEACHER
          || loginAndPerms->second == User::SUPERUSER)
    ) return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    Practice *practice;
    std::string teacher, student, car;
    int time;
    json j_practice;

    try {
        student = j_practice["student"];
        car = j_practice["car"];
    } catch (nlohmann::json::exception) {
        return std::shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    }

    if(student != loginAndPerms->first) {
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));
    }

    std::string jsonStr = std::string(req.get_content());

    if(!req.get_arg("teacher").values.empty())
        return std::shared_ptr<http_response>(new string_response("Bad request!", 400));
    teacher = req.get_arg("teacher");
    try {
        time = std::stoi(req.get_arg("time"));
    } catch (std::exception &e) {
        return std::shared_ptr<http_response>(new string_response("Bad request!", 400));
    }

    if(!(practice = AppDB().getPractice(teacher, time)))
        return std::shared_ptr<http_response>(new string_response("Practice does not exist!", 404));

    if(practice->student_login != "") {
        return std::shared_ptr<http_response>(new string_response("Practice is already taken!", 409));
    }

    if(JsonConverter::SUCCESS != JsonConverter::jsonStringToJson(jsonStr, j_practice)) {
        return std::shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    }

    practice->student_login = student;
    practice->car = car;

    if(!AppDB().changePractice(*practice)) {
        delete practice;
        return std::shared_ptr<http_response>(new string_response("Practice does not exist!", 404));
    }
    delete practice;
    return std::shared_ptr<http_response>(new string_response("SUCCESS"));
}

std::shared_ptr<http_response> practices_resource::render(const http_request &)
{
    return std::shared_ptr<http_response>(new string_response("Bad request!", 400));
}

std::shared_ptr<http_response> practices_resource::practicesToResponse(std::vector<Practice> *practices)
{
    json j_practices = {};
    std::string responseString;
    for (auto &practice : *practices) {
        json j_practice;
        JsonConverter::practiceToJson(practice, j_practice);

        // Changing logins to users names to hide logins from other users
        j_practice.erase("teacher_login");
        j_practice.erase("student_login");

        auto studentName = AppDB().getUserName(practice.student_login);
        if (studentName) {
            j_practice["student_name"] = *studentName;
            delete studentName;
        } else j_practice["student_name"] = "";
        auto teacherName = AppDB().getUserName(practice.teacher_login);
        j_practice["teacher_name"] = *teacherName;
        delete teacherName;

        j_practices.push_back(j_practice);
    }
    JsonConverter::jsonToJsonString(j_practices, responseString);
    auto response = std::shared_ptr<string_response>(new string_response("SUCCESS"));
    response->with_footer("practices", responseString);
    return response;
}

