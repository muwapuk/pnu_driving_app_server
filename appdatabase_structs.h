#ifndef APPDATABASE_STRUCTS_H
#define APPDATABASE_STRUCTS_H

#include <cmath>
#include <string>
#include <sys/types.h>

struct Question {
    static const int MIN_TICKET_NUM = 1;
    static const int MAX_TICKET_NUM = 99;
    static const int MIN_QUESTION_NUM = 1;

    enum Category {
        UNDEFINED,
        AB,
        CD,
        TOTAL_CATEGORIES
    } category;

    uint ticket_num;
    uint question_num;
    // In database it has unique id consisting of question_num*1000 + ticket_num*10 + category

    std::string theme;
    std::string image_base64;
    std::string question_text;
    std::string answers;
    std::string comment;
    uint rightAnswer;
};

struct User {
    std::string login;
    std::string password;
    std::string name;
    enum Permissions {
        NONE,
        SUPERUSER,
        TEACHER,
        STUDENT,
    } permissions = NONE;
};



struct Lecture {
    std::string teacher_login;
    std::string group_name;
    std::string thematic;
    std::string cabinet;
    int date;
};

struct Practice {
    std::string teacher_login;
    std::string student_login;
    std::string thematic;
    std::string car;
    int date;
};



#endif // APPDATABASE_STRUCTS_H
