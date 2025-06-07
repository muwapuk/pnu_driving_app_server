#include "jsonconverter.h"

using json = nlohmann::json;

JsonConverter::Result JsonConverter::jsonStringToJsonObject(const std::string &str, nlohmann::json &j)
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

JsonConverter::Result JsonConverter::jsonObjectToJsonString(nlohmann::json &j, std::string &str)
{
    std::stringstream ss;

    ss << j;
    str = ss.str();

    return SUCCESS;
}

void JsonConverter::mergeJsonObjects(nlohmann::json &j, const nlohmann::json &j_patch)
{
    j.merge_patch(j_patch);
}

JsonConverter::Result JsonConverter::jsonValueToString(const nlohmann::json &j, std::string key, std::string &out)
{
    if (!j[key].is_string())
        return CONVERTATION_ERROR;

    j[key].get_to(out);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::stringToJsonValue(std::string key, const std::string &obj, nlohmann::json &j)
{
    j[key] = obj;

    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonValueToInt(const nlohmann::json &j, std::string key, int &out)
{
    if (!j[key].is_number())
        return CONVERTATION_ERROR;

    j[key].get_to(out);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::intToJsonValue(std::string key, int obj, nlohmann::json &j)
{
    j[key] = obj;

    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonToQuestion(nlohmann::json &j, Question &question)
{
    if (!j["ticketNum"].is_number()
     || !j["questionNum"].is_number()
     || !j["subject"].is_string()
     || !j["image"].is_string()
     || !j["questionText"].is_string()
     || !j["answers"].is_string()
     || !j["rightAnswer"].is_number()
     || !j["comment"].is_string()
    ) return CONVERTATION_ERROR;



    j["ticket-id"].get_to(question.ticketId);
    j["number"].get_to(question.number);
    j["subject"].get_to(question.subject);
    j["image"].get_to(question.image_base64);
    j["questionText"].get_to(question.questionText);
    j["answers"].get_to(question.answers);
    j["rightAnswer"].get_to(question.rightAnswer);
    j["comment"].get_to(question.comment);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::questionToJson(const Question &question, nlohmann::json &j)
{
    json questionJson = {
        {"ticket-id", question.ticketId},
        {"number", question.number},
        {"subject", question.subject},
        {"image", question.image_base64},
        {"question-text", question.questionText},
        {"answers", question.answers},
        {"rightAnswer", question.rightAnswer},
        {"comment", question.comment}
    };

    j.merge_patch(questionJson);

    return SUCCESS;
}

#ifdef STRUCTS_CONVERTERS
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
        {"name", user.name},
        {"permissions", user.permissions},
    };

    j.merge_patch(userJson);

    return SUCCESS;
}

#endif
JsonConverter::Result JsonConverter::jsonToLecture(nlohmann::json &j, Lecture &lecture)
{
    if (!j["teacher-id"].is_number()
        || !j["group-id"].is_number()
        || !j["classroom"].is_string()
        || !j["title"].is_number()
        || !j["time"].is_number()
        ) return CONVERTATION_ERROR;

    j["teacher-id"].get_to(lecture.teacherId);
    j["group-id"].get_to(lecture.groupId);
    j["classroom"].get_to(lecture.classroom);
    j["title"].get_to(lecture.title);
    j["time"].get_to(lecture.time);


    return SUCCESS;
}

JsonConverter::Result JsonConverter::lectureToJson(Lecture &lecture, nlohmann::json &j)
{
    json lectureJson = {
        {"teacher-id", lecture.teacherId},
        {"group-id", lecture.groupId},
        {"classroom", lecture.classroom},
        {"title", lecture.title},
        {"time", lecture.time}
    };

    j.merge_patch(lectureJson);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonToPracticeSlot(nlohmann::json &j, PracticeSlot &slot)
{
    if (!j["teacher-id"].is_number()
        || !j["time"].is_number()
    ) return CONVERTATION_ERROR;

    j["teacher-id"].get_to(slot.teacherId);
    j["time"].get_to(slot.time);

    return SUCCESS;
}

JsonConverter::Result JsonConverter::jsonToPracticeBooking(nlohmann::json &j, PracticeBooking &booking)
{
    if (!j["student-id"].is_string()
        || !j["time"].is_number()
        ) return CONVERTATION_ERROR;

    j["student-id"].get_to(booking.studentId);
    j["slot-id"].get_to(booking.slotId);

    return SUCCESS;
}










