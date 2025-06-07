#include "practices_resources.h"
#include "appdatabase.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"


using namespace pr;

using json = nlohmann::json;

// .../practices/by-student/{id|[0-9]+} -> JSON
shared_ptr<http_response> practices_resource::render_GET_slotsByStudent(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    return buildStudentGetPracticesResponse(std::stoi(req.get_arg("id")));
}
// .../practices/by-teacher/{id|[0-9]+} -> JSON
shared_ptr<http_response> practices_resource::render_GET_slotsByTeacher(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    return buildTeacherGetPracticesResponse(std::stoi(req.get_arg("id")));
}
// .../practices <- JSON
shared_ptr<http_response> practices_resource::render_PUT_practiceSlot(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));
    if (uidAndPerms->second != User::TEACHER)
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    PracticeSlot slot;
    json j_slot;
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_slot))
        return shared_ptr<http_response>(new string_response("Bad request!", 400));
    if (!JsonConverter::jsonToPracticeSlot(j_slot, slot))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    if (uidAndPerms->first != slot.teacherId)
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    if(AppDB().insertPracticeSlot(slot))
        return shared_ptr<http_response>(new string_response("Slot already exist!", 409));

    return shared_ptr<http_response>(new string_response("SUCCESS"));
}
// .../practices <- JSON
shared_ptr<http_response> practices_resource::render_POST_practiceBooking(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE
     || uidAndPerms->second != User::STUDENT)
        return shared_ptr<http_response>(new string_response("Forbidden", 403));
    PracticeBooking booking;
    json j_booking;
    if (!JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_booking))
        return shared_ptr<http_response>(new string_response("Bad request!", 400));
    if (!JsonConverter::jsonToPracticeBooking(j_booking, booking))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    if (uidAndPerms->first != booking.studentId)
        return shared_ptr<http_response>(new string_response("Forbidden!", 403));
    if(AppDB().insertPracticeBooking(booking))
        return shared_ptr<http_response>(new string_response("Booking error!", 409));

    return shared_ptr<http_response>(new string_response("SUCCESS"));
}

// .../practices/slots/{id|[0-9]+}
shared_ptr<http_response> practices_resource::render_DELETE_practiceSlot(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    int requestId = std::stoi(req.get_arg("id"));
    if (uidAndPerms->second == User::SUPERUSER
     || AppDB().getPracticeSlot(requestId)->teacherId == uidAndPerms->first) {
        if(!AppDB().deletePracticeSlot(requestId))
            return shared_ptr<http_response>(new string_response("Slot not found!", 404));
    } else {
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));
    }
    return shared_ptr<http_response>(new string_response("SUCCESS"));
}
// .../practices/bookings/{id[0-9]+}
shared_ptr<http_response> practices_resource::render_DELETE_practiceBooking(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    int requestId = std::stoi(req.get_arg("id"));
    if (uidAndPerms->second == User::SUPERUSER
     || AppDB().getPracticeBooking(requestId)->studentId == uidAndPerms->first) {
        if(!AppDB().deletePracticeBooking(requestId))
            return shared_ptr<http_response>(new string_response("Booking not found!", 404));
    } else {
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));
    }
    return shared_ptr<http_response>(new string_response("SUCCESS"));
}
shared_ptr<http_response> practices_resource::render(const http_request &)
{
    return shared_ptr<http_response>(new string_response("Method not allowed", 405));
}

shared_ptr<http_response>
practices_resource::buildStudentGetPracticesResponse(int studentId)
{
    auto freeSlots = AppDB().getFreePracticeSlotsForStudent(studentId);
    auto bookedSlots = AppDB().getBookedPracticeSlotsForStudent(studentId);
    if (!freeSlots || !bookedSlots)
        return shared_ptr<http_response>(new string_response("Student does not exist!", 404));

    json j_practices;
    json j_freeSlots;
    json j_bookedSlots;
    json j_selfBookedSlots;

    for (auto &slot : *freeSlots) {
        json j_slot;
        j_slot["id"] = slot.slotId;
        j_slot["time"] = slot.time;
        j_freeSlots.push_back(j_slot);
    }
    j_practices["free-slots"] = j_freeSlots;

    for (auto &slot : *bookedSlots) {
        json j_slot;
        if (slot.studentId != studentId) {
            j_slot["id"] = slot.slotId;
            j_slot["time"] = slot.time;
            j_bookedSlots.push_back(j_slot);
        } else {
            j_slot["id"] = slot.slotId;
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
practices_resource::buildTeacherGetPracticesResponse(int teacherId)
{
    auto freeSlots = AppDB().getFreePracticeSlotsForStudent(teacherId);
    auto bookedSlots = AppDB().getBookedPracticeSlotsForStudent(teacherId);
    if (!freeSlots || !bookedSlots)
        return shared_ptr<http_response>(new string_response("Student does not exist!", 404));

    json j_practices;
    json j_freeSlots;
    json j_bookedSlots;

    for (auto &slot : *freeSlots) {
        json j_slot;
        j_slot["id"] = slot.slotId;
        j_slot["time"] = slot.time;
        j_freeSlots.push_back(j_slot);
    }
    j_practices["free-slots"] = j_freeSlots;

    for (auto &slot : *bookedSlots) {
        json j_slot;
        j_slot["id"] = slot.slotId;
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












