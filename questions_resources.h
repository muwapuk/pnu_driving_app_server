#ifndef QUESTIONS_RESOURCES_H
#define QUESTIONS_RESOURCES_H

#include <httpserver.hpp>


using namespace httpserver;
namespace qr {

class questions_resource : public http_resource
{
public:
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

    // .../testing/categories/{category}/tickets/{number[0-9]+} -> json {id}
    std::shared_ptr<http_response> render_PUT_ticket(const http_request &req);
    // .../testing/questions <- json {question} -> json {id}
    std::shared_ptr<http_response> render_PUT_question(const http_request &req);

    // .../testing/tickets/by-id/{id|[0-9]+}
    std::shared_ptr<http_response> render_DELETE_ticket(const http_request &req);
    // .../testing/questions/by-id/{id|[0-9]+}
    std::shared_ptr<http_response> render_DELETE_question(const http_request &req);

    // .../testing/questions/by-id/{id} <- json {question}
    std::shared_ptr<http_response> render_PATCH_question(const http_request &req);


    std::shared_ptr<http_response> render(const http_request &req);
};

}

#endif // QUESTIONS_RESOURCES_H
