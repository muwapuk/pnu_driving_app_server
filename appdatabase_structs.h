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
    // In database it has unique id consisting of (ticket_num + question_num*100)*10 + category

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
        superuser,
        teacher,
        student,
    } permissions = student;
};

struct DBDate {
    DBDate() {}
    DBDate(std::string strDate)
    {
        this->loadFromString(strDate);
    }

    int year;
    int month;
    int day;

    int hour;
    int min;

    bool initialized = false;

    ///
    /// \brief loadFromString
    /// \param date in format "YYYY-MM-DD hh:mm"
    /// \return true if parsed
    ///
    bool loadFromString(std::string date)
    {
        // Date: "YYYY-MM-DD hh:mm"
        try {
            year = stoi(date.substr(0,4));
            month = stoi(date.substr(5,2));
            day = stoi(date.substr(8,2));

            hour = stoi(date.substr(11,2));
            min = stoi(date.substr(14,2));
        } catch(std::exception &e) {
            return false;
        }
        initialized = true;
        return true;
    }
    std::string getDateString()
    {
        if (!initialized) {
            return "";
        }
        return std::string(
            std::to_string(year).insert(0, 4-((int)log10((double)year)+1), '0') + '-' +
            std::to_string(month).insert(0, 2-((int)log10((double)month)+1), '0') + '-' +
            std::to_string(day).insert(0, 2-((int)log10((double)day)+1), '0') + ' ' +
            std::to_string(hour).insert(0, 2-((int)log10((double)hour)+1), '0') + ':' +
            std::to_string(min).insert(0, 2-((int)log10((double)min)+1), '0')
        );
    }
};

struct Lecture {
    std::string teacher_name;
    std::string group_name;
    std::string thematic;
    std::string cabinet;
    DBDate date;
};

struct Practice {
    std::string teacher_name;
    std::string student_name;
    std::string thematic;
    std::string car;
    DBDate date;
};



#endif // APPDATABASE_STRUCTS_H
