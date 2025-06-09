#include "appdatabase.h"
#include "testing_resources.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"
#include "appwebserver.h"

namespace qr {
// Used pass string arguments in switch expression

std::map <std::string, tickets::Categories> categories_strings = {
    {"AB", tickets::AB},
    {"CD", tickets::CD},
    {"", tickets::UNDEFINED}
};
}

using namespace qr;
using json = nlohmann::json;

/////////////////////////
// QUESTIONS RESOURCES //
/////////////////////////

// .../testing/subjects
// .../testing/subjects/{subject}/questions-id-list
// .../testing/categories/{category}/tickets
// .../testing/tickets/by-id/{id|[0-9]+}/questions-id-list
// .../testing/questions/by-id/{id|[0-9]+}



// GET .../testing/subjects -> json {json subjects[subject, ...]}
std::shared_ptr<http_response>
testing_resources::render_GET_subjectsList(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    auto subjects = AppDB().getQuestionsSubjects();
    json j_subjects;
    for (auto &subject : *subjects) {
        subjects->push_back(subject);
    }
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_subjects, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}
// GET .../testing/subjects/{subject}/questions-list -> {questions[{num,id},...]}
std::shared_ptr<http_response>
testing_resources::render_GET_questionsListBySubject(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    auto questions = AppDB().getQuestionIdVecBySubject(req.get_arg("subject"));
    if (questions->empty())
        return shared_ptr<http_response>(new string_response("No such subject!", 404));
    json j_questions;
    for (auto &id : *questions) {
        j_questions.push_back(id);
    }
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_questions, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}
// GET .../testing/categories/{category}/tickets -> json {tickets[{num,id},...]}
std::shared_ptr<http_response>
testing_resources::render_GET_ticketsList(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    tickets::Categories category = categories_strings[req.get_arg("category")];
    if (category == tickets::UNDEFINED)
        return shared_ptr<http_response>(new string_response("No such category!", 404));
    auto tickets = AppDB().getTicketNumIdPairsByCategory(category);
    json j_tickets;

    for (auto &numAndId : *tickets) {
        json j_ticket;
        j_ticket["num"] = numAndId.first;
        j_ticket["id"] = numAndId.second;
        j_tickets.push_back(j_ticket);
    }
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_tickets, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}
// .../testing/tickets/by-id/{ticket-id|[0-9]+}/questions-list -> json {questions[{num,id},...]}
std::shared_ptr<http_response>
testing_resources::render_GET_questionsListByTicket(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    auto questions = AppDB().getQuestionNumIdPairsByTicket(std::stoi(req.get_arg("ticket-id")));
    json j_questions;

    for (auto &numAndId : *questions) {
        json j_question;
        j_question["num"] = numAndId.first;
        j_question["id"] = numAndId.second;
        j_questions.push_back(j_question);
    }
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_questions, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}
// GET .../testing/questions/by-id/{id|[0-9]+} -> json {question}

// std::shared_ptr<http_response>
// testing_resources::render_GET_question(const http_request &req)
// {
//     auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
//     if (!uidAndPerms)
//         return shared_ptr<http_response>(new string_response("Bad Token", 401));
//     if (uidAndPerms->second == User::NONE)
//         return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

//     shared_ptr<Question> question;
//     if (!(question = AppDB().getQuestion(std::stoi(req.get_arg("question-id")))))
//         return shared_ptr<http_response>(new string_response("No such question", 404, "application/json"));
//     string responseBody = question->image_base64;
//     return shared_ptr<http_response>(new string_response(responseBody, 200, "text/plain"));
// }
std::shared_ptr<http_response>
testing_resources::render_GET_question(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    shared_ptr<Question> question;
    if (!(question = AppDB().getQuestion(std::stoi(req.get_arg("question-id")))))
        return shared_ptr<http_response>(new string_response("No such question", 404, "application/json"));
    json j_question;
    JsonConverter::questionToJson(*question, j_question);
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_question, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}
// std::shared_ptr<http_response>
// testing_resources::render_GET_questionImage(const http_request &req) {
//     auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
//     if (!uidAndPerms)
//         return shared_ptr<http_response>(new string_response("Bad Token", 401));
//     if (uidAndPerms->second == User::NONE)
//         return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

//     shared_ptr<Question> question;
//     if (!(question = AppDB().getQuestion(std::stoi(req.get_arg("question-id")))))
//         return shared_ptr<http_response>(new string_response("No such question", 404, "application/json"));
//     json j_question;
//     JsonConverter::questionToJson(*question, j_question);
//     string responseBody;
//     JsonConverter::jsonObjectToJsonString(j_question, responseBody);
//     return shared_ptr<http_response>(new file_response(responseBody, 200, "application/json"));
// }
// .../testing/categories/{category}/tickets/{number[0-9]+} -> json {id}
std::shared_ptr<http_response> testing_resources::render_PUT_ticket(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second != User::TEACHER
     && uidAndPerms->second != User::SUPERUSER)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    tickets::Categories category = categories_strings[req.get_arg("category")];
    if (category == tickets::UNDEFINED)
        return shared_ptr<http_response>(new string_response("No such category!", 404));

    int num = std::stoi(req.get_arg("number"));
    int newId;
    if (!(newId = AppDB().insertTicket(category, num)))
        return shared_ptr<http_response>(new string_response("Could not insert new ticket!", 409));
    json j_response;
    j_response["id"] = newId;
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_response, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}

// .../testing/questions <- json {question} -> json {id}
std::shared_ptr<http_response> testing_resources::render_PUT_question(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second != User::TEACHER
     && uidAndPerms->second != User::SUPERUSER)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));


    json j_question;
    if (JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_question))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    Question question;
    if (JsonConverter::jsonToQuestion(j_question, question))
        return shared_ptr<http_response>(new string_response("Bad Question struct!", 400));

    int newId;
    if (!(newId = AppDB().insertQuestion(question)))
        return shared_ptr<http_response>(new string_response("Could not insert new question!", 409));
    json j_response;
    j_response["id"] = newId;
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_response, responseBody);
    return shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
}

// .../testing/tickets/by-id/{id|[0-9]+}
std::shared_ptr<http_response> testing_resources::render_DELETE_ticket(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second != User::TEACHER
     && uidAndPerms->second != User::SUPERUSER)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    if (AppDB().deleteTicket(std::stoi(req.get_arg("ticket-id"))))
        return shared_ptr<http_response>(new string_response("No such ticket!", 404));
    return shared_ptr<http_response>(new string_response("SUCCESS"));
}

// .../testing/questions/by-id/{id|[0-9]+}
std::shared_ptr<http_response> testing_resources::render_DELETE_question(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second != User::TEACHER
     && uidAndPerms->second != User::SUPERUSER)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    if (AppDB().deleteQuestion(std::stoi(req.get_arg("id"))))
        return shared_ptr<http_response>(new string_response("No such ticket!", 404));
    return shared_ptr<http_response>(new string_response("SUCCESS"));
}
// .../testing/questions/by-id/{id|[0-9]+} <- json {question}
std::shared_ptr<http_response> testing_resources::render_PATCH_question(const http_request &req)
{
    auto uidAndPerms = AppDB().getUserIdAndPermissionsByToken(string(req.get_header("token")));
    if (!uidAndPerms)
        return shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (uidAndPerms->second != User::TEACHER
     && uidAndPerms->second != User::SUPERUSER)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    json j_question;
    if (JsonConverter::jsonStringToJsonObject(string(req.get_content()), j_question))
        return shared_ptr<http_response>(new string_response("Bad JSON!", 400));
    Question question;
    if (JsonConverter::jsonToQuestion(j_question, question))
        return shared_ptr<http_response>(new string_response("Bad Question struct!", 400));

    int questionId = std::stoi(req.get_arg("question-id"));
    if (!AppDB().changeQuestion(questionId, question))
        return shared_ptr<http_response>(new string_response("Could not change the question!", 409));

    return shared_ptr<http_response>(new string_response("SUCCESS"));
}

