#include "appdatabase.h"
#include "questions_resources.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"
#include "appwebserver.h"

namespace qr {
// Used pass string arguments in switch expression

std::map <std::string, int> categories_strings = {
    {"AB", tickets::AB},
    {"CD", tickets::CD}
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
// .../testing/categories/{category}/tickets/by-id/{id|[0-9]+}/questions-id-list
// .../testing/questions/by-id/{id|[0-9]+}

std::shared_ptr<http_response>
questions_resource::render_GET(const http_request &req)
{
    AppWebserver::printDebug(req);

    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return std::shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    // .../testing/subjects...
    if (req.get_path_piece(1) == "subjects") {
        // .../testing/subjects
        if (req.get_path().size() == 2) {
            return buildSubjectsResponse();
        // .../testing/subjects/{subject}/questions-id-list
        } else if (req.get_path_piece(3) == "questions-id-list") {
            return buildQuestionsIdBySubjectResponse(req.get_arg("subject"));
        } else {
            return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));
        }
    } else
    // .../testing/categories/{category}/tickets...
    if (req.get_path_piece(3) == "tickets") {
        if (req.get_path().size() == 4) {
            // .../testing/categories/{category}/tickets
            return buildTicketsIdResponse(req.get_arg("category"));
        } else if (req.get_path_piece(4) == "by-id"
                && req.get_path_piece(6) == "questions-id-list") {
            // .../testing/categories/{category}/tickets/by-id/{id|[0-9]+}/questions-id-list
            return buildQuestionsIdBySubjectResponse(req.get_arg("subject"));
        } else {
            return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));
        }
    } else
    // .../testing/questions/by-id/{id|[0-9]+}
    if (req.get_path_piece(1) == "questions"
    && !req.get_arg("id").values.empty()) {
        return buildQuestionResponse(std::stoi(req.get_arg("id")));
    } else {
        return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));
    }
}

// .../questions <- json
std::shared_ptr<http_response>
questions_resource::render_PUT(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return std::shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second != User::TEACHER
     || loginAndPerms->second != User::SUPERUSER
    ) return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    Question question;
    json jsonQuest;

    if(JsonConverter::SUCCESS != JsonConverter::jsonStringToJson(
        static_cast<std::string>(req.get_content()), jsonQuest)
    ) return std::shared_ptr<http_response>(new string_response("Bad JSON!", 400));

    if(JsonConverter::SUCCESS != JsonConverter::jsonToQuestion(jsonQuest, question))
        return std::shared_ptr<http_response>(new string_response("JSON struct does not match question struct!", 400));

    if(!AppDB().addQuestion(question))
        return std::shared_ptr<http_response>(new string_response("Could not add the question!", 500));

    return std::shared_ptr<http_response>(new string_response("SUCCESS"));
}

// .../questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+}
std::shared_ptr<http_response>
questions_resource::render_DELETE(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return std::shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second != User::TEACHER
        || loginAndPerms->second != User::SUPERUSER
        ) return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    // Check args are given
    if (req.get_arg("category").values.empty()
        || req.get_arg("ticket").values.empty()
        || req.get_arg("question").values.empty()
        ) return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));

    std::string responseStr;
    tickets::Categories category = categories_strings[std::string(req.get_arg("category"))];
    int ticket_num = std::stoi(std::string(req.get_arg("ticket")));
    int question_num = std::stoi(std::string(req.get_arg("question")));

    // Check args match bounds
    switch (category) {
    case tickets::AB:
        if (ticket_num > AppDB().getTicketsABamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case tickets::CD:
        if (ticket_num > AppDB().getTicketsCDamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case tickets::UNDEFINED:
        return std::shared_ptr<http_response>(new string_response("Category not found!", 404));
    default:
        break;
    }

    if(!(AppDB().deleteQuestion(category, ticket_num, question_num))) {
        return std::shared_ptr<http_response>(new string_response("Question not found!", 404));
    }

    return std::shared_ptr<http_response>(new string_response("SUCCESS"));
}

// .../questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/{question|[0-9]+} <- json
std::shared_ptr<http_response>
questions_resource::render_PATCH(const http_request &req)
{
    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return std::shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second != User::TEACHER
     || loginAndPerms->second != User::SUPERUSER
    ) return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    Question *question;
    json jsonPatch;
    json jsonQuest;
    std::string responseStr;

    // Check args are given
    if (req.get_arg("category").values.empty()
     || req.get_arg("ticket").values.empty()
     || req.get_arg("question").values.empty()
    ) return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));

    tickets::Categories category = categories_strings[std::string(req.get_arg("category"))];
    int ticket_num = std::stoi(std::string(req.get_arg("ticket")));
    int question_num = std::stoi(std::string(req.get_arg("question")));

    // Check args match bounds
    switch (category) {
    case tickets::AB:
        if (ticket_num > AppDB().getTicketsABamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case tickets::CD:
        if (ticket_num > AppDB().getTicketsCDamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case tickets::UNDEFINED:
        return std::shared_ptr<http_response>(new string_response("Category not found!", 404));
    default:
        break;
    }

    // Check json correctness
    if(JsonConverter::SUCCESS != JsonConverter::jsonStringToJson(
            static_cast<std::string>(req.get_content()), jsonPatch)
    ) return std::shared_ptr<http_response>(new string_response("Bad JSON!", 400));

    if(!(question = AppDB().getQuestion(category, ticket_num, question_num))) {
        return std::shared_ptr<http_response>(new string_response("Question not found!", 404));
    }

    JsonConverter::questionToJson(*question, jsonQuest);
    JsonConverter::mergeJson(jsonQuest, jsonPatch);
    JsonConverter::jsonToQuestion(jsonQuest, *question);

    AppDB().modifyQuestion(*question);

    return std::shared_ptr<http_response>(new string_response("SUCCESS"));
}

std::shared_ptr<http_response>
questions_resource::render(const http_request &)
{
  return std::shared_ptr<http_response>(new string_response("Bad request!", 400));
}

/////////////////////////////////
// QUESTIONS RESULTS RESOURCES //
/////////////////////////////////

std::shared_ptr<http_response> questions_resource::buildSubjectsResponse()
{
    json j_response = {};
    vector<string> *subjects = AppDB().getQuestionssubjects();
    for (auto &subject : *subjects) {
        json j_response;
        j_response.push_back(subject);
    }
    string responseBody;
    JsonConverter::jsonToJsonString(j_response, responseBody);
    auto response = std::shared_ptr<http_response>(new string_response("SUCCESS"));
    response->with_header("subjects", responseBody);
}

std::shared_ptr<http_response> questions_resource::buildTicketsIdResponse(string categoryString)
{

}

std::shared_ptr<http_response> questions_resource::buildQuestionsIdBySubjectResponse(std::string subject)
{
    json j_response = {};
    auto ids = AppDB().getQuestionsIdBySubject(req.get_arg("subject"));
    for (auto &id : *ids) {
        json j_response;
        j_response.push_back(id);
    }
    string responseBody;
    JsonConverter::jsonObjectToJsonString(j_response, responseBody);
    auto response = std::shared_ptr<http_response>(new string_response(responseBody, 200, "application/json"));
    response->with_header("ids", responseBody);
}

std::shared_ptr<http_response> questions_resource::buildQuestionsIdByTicketResponse(int ticketId)
{

}

std::shared_ptr<http_response> questions_resource::buildQuestionResponse(int questionId)
{

}

std::shared_ptr<http_response> questions_resource::buildAnswerResponse(int questionId, int answer)
{

}
