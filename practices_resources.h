#ifndef PRACTICES_RESOURCES_H
#define PRACTICES_RESOURCES_H

#include <memory>
#include <httpserver.hpp>

using namespace httpserver;

namespace pr {

class practices_resource : public http_resource
{
protected:
    std::shared_ptr<http_response>
    buildStudentGetPracticesResponse(int studentId);

    std::shared_ptr<http_response>
    buildTeacherGetPracticesResponse(int teacherId);

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

public:
    std::shared_ptr<http_response> render(const http_request &);
};
// GET /practices/by-student -> JSON
class student_slots_resource : public practices_resource {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_slotsByStudent(req);
    }
};
// GET /practices/by-teacher -> JSON
class teacher_slots_resource : public practices_resource {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_slotsByTeacher(req);
    }
};
// PUT /practices <- JSON
class create_slot_resource : public practices_resource {
public:
    std::shared_ptr<http_response> render_PUT(const http_request& req) override {
        return render_PUT_practiceSlot(req);
    }
};
// POST /practices <- JSON
class create_booking_resource : public practices_resource {
public:
    std::shared_ptr<http_response> render_POST(const http_request& req) override {
        return render_POST_practiceBooking(req);
    }
};
// DELETE /practices/slots/{id}
class delete_slot_resource : public practices_resource {
public:
    std::shared_ptr<http_response> render_DELETE(const http_request& req) override {
        return render_DELETE_practiceSlot(req);
    }
};
// DELETE /practices/bookings/{id[0-9]+}
class delete_booking_resource : public practices_resource {
public:
    std::shared_ptr<http_response> render_DELETE(const http_request& req) override {
        return render_DELETE_practiceBooking(req);
    }
};

}
#endif // PRACTICES_RESOURCES_H
