#ifndef APPDATABASE_STRUCTS_H
#define APPDATABASE_STRUCTS_H

#include <vector>
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
    std::string teacherLogin;
    std::string groupId;
    std::string title;
    std::string classroom;
    int time;
};
// Needed for inserting
struct PracticeSlot {
    std::string teacher_login;
    int time;
};
struct PracticeBooking {
    std::string student_login;
    int slot_id;
};
//

// Needed for selecting
struct FreePracticeSlot {
    int id;
    int time;
};
struct BookedPracticeSlot {
    int id;
    int booking_id;
    std::string studentLogin;
    std::string studentName;
    int time;
};
//


#endif // APPDATABASE_STRUCTS_H
