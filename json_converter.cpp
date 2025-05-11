#include "json_converter.h"
#include <iostream>

    using json = nlohmann::json;

Question *json_converter::jsonStringToQuestion(std::string &jsonQuestion)
{
    json j = json::parse(jsonQuestion);
    Question *newQuestion = new Question;/* {
        j["category"],
        j["ticketNum"],
        j["questionNum"],
        j["image"],
        j["questionText"],
        j["answers"],
        j["rightAnswer"],
        j["comment"],
    };*/

    j["category"].get_to(newQuestion->category);
    j["ticketNum"].get_to(newQuestion->ticket_num);
    j["questionNum"].get_to(newQuestion->question_num);
    j["image"].get_to(newQuestion->image_base64);
    j["questionText"].get_to(newQuestion->question_text);
    j["answers"].get_to(newQuestion->answers);
    j["rightAnswer"].get_to(newQuestion->rightAnswer);
    j["comment"].get_to(newQuestion->comment);

    return newQuestion;
}

std::string *json_converter::questionToJsonString(Question &question)
{
    json questionJson = {
        {"category", question.category},
        {"ticketNum", question.ticket_num},
        {"questionNum", question.question_num},
        {"image", question.image_base64},
        {"questionText", question.question_text},
        {"answers", question.answers},
        {"rightAnswer", question.rightAnswer},
        {"comment", question.comment}
    };

    auto jsonString = new std::string;
    std::stringstream ss;

    ss << questionJson;
    *jsonString = ss.str();

    return jsonString;
}

