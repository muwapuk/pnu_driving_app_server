#include "users_resources.h"
#include "appdatabase.h"
#include "jsonconverter.h"

using json = nlohmann::json;
using namespace ur;



namespace ur {
// Used pass string arguments in switch expression


std::map <std::string, UserAttributes> user_attributes = {
    {"name", NAME},
    {"password", PASSWORD},
    {"permissions", PERMISSIONS},
    {"", BLANK}
};

}

User *users_resources::getUser(std::string user)
{
    return AppDB()->getUser(user);
}

std::vector<User> *users_resources::getUsersPage(int page)
{
    return AppDB()->getUsers(1 + (page-1)*MAX_JSON_ARRAY_SIZE, MAX_JSON_ARRAY_SIZE);
}
User *users_resources::getUserAttribute(std::string user, UserAttributes attribute)
{
    // switch(question_attributes[std::string(req.get_arg("attribute"))]) {
    // case NAME:
    //     JsonConverter::jsonObjectToString(questJson, "image", responseStr);
    //     break;
    // case PASSWORD:
    //     JsonConverter::jsonObjectToString(questJson, "questionText", responseStr);
    //     break;
    // case PERMISSIONS:
    //     JsonConverter::jsonObjectToString(questJson, "answers", responseStr);
    //     break;

    // case BLANK:
    //     JsonConverter::jsonToJsonString(questJson, responseStr);
    //     break;
    // case UNDEFINED:
    //     return std::shared_ptr<http_response>(new string_response("Attribute not found"));
    // }
}

std::vector<User> *users_resources::getUsersAttributesPage(UserAttributes, int page)
{

}

////////////////////
// USER RESOURSES //
////////////////////

// .../users/{page|[0-9]+}?login="..."&attribute="..." -> json
std::shared_ptr<http_response> users_resources::render_GET(const http_request &req)
{
    std::string response;
    int page;

    if (!req.get_arg("category").values.empty())
        page = std::stoi(std::string(req.get_arg("page")));
    else
        page = 1;

    UserAttributes attribute = user_attributes[std::string(req.get_arg("attribute"))];

    if (attribute == UNDEFINED)
        return std::shared_ptr<http_response>(new string_response("Incorrect attribute!", 400));

    if (req.get_arg("login").values.empty()) {
        if (attribute == BLANK) {
            getUsersPage(page);
        } else {
            getUsersAttributesPage(attribute, page);
        }
    } else {
        if (attribute == BLANK) {
            getUser(req.get_arg("login"));
        } else {
            getUserAttribute(req.get_arg("login"), attribute);
        }
    }


    // Question *question;
    // if(!(question = AppDB()->getQuestion(category, ticket_num, question_num))) {
    //     return std::shared_ptr<http_response>(new string_response("Question not found!", 404));
    // }
    // json questJson;
    // JsonConverter::questionToJson(*question, questJson);

    // switch(question_attributes[std::string(req.get_arg("attribute"))]) {
    // case IMAGE:
    //     JsonConverter::jsonObjectToString(questJson, "image", responseStr);
    //     break;
    // case QUESTION:
    //     JsonConverter::jsonObjectToString(questJson, "questionText", responseStr);
    //     break;
    // case ANSWER:
    //     JsonConverter::jsonObjectToString(questJson, "answers", responseStr);
    //     break;
    // case RIGHT_ANSWER:
    //     JsonConverter::jsonObjectToString(questJson, "rightAnswer", responseStr);
    //     break;
    // case COMMENT:
    //     JsonConverter::jsonObjectToString(questJson, "comment", responseStr);
    //     break;
    // case BLANK:
    //     JsonConverter::jsonToJsonString(questJson, responseStr);
    //     break;
    // case UNDEFINED:
    //     return std::shared_ptr<http_response>(new string_response("Attribute not found"));
    // }

    // delete question;
    // return std::shared_ptr<http_response>(new string_response(responseStr));
}

std::shared_ptr<http_response> users_resources::render_PUT(const http_request &req)
{

}

std::shared_ptr<http_response> users_resources::render(const http_request &req)
{

}
