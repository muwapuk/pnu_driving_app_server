#include "appdatabase.h"
#include "questions_resources.h"
#include "jsonconverter.h"

namespace qr {
// Used pass string arguments in switch expression

std::map <std::string, Question::Category> categories_strings = {
    {"AB", Question::AB},
    {"CD", Question::CD}
};

enum QuestionAttributes {
    UNDEFINED,
    IMAGE,
    QUESTION,
    ANSWER,
    RIGHT_ANSWER,
    COMMENT,
    BLANK
};
std::map <std::string, QuestionAttributes> question_attributes = {
    {"image", IMAGE},
    {"question", QUESTION},
    {"answer", ANSWER},
    {"right-answer", RIGHT_ANSWER},
    {"comment", COMMENT},
    {"", BLANK}
};

}

using namespace qr;
using json = nlohmann::json;

/////////////////////////
// QUESTIONS RESOURCES //
/////////////////////////

// .../questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+}?attribute="..." -> json
std::shared_ptr<http_response>
questions_resource::render_GET(const http_request &req)
{
    if (req.get_arg("category").values.empty()
     || req.get_arg("category").values.empty()
     || req.get_arg("category").values.empty()
    ) return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));

    std::string responseStr;
    Question::Category category = categories_strings[std::string(req.get_arg("category"))];
    int ticket_num = std::stoi(std::string(req.get_arg("ticket")));
    int question_num = std::stoi(std::string(req.get_arg("question")));

    switch (category) {
    case Question::AB:
        if (ticket_num > AppDB()->getTicketsABamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::CD:
        if (ticket_num > AppDB()->getTicketsCDamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::UNDEFINED:
        return std::shared_ptr<http_response>(new string_response("Category not found!", 404));
    default:
        break;
    }

    Question *question;
    if(!(question = AppDB()->getQuestion(category, ticket_num, question_num))) {
        return std::shared_ptr<http_response>(new string_response("Question not found!", 404));
    }
    json questJson;
    JsonConverter::questionToJson(*question, questJson);

    switch(question_attributes[std::string(req.get_arg("attribute"))]) {
    case IMAGE:
        JsonConverter::jsonObjectToString(questJson, "image", responseStr);
        break;
    case QUESTION:
        JsonConverter::jsonObjectToString(questJson, "questionText", responseStr);
        break;
    case ANSWER:
        JsonConverter::jsonObjectToString(questJson, "answers", responseStr);
        break;
    case RIGHT_ANSWER:
        JsonConverter::jsonObjectToString(questJson, "rightAnswer", responseStr);
        break;
    case COMMENT:
        JsonConverter::jsonObjectToString(questJson, "comment", responseStr);
        break;
    case BLANK:
        JsonConverter::jsonToJsonString(questJson, responseStr);
        break;
    case UNDEFINED:
        return std::shared_ptr<http_response>(new string_response("Attribute not found"));
    }

    delete question;
    return std::shared_ptr<http_response>(new string_response(responseStr));
}

// .../questions <- json
std::shared_ptr<http_response>
questions_resource::render_PUT(const http_request &req)
{
    Question question;
    json jsonQuest;

    // Check json correctness
    if(JsonConverter::SUCCESS != JsonConverter::stringToJson(
        static_cast<std::string>(req.get_content()), jsonQuest)
    ) return std::shared_ptr<http_response>(new string_response("Bad JSON!", 400));

    if(JsonConverter::SUCCESS != JsonConverter::jsonToQuestion(jsonQuest, question))
        return std::shared_ptr<http_response>(new string_response("JSON struct does not match question struct!", 400));

    if(!AppDB()->addQuestion(question))
        return std::shared_ptr<http_response>(new string_response("Could not add the question!", 500));

    return std::shared_ptr<http_response>(new string_response("SUCCESS"));
}

// .../questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+}
std::shared_ptr<http_response>
questions_resource::render_DELETE(const http_request &req)
{
    // Check args are given
    if (req.get_arg("category").values.empty()
        || req.get_arg("category").values.empty()
        || req.get_arg("category").values.empty()
        ) return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));

    std::string responseStr;
    Question::Category category = categories_strings[std::string(req.get_arg("category"))];
    int ticket_num = std::stoi(std::string(req.get_arg("ticket")));
    int question_num = std::stoi(std::string(req.get_arg("question")));

    // Check args match bounds
    switch (category) {
    case Question::AB:
        if (ticket_num > AppDB()->getTicketsABamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::CD:
        if (ticket_num > AppDB()->getTicketsCDamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::UNDEFINED:
        return std::shared_ptr<http_response>(new string_response("Category not found!", 404));
    default:
        break;
    }

    if(!(AppDB()->deleteQuestion(category, ticket_num, question_num))) {
        return std::shared_ptr<http_response>(new string_response("Question not found!", 404));
    }

    return std::shared_ptr<http_response>(new string_response("SUCCESS"));
}

// .../questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+} <- json
std::shared_ptr<http_response>
questions_resource::render_PATCH(const http_request &req)
{
    Question *question;
    json jsonPatch;
    json jsonQuest;
    std::string responseStr;

    // Check args are given
    if (req.get_arg("category").values.empty()
     || req.get_arg("category").values.empty()
     || req.get_arg("category").values.empty()
    ) return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));

    Question::Category category = categories_strings[std::string(req.get_arg("category"))];
    int ticket_num = std::stoi(std::string(req.get_arg("ticket")));
    int question_num = std::stoi(std::string(req.get_arg("question")));

    // Check args match bounds
    switch (category) {
    case Question::AB:
        if (ticket_num > AppDB()->getTicketsABamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::CD:
        if (ticket_num > AppDB()->getTicketsCDamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::UNDEFINED:
        return std::shared_ptr<http_response>(new string_response("Category not found!", 404));
    default:
        break;
    }

    // Check json correctness
    if(JsonConverter::SUCCESS != JsonConverter::stringToJson(
            static_cast<std::string>(req.get_content()), jsonPatch)
    ) return std::shared_ptr<http_response>(new string_response("Bad JSON!", 400));

    if(!(question = AppDB()->getQuestion(category, ticket_num, question_num))) {
        return std::shared_ptr<http_response>(new string_response("Question not found!", 404));
    }

    JsonConverter::questionToJson(*question, jsonQuest);
    JsonConverter::mergeJson(jsonQuest, jsonPatch);
    JsonConverter::jsonToQuestion(jsonQuest, *question);

    AppDB()->modifyQuestion(*question);

    delete question;
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

std::shared_ptr<http_response>
questions_results_resource::render_GET(const http_request &req)
{
    return std::shared_ptr<http_response>(
        new string_response("Hello: " + std::string(req.get_arg("name"))));
}

std::shared_ptr<http_response>
questions_results_resource::render_PUT(const http_request &req)
{
    return std::shared_ptr<http_response>(
        new string_response("Hello: " + std::string(req.get_arg("name"))));
}

std::shared_ptr<http_response>
questions_results_resource::render(const http_request &req)
{
    return std::shared_ptr<http_response>(
        new string_response("Hello: " + std::string(req.get_arg("name"))));
}
