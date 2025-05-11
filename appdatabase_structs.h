#ifndef APPDATABASE_STRUCTS_H
#define APPDATABASE_STRUCTS_H

#include <string>
#include <sys/types.h>

struct Question
{
    enum Category {
        AB,
        CD
    } category;

    uint ticket_num;
    uint question_num;
    // In database it has unique id consisting of (ticket_num + question_num*100)*10 + category

    std::string image_base64;
    std::string question_text;
    std::string answers;
    uint rightAnswer;
    std::string comment;
};

struct User
{
    std::string login;
    std::string password;
    std::string name;
    enum Permissions {
        superuser,
        teacher,
        student,
    } permissions = student;
};

#endif // APPDATABASE_STRUCTS_H
