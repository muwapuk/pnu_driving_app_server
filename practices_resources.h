#ifndef PRACTICES_RESOURCES_H
#define PRACTICES_RESOURCES_H

#include <memory>
#include <httpserver.hpp>

using namespace httpserver;

namespace pr {

class practices_resource : public http_resource
{
    std::shared_ptr<http_response>
    buildStudentGetPracticesResponse(int studentId);

    std::shared_ptr<http_response>
    buildTeacherGetPracticesResponse(int teacherId);

    bool bookStudentForPractice();
    bool addSlotForPractice();
public:
    // .../practices/by-student -> JSON
    std::shared_ptr<http_response> render_GET_slotsByStudent(const http_request &req);
    // .../practices/by-teacher -> JSON
    std::shared_ptr<http_response> render_GET_slotsByTeacher(const http_request &req);

    // .../practices <- JSON
    std::shared_ptr<http_response> render_PUT_practiceSlot(const http_request &req);

    // .../practices <- JSON
    std::shared_ptr<http_response> render_POST_practiceBooking(const http_request &req);

    // .../practices/slots/{id}
    std::shared_ptr<http_response> render_DELETE_practiceSlot(const http_request &req);
    // .../practices/bookings/{id[0-9]+}
    std::shared_ptr<http_response> render_DELETE_practiceBooking(const http_request &req);

    std::shared_ptr<http_response> render(const http_request &);
};

}
#endif // PRACTICES_RESOURCES_H
