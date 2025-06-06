#include "practices_resources.h"
#include "appdatabase.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"

#include <iostream>
#include <nlohmann/json.hpp>

using namespace pr;

using json = nlohmann::json;

// .../practices/by-student -> JSON
// .../practices/by-teacher -> JSON
shared_ptr<http_response> practices_resource::render_GET_slotsByStudent(const http_request &req)
{
    json j_requestBody;
    string requestLogin;
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_requestBody)
     || !JsonConverter::jsonValueToString(j_requestBody, "login",  requestLogin))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    if ((requestLogin != loginAndPerms->first
      && loginAndPerms->second != User::STUDENT)
      || loginAndPerms->second != User::SUPERUSER)
        return shared_ptr<http_response>(new string_response("Forbidden", 403));
    return buildStudentGetPracticesResponse(requestLogin);
}
shared_ptr<http_response> practices_resource::render_GET_slotsByTeacher(const http_request &req)
{
    json j_requestBody;
    string requestLogin;
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_requestBody)
     || !JsonConverter::jsonValueToString(j_requestBody, "login",  requestLogin))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    if ((requestLogin != loginAndPerms->first
      && loginAndPerms->second != User::TEACHER)
      || loginAndPerms->second != User::SUPERUSER)
        return shared_ptr<http_response>(new string_response("Forbidden", 403));
    return buildTeacherGetPracticesResponse(requestLogin);
}
// .../practices <- JSON
shared_ptr<http_response> practices_resource::render_PUT_practiceSlot(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));
    if (loginAndPerms->second != User::TEACHER)
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    PracticeSlot slot;
    json j_slot;
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_slot))
        return shared_ptr<http_response>(new string_response("Bad request!", 400));
    if (!JsonConverter::jsonToPracticeSlot(j_slot, slot))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    if (loginAndPerms->first != slot.teacher_login)
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    if(AppDB().insertPracticeSlot(slot))
        return shared_ptr<http_response>(new string_response("Slot already exist!", 409));

    return shared_ptr<http_response>(new string_response("SUCCESS"));
}
// .../practices <- JSON
shared_ptr<http_response> practices_resource::render_POST_practiceBooking(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));
    if (loginAndPerms->second != User::STUDENT)
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    PracticeBooking booking;
    json j_booking;
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_booking))
        return shared_ptr<http_response>(new string_response("Bad request!", 400));
    if (!JsonConverter::jsonToPracticeBooking(j_booking, booking))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    if (loginAndPerms->first != booking.student_login)
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    if(AppDB().insertPracticeBooking(booking))
        return shared_ptr<http_response>(new string_response("Booking error!", 409));

    return shared_ptr<http_response>(new string_response("SUCCESS"));
}

// .../practices/slots/{id}
shared_ptr<http_response> practices_resource::render_DELETE_practiceSlot(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    json j_request;
    string requestLogin;
    int requestId;
    if (!req.get_arg("id").values.empty()) {
        return shared_ptr<http_response>(new string_response("Bad request!", 400));
    }
    requestId = std::stoi(req.get_arg("id"));
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_request))
        return shared_ptr<http_response>(new string_response("Bad request!", 400));
    if (!JsonConverter::jsonValueToString(j_request, "login", requestLogin))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));

    if ((requestLogin == loginAndPerms->first
      && loginAndPerms->second == User::TEACHER)
      || loginAndPerms->second == User::SUPERUSER) {
        if(!AppDB().deletePracticeSlot(requestId, requestLogin))
            return shared_ptr<http_response>(new string_response("Slot not found!", 404));
    } else {
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    }
    return shared_ptr<http_response>(new string_response("SUCCESS"));
}
// .../practices/bookings/{id[0-9]+}
shared_ptr<http_response> practices_resource::render_DELETE_practiceBooking(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    json j_request;
    string requestLogin;
    int requestId;
    if (!req.get_arg("id").values.empty()) {
        return shared_ptr<http_response>(new string_response("Bad request!", 400));
    }
    requestId = std::stoi(req.get_arg("id"));
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_request))
        return shared_ptr<http_response>(new string_response("Bad request!", 400));
    if (!JsonConverter::jsonValueToString(j_request, "login", requestLogin))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));

    if ((requestLogin == loginAndPerms->first
      && loginAndPerms->second == User::STUDENT)
      || loginAndPerms->second == User::SUPERUSER) {
        if(!AppDB().deletePracticeBooking(requestId, requestLogin))
            return shared_ptr<http_response>(new string_response("Booking not found!", 404));
    } else {
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    }

    return shared_ptr<http_response>(new string_response("SUCCESS"));
}
shared_ptr<http_response> practices_resource::render(const http_request &)
{
    return shared_ptr<http_response>(new string_response("Method not allowed", 405));
}

shared_ptr<http_response>
practices_resource::buildStudentGetPracticesResponse(const std::string &studentLogin)
{
    auto freeSlots = AppDB().getFreePracticeSlotsForStudent(studentLogin);
    auto bookedSlots = AppDB().getBookedPracticeSlotsForStudent(studentLogin);
    if (!freeSlots || !bookedSlots)
        return shared_ptr<http_response>(new string_response("Student does not exist!", 404));

    json j_practices;
    json j_freeSlots;
    json j_bookedSlots;
    json j_selfBookedSlots;

    for (auto &slot : *freeSlots) {
        json j_slot;
        j_slot["id"] = slot.id;
        j_slot["time"] = slot.time;
        j_freeSlots.push_back(j_slot);
    }
    j_practices["free-slots"] = j_freeSlots;

    for (auto &slot : *bookedSlots) {
        json j_slot;
        if (slot.studentLogin != studentLogin) {
            j_slot["id"] = slot.id;
            j_slot["time"] = slot.time;
            j_bookedSlots.push_back(j_slot);
        } else {
            j_slot["id"] = slot.id;
            j_slot["time"] = slot.time;
            j_selfBookedSlots.push_back(j_slot);
        }
    }
    j_practices["booked-slots"] = j_bookedSlots;
    j_practices["self-booked-slots"] = j_selfBookedSlots;

    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_practices, responseBody);
    return shared_ptr<string_response>(new string_response(responseBody,
                                                           200,
                                                           "application/json"));
}

shared_ptr<http_response>
practices_resource::buildTeacherGetPracticesResponse(const std::string &teacherLogin)
{
    auto freeSlots = AppDB().getFreePracticeSlotsForStudent(teacherLogin);
    auto bookedSlots = AppDB().getBookedPracticeSlotsForStudent(teacherLogin);
    if (!freeSlots || !bookedSlots)
        return shared_ptr<http_response>(new string_response("Student does not exist!", 404));

    json j_practices;
    json j_freeSlots;
    json j_bookedSlots;

    for (auto &slot : *freeSlots) {
        json j_slot;
        j_slot["id"] = slot.id;
        j_slot["time"] = slot.time;
        j_freeSlots.push_back(j_slot);
    }
    j_practices["free-slots"] = j_freeSlots;

    for (auto &slot : *bookedSlots) {
        json j_slot;
        j_slot["id"] = slot.id;
        j_slot["time"] = slot.time;
        j_slot["student-name"] = slot.studentName;
        j_bookedSlots.push_back(j_slot);
    }
    j_practices["booked-slots"] = j_bookedSlots;

    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_practices, responseBody);
    return shared_ptr<string_response>(
        new string_response(responseBody,200,"application/json"));
}












