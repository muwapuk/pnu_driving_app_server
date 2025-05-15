#include "jsonconverter.h"

using json = nlohmann::json;

JsonConverter::Result JsonConverter::jsonStringToJson(const std::string &str, nlohmann::json &j)
{
    json json;
    try {
        json = json::parse(str);
    } catch (json::exception &e) {
        return PARSE_ERROR;
    }
    j = json;
    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonToJsonString(nlohmann::json &j, std::string &str)
{
    std::stringstream ss;

    ss << j;
    str = ss.str();

    return SUCCESS;
}

void JsonConverter::mergeJson(nlohmann::json &j, const nlohmann::json &j_patch)
{
    j.merge_patch(j_patch);
}

JsonConverter::Result JsonConverter::jsonObjectToString(const nlohmann::json &j, std::string key, std::string &out)
{
    if (!j[key].is_string())
        return CONVERTATION_ERROR;

    j[key].get_to(out);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::stringToJson(std::string key, const std::string &obj, nlohmann::json &j)
{
    j[key] = obj;

    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonObjectToInt(const nlohmann::json &j, std::string key, int &out)
{
    if (!j[key].is_number())
        return CONVERTATION_ERROR;

    j[key].get_to(out);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::intToJson(std::string key, int obj, nlohmann::json &j)
{
    j[key] = obj;

    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonToQuestion(nlohmann::json &j, Question &question)
{
    if (!j["category"].is_number()
     || !j["ticketNum"].is_number()
     || !j["questionNum"].is_number()
     || !j["image"].is_string()
     || !j["questionText"].is_string()
     || !j["answers"].is_string()
     || !j["rightAnswer"].is_number()
     || !j["comment"].is_string()
    ) return CONVERTATION_ERROR;

    j["category"].get_to(question.category);
    j["ticketNum"].get_to(question.ticket_num);
    j["questionNum"].get_to(question.question_num);
    j["image"].get_to(question.image_base64);
    j["questionText"].get_to(question.question_text);
    j["answers"].get_to(question.answers);
    j["rightAnswer"].get_to(question.rightAnswer);
    j["comment"].get_to(question.comment);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::questionToJson(const Question &question, nlohmann::json &j)
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

    j.merge_patch(questionJson);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonToUser(const nlohmann::json &j, User &user)
{
    if (!j["login"].is_string()
     || !j["password"].is_string()
     || !j["name"].is_string()
     || !j["permissions"].is_number()
    ) return CONVERTATION_ERROR;

    j["login"].get_to(user.login);
    j["password"].get_to(user.password);
    j["name"].get_to(user.name);
    j["permissions"].get_to(user.permissions);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::userToJson(const User &user, nlohmann::json &j)
{
    json userJson = {
        {"login", user.login},
        {"password", user.password},
        {"name", user.name},
        {"permissions", user.permissions},
    };

    j.merge_patch(userJson);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonToLecture(nlohmann::json &j, Lecture &lecture)
{
    if (!j["teacher_name"].is_string()
        || !j["group_name"].is_string()
        || !j["thematic"].is_string()
        || !j["cabinet"].is_number()
        || !j["date"].is_number()
        ) return CONVERTATION_ERROR;

    j["teacher_name"].get_to(lecture.teacher_name);
    j["group_name"].get_to(lecture.group_name);
    j["thematic"].get_to(lecture.thematic);
    j["cabinet"].get_to(lecture.cabinet);

    std::string datestr;
    j["date"].get_to(datestr);
    if (!lecture.date.loadFromString(datestr))
        return PARSE_ERROR;

    return SUCCESS;
}

JsonConverter::Result JsonConverter::lectureToJson(Lecture &lecture, nlohmann::json &j)
{
    json lectureJson = {
                     {"login", lecture.teacher_name},
                     {"password", lecture.group_name},
                     {"name", lecture.thematic},
                     {"permissions", lecture.cabinet},
                     {"permissions", lecture.date.getDateString()}
                    };

    j.merge_patch(lectureJson);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonToPractice(nlohmann::json &j, Practice &practice)
{
    if (!j["teacher_name"].is_string()
        || !j["student_name"].is_string()
        || !j["thematic"].is_string()
        || !j["car"].is_number()
        || !j["date"].is_number()
        ) return CONVERTATION_ERROR;

    j["teacher_name"].get_to(practice.teacher_name);
    j["student_name"].get_to(practice.student_name);
    j["thematic"].get_to(practice.thematic);
    j["car"].get_to(practice.car);

    std::string datestr;
    j["date"].get_to(datestr);
    if (!practice.date.loadFromString(datestr))
        return PARSE_ERROR;

    return SUCCESS;
}

JsonConverter::Result JsonConverter::practiceToJson(Practice &practice, nlohmann::json &j)
{
    json lectureJson = {
        {"teacher_name", practice.teacher_name},
        {"student_name", practice.student_name},
        {"thematic", practice.thematic},
        {"car", practice.car},
        {"date", practice.date.getDateString()}
    };

    j.merge_patch(lectureJson);


    return SUCCESS;
}












