#include "appdatabase.h"
#include "questions_resources.h"
#include "authentication_resourses.h"
#include "jsonconverter.h"
#include "appwebserver.h"

namespace qr {
// Used pass string arguments in switch expression

std::map <std::string, Question::Category> categories_strings = {
    {"AB", Question::AB},
    {"CD", Question::CD}
};

enum QuestionAttributes {
    UNDEFINED,
    IMAGE,
    THEME,
    QUESTION,
    ANSWER,
    RIGHT_ANSWER,
    COMMENT,
    BLANK
};
std::map <std::string, QuestionAttributes> question_attributes = {
    {"image", IMAGE},
    {"theme", THEME},
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

// .../questions/themes
// .../questions/themes/{theme}/questions/ids
// .../questions/themes/{theme}/questions/amount
// .../questions/categories/{category}/tickets/amount
// .../questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/amount
// .../questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/{question|[0-9]+}?attribute="..." -> json
std::shared_ptr<http_response>
questions_resource::render_GET(const http_request &req)
{
    AppWebserver::printDebug(req);

    auto loginAndPerms = auth::tokenToUserAndPermenissions(string(req.get_header("token")));
    if (!loginAndPerms)
        return std::shared_ptr<http_response>(new string_response("Bad Token", 401));
    if (loginAndPerms->second == User::NONE)
        return std::shared_ptr<http_response>(new string_response("Forbidden", 403));

    if (req.get_path_piece(1) == "themes") {
        if (!req.get_arg("theme").values.empty()) {
            if(req.get_path_piece(4) == "ids" ) {
                json j_response = {};
                vector<int> *ids = AppDB().getQuestionsIdsByTheme(req.get_arg("theme"));
                for (auto &id : *ids) {
                    json j_response;
                    j_response.push_back(id);
                }
                delete ids;
                string s_response;
                JsonConverter::jsonToJsonString(j_response, s_response);
                auto response = std::shared_ptr<http_response>(new string_response("SUCCESS"));
                response->with_header("ids", s_response);
                return response;
            } else if (req.get_path_piece(4) == "amount") {
                int amount = AppDB().getThemeQuestionsAmount(req.get_arg("theme"));
                auto response = std::shared_ptr<http_response>(new string_response("SUCCESS"));
                response->with_header("questions-amount", std::to_string(amount));
                return response;
            } else {
                return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));
            }
        } else {
            json j_response = {};
            vector<string> *themes = AppDB().getQuestionsThemes();
            for (auto &theme : *themes) {
                json j_response;
                j_response.push_back(theme);
            }
            delete themes;
            string s_response;
            JsonConverter::jsonToJsonString(j_response, s_response);
            auto response = std::shared_ptr<http_response>(new string_response("SUCCESS"));
            response->with_header("themes", s_response);
            return response;
        }
    }

    if (!req.get_arg("category").values.empty()
      && req.get_path_piece(4) == "amount") {
        int ticketsAmount = AppDB().getTicketsABamount();
        auto response = std::shared_ptr<http_response>(new string_response("SUCCESS"));
        response->with_header("tickets-amount", std::to_string(ticketsAmount));
        return response;
    }
    if (!req.get_arg("category").values.empty()
     && !req.get_arg("ticket").values.empty()
      && req.get_path_piece(6) == "amount") {
        auto category = categories_strings[std::string(req.get_arg("category"))];
        int ticketNum = std::stoi(std::string(req.get_arg("ticket")));
        int ticketsAmount = AppDB().getTicketQuestionsAmount(category, ticketNum);
        auto response = std::shared_ptr<http_response>(new string_response("SUCCESS"));
        response->with_header("questions-amount", std::to_string(ticketsAmount));
        return response;
    }


    if (req.get_arg("category").values.empty()
     || req.get_arg("ticket").values.empty()
     || req.get_arg("question").values.empty()
    ) return std::shared_ptr<http_response>(new string_response("Resource not specified!", 400));

    std::string responseStr;
    Question::Category category = categories_strings[std::string(req.get_arg("category"))];
    int ticket_num = std::stoi(std::string(req.get_arg("ticket")));
    int question_num = std::stoi(std::string(req.get_arg("question")));

    switch (category) {
    case Question::AB:
        if (ticket_num > AppDB().getTicketsABamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::CD:
        if (ticket_num > AppDB().getTicketsCDamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::UNDEFINED:
        return std::shared_ptr<http_response>(new string_response("Category not found!", 404));
    default:
        break;
    }

    Question *question;
    if(!(question = AppDB().getQuestion(category, ticket_num, question_num))) {
        return std::shared_ptr<http_response>(new string_response("Question not found!", 404));
    }
    json questJson;
    JsonConverter::questionToJson(*question, questJson);
    auto response = std::shared_ptr<http_response>(new string_response("SUCCESS"));

    switch(question_attributes[std::string(req.get_arg("attribute"))]) {
    case IMAGE:
        JsonConverter::jsonObjectToString(questJson, "image", responseStr);
        response->with_header("image", responseStr);
        break;
    case THEME:
        JsonConverter::jsonObjectToString(questJson, "theme", responseStr);
        response->with_header("theme", responseStr);
        break;
    case QUESTION:
        JsonConverter::jsonObjectToString(questJson, "questionText", responseStr);
        response->with_header("question-text", responseStr);
        break;
    case ANSWER:
        JsonConverter::jsonObjectToString(questJson, "answers", responseStr);
        response->with_header("answers", responseStr);
        break;
    case RIGHT_ANSWER:
        JsonConverter::jsonObjectToString(questJson, "rightAnswer", responseStr);
        response->with_header("right-answer", responseStr);
        break;
    case COMMENT:
        JsonConverter::jsonObjectToString(questJson, "comment", responseStr);
        response->with_header("comment", responseStr);
        break;
    case BLANK:
        JsonConverter::jsonToJsonString(questJson, responseStr);
        response->with_header("question", responseStr);
        break;
    case UNDEFINED:
        return std::shared_ptr<http_response>(new string_response("Attribute not found"));

    }

    delete question;

    return response;
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
    Question::Category category = categories_strings[std::string(req.get_arg("category"))];
    int ticket_num = std::stoi(std::string(req.get_arg("ticket")));
    int question_num = std::stoi(std::string(req.get_arg("question")));

    // Check args match bounds
    switch (category) {
    case Question::AB:
        if (ticket_num > AppDB().getTicketsABamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::CD:
        if (ticket_num > AppDB().getTicketsCDamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::UNDEFINED:
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

    Question::Category category = categories_strings[std::string(req.get_arg("category"))];
    int ticket_num = std::stoi(std::string(req.get_arg("ticket")));
    int question_num = std::stoi(std::string(req.get_arg("question")));

    // Check args match bounds
    switch (category) {
    case Question::AB:
        if (ticket_num > AppDB().getTicketsABamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::CD:
        if (ticket_num > AppDB().getTicketsCDamount() || ticket_num < 1)
            return std::shared_ptr<http_response>(new string_response("Ticket not found!", 404));
        break;
    case Question::UNDEFINED:
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
