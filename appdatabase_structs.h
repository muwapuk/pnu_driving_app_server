#ifndef APPDATABASE_STRUCTS_H
#define APPDATABASE_STRUCTS_H

#include <cmath>
#include <string>
#include <sys/types.h>

namespace tickets {
enum Categories {
    AB,
    CD,
};
}

struct Question {
    static const int MIN_QUESTION_NUM = 1;

    uint number;
    uint ticketId;

    std::string subject;
    std::string image_base64;
    std::string questionText;
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
        GUEST
    } permissions = NONE;
};
struct Teacher {
    std::string login;
    std::string name;
    std::string car;
};
struct Student {
    std::string login;
    std::string name;
    std::string groupName;
    Teacher teacher;
};
struct Lecture {
    std::string teacher;
    std::string groupName;
    std::string title;
    std::string classroom;
    int time;
};
struct Practice {
    std::string teacher_login;
    std::string student_login;
    std::string title;
    std::string car;
    int time;
};



#endif // APPDATABASE_STRUCTS_H
