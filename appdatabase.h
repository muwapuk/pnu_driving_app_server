#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include <string>
#include <list>
#include <vector>

#include "appdatabase_structs.h"

using std::vector;
using std::pair;
using std::string;

// Singleton
class AppDatabase
{
    const string DATABASE_NAME = "data.db3";

    AppDatabase();
    ~AppDatabase();

    // If not exist
    bool createTables();
    bool createTriggers();

    bool decreaseQuestionId(int id);
public:
    // Singleton preparation
    static AppDatabase &getInstance();
    AppDatabase(const AppDatabase &) = delete;
    AppDatabase operator=(const AppDatabase &) = delete;

//  ACCESSORS

// Ticket table accessors
    bool insertTicket(tickets::Categories category, int num);
    bool deleteTicket(int id);
    std::shared_ptr<vector<int>> getTicketsIdByCategory(tickets::Categories category);

// Question table accessors
    std::shared_ptr<vector<string>> getQuestionsSubjects();
    std::shared_ptr<vector<int>> getQuestionsIdByTicket(int ticketId);
    std::shared_ptr<vector<int>> getQuestionsIdBySubject(string subject);

    bool insertQuestion(Question &);
    bool changeQuestion(int id, Question &);
    bool deleteQuestion(int id);
    std::shared_ptr<Question> getQuestion(int id);
    int getRightAnswer(int id);

// Groups table accessors
    bool insertGroup(string groupName);
    bool deleteGroup(int id);
    std::shared_ptr<string> getGroupName(int id);
    int getGroupId(string groupName);

// Users table accessors
    bool insertUser(User &);
    bool deleteUser(string login);
    bool changeUserName(string login, string newName);
    bool changeUserPassword(string login, string newPass);
    bool changeUserPermissions(string login, User::Permissions newPerm);

    // Make student
    bool setUserStudent(string login, int groupId, string assignedTeacherLogin);
    bool changeStudentGroup(string studentLogin, int groupId);
    bool assignStudentTeacher(string studentLogin, string teacherLogin);
    // Make teacher
    bool setUserTeacher(string login, string car);

    // Getters
    std::shared_ptr<User> getUser(string login);
    std::shared_ptr<vector<User>> getUsersList(int startRow, int amount);
    std::shared_ptr<vector<Student>> getStudentsList(int startRow, int amount);
    std::shared_ptr<vector<Student>> getStudentsListByGroup(int groupId, int startRow, int amount);
    std::shared_ptr<vector<Teacher>> getTeachersList(int startRow, int amount);
    bool isUserExist(string login);

// Tokens table accessors
    bool addToken(string token, string login);
    bool deleteToken(string token);
    bool deleteTokenByUser(string login);
    bool deleteTokensByTime(int time);
    std::shared_ptr<pair<string, User::Permissions>> getLoginAndPermissionsByToken(string token);

// Lectures table accessors
    bool insertLecture(Lecture &);


    bool addLecture(Lecture &);
    bool deleteLecture(string teacherLogin, int date);
    vector<Lecture> *getLectures();
    vector<Lecture> *getLecturesByGroup(string group);
    vector<Lecture> *getLecturesByTeacher(string teacherLogin);

    // Students_to_instructors table accessors
    bool addStudentsToInstructorsLink(string studentLogin, string teacherLogin);
    bool deleteStudentsToInstructorsLink(string studentLogin, string teacherLogin);
    std::string *getStudentsInstructorLogin(string studentLogin);
    vector<std::string> *getInstructorsStudentsLogins(string teacherLogin);

    // Practices table accessors
    bool addPractice(Practice &);
    bool changePractice(Practice &);
    bool deletePractice(string teacherLogin, int date);
    Practice *getPractice(string teacherLogin, int date);
    vector<Practice> *getPractices();
    vector<Practice> *getPracticesByStudent(string studentLogin);
    vector<Practice> *getPracticesByUserInstructor(string studentLogin);
    vector<Practice> *getPracticesByTeacher(string teacherLogin);
    vector<Practice> *getPracticesByTime(int fromTime, int toTime);


private:
    SQLite::Database db = SQLite::Database(DATABASE_NAME, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    uint ticketsABamount = 0;
    uint ticketsCDamount = 0;
};

inline AppDatabase &AppDB(){
    return AppDatabase::getInstance();
}

#endif // APPDATABASE_H
