#ifndef APPDATABASE_STRUCTS_H
#define APPDATABASE_STRUCTS_H

#include <vector>
#include <string>
#include <sys/types.h>

namespace tickets {
enum Categories {
    UNDEFINED,
    AB,
    CD,
};
}

struct Question {
    int id;
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
    int id;
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
    int userId;
    std::string name;
    std::string car;
};
struct Student {
    int userId;
    std::string name;
    std::string groupName;
    Teacher teacher;
};
struct Lecture {
    int id;
    int teacherId;
    int groupId;
    std::string title;
    std::string classroom;
    int time;
};
// Needed for inserting
struct PracticeSlot {
    int teacherId;
    int time;
};
struct PracticeBooking {
    int studentId;
    int slotId;
};
//

// Needed for selecting
struct FreePracticeSlot {
    int slotId;
    int time;
};
struct BookedPracticeSlot {
    int slotId;
    int bookingId;
    int studentId;
    std::string studentName;
    int time;
};
//


#endif // APPDATABASE_STRUCTS_H
