#ifndef QUESTIONS_RESOURCES_H
#define QUESTIONS_RESOURCES_H

#include <httpserver.hpp>

#include "appdatabase_structs.h"

using namespace httpserver;
namespace qr {

class questions_resource : public http_resource
{
    // GET .../testing/subjects
    std::shared_ptr<http_response> buildSubjectsResponse();
    // GET .../testing/subjects/{subject}/questions-id-list
    std::shared_ptr<http_response> buildQuestionsIdBySubjectResponse(std::string subject);
    // GET .../testing/categories/{category}/tickets
    std::shared_ptr<http_response> buildTicketsIdResponse(std::string categoryString);
    // GET .../testing/categories/{category}/tickets/by-id/{id|[0-9]+}/questions-id-list
    std::shared_ptr<http_response> buildQuestionsIdByTicketResponse(std::string subject, int ticketNumber);
    // GET .../testing/questions/by-id/{id|[0-9]+}
    std::shared_ptr<http_response> buildQuestionResponse(int questionId);
    // POST .../testing/questions/by-id/{id|[0-9]+}/answer
    std::shared_ptr<http_response> buildAnswerResponse(int questionId, int answer);

public:
    // .../questions/subjects
    // .../questions/categories/{category}/tickets/amount
    // .../questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/amount
    std::shared_ptr<http_response> render_GET(const http_request &req);

    // .../questions <- JSON
    std::shared_ptr<http_response> render_PUT(const http_request &req);

    // .../questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+}
    std::shared_ptr<http_response> render_DELETE(const http_request &req);

    // .../questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+} <- JSON
    std::shared_ptr<http_response> render_PATCH(const http_request &req);

    std::shared_ptr<http_response> render(const http_request &req);
};

// class questions_results_resource : public http_resource
// {
// public:
//     std::shared_ptr<http_response> render_GET(const http_request &req);
//     std::shared_ptr<http_response> render_PUT(const http_request &req);
//     std::shared_ptr<http_response> render(const http_request &req);
// };

}

#endif // QUESTIONS_RESOURCES_H
