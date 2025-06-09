#ifndef TESTING_RESOURCES_H
#define TESTING_RESOURCES_H

#include <httpserver.hpp>


using namespace httpserver;
namespace qr {

class testing_resources : public http_resource
{
protected:
    // .../testing/subjects -> json
    std::shared_ptr<http_response> render_GET_subjectsList(const http_request &req);
    // .../testing/subjects/{subject}/questions-list -> json
    std::shared_ptr<http_response> render_GET_questionsListBySubject(const http_request &req);
    // .../testing/categories/{category}/tickets -> json
    std::shared_ptr<http_response> render_GET_ticketsList(const http_request &req);
    // .../testing/tickets/by-id/{id|[0-9]+}/questions-list -> json
    std::shared_ptr<http_response> render_GET_questionsListByTicket(const http_request &req);
    // .../testing/questions/by-id/{id|[0-9]+} -> json
    std::shared_ptr<http_response> render_GET_question(const http_request &req);
    // // .../testing/questions/by-id/{id|[0-9]+}/image -> json
    // std::shared_ptr<http_response> render_GET_questionImage(const http_request &req);

    // .../testing/categories/{category}/tickets/{number[0-9]+} -> json {id}
    std::shared_ptr<http_response> render_PUT_ticket(const http_request &req);
    // .../testing/questions <- json {question} -> json {id}
    std::shared_ptr<http_response> render_PUT_question(const http_request &req);


    // .../testing/questions/by-id/{id|[0-9]+}
    std::shared_ptr<http_response> render_PATCH_question(const http_request &req);

    // .../testing/tickets/by-id/{id|[0-9]+}
    std::shared_ptr<http_response> render_DELETE_ticket(const http_request &req);
    // .../testing/questions/by-id/{id|[0-9]+}
    std::shared_ptr<http_response> render_DELETE_question(const http_request &req);
};
// GET /testing/subjects -> json
class subjects_resource : public testing_resources {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_subjectsList(req);
    }
};
// GET /testing/subjects/{subject}/questions-list -> json
class questions_by_subject_resource : public testing_resources {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_questionsListBySubject(req);
    }
};
// GET /testing/categories/{category}/tickets -> json
class tickets_by_category_resource : public testing_resources {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_ticketsList(req);
    }
};
// GET /testing/tickets/by-id/{id|[0-9]+}/questions-list -> json
class questions_by_ticket_resource : public testing_resources {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_questionsListByTicket(req);
    }
};

// PUT /testing/categories/{category}/tickets/{number[0-9]+} -> json {id}
class create_ticket_resource : public testing_resources {
public:
    std::shared_ptr<http_response> render_PUT(const http_request& req) override {
        return render_PUT_ticket(req);
    }
};
// PUT /testing/questions
class create_question_resource : public testing_resources {
public:
    std::shared_ptr<http_response> render_PUT(const http_request& req) override {
        return render_PUT_question(req);
    }
};
// DELETE /testing/tickets/by-id/{id|[0-9]+}
class delete_ticket_resource : public testing_resources {
public:
    std::shared_ptr<http_response> render_DELETE(const http_request& req) override {
        return render_DELETE_ticket(req);
    }
};
// GET/PATCH/DELETE /testing/questions/by-id/{id|[0-9]+} -> json
class question_by_id_resource : public testing_resources {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_question(req);
    }
    std::shared_ptr<http_response> render_PATCH(const http_request& req) override {
        return render_PATCH_question(req);
    }
    std::shared_ptr<http_response> render_DELETE(const http_request& req) override {
        return render_DELETE_question(req);
    }
};
// GET /testing/questions/by-id/{id|[0-9]+}/image -> json
class question_image_by_id_resource : public testing_resources {
public:
    std::shared_ptr<http_response> render_GET(const http_request& req) override {
        return render_GET_questionsListByTicket(req);
    }
};

}

#endif // TESTING_RESOURCES_H
