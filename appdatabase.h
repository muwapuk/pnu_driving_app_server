#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include <string>
#include <vector>

#include "appdatabase_structs.h"

using std::vector;
using std::pair;
using std::string;
using std::shared_ptr;

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
    shared_ptr<vector<int>> getTicketsIdByCategory(tickets::Categories category);

// Question table accessors
    shared_ptr<vector<string>> getQuestionsSubjects();
    shared_ptr<vector<int>> getQuestionsIdByTicket(int ticketId);
    shared_ptr<vector<int>> getQuestionsIdBySubject(string subject);

    bool insertQuestion(Question &);
    bool changeQuestion(int id, Question &);
    bool *deleteQuestion(int id);
    shared_ptr<Question> getQuestion(int id);
    int getRightAnswer(int id);

// Groups table accessors
    bool insertGroup(string groupName);
    bool deleteGroup(int id);
    int getGroupIdByStudent(string login);
    shared_ptr<string> getGroupName(int id);
    int getGroupId(string groupName);
    shared_ptr<vector<pair<int, string>>> getGroups();

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
    shared_ptr<User> getUser(string login);
    shared_ptr<vector<User>> getUsersList(int startRow, int amount);
    shared_ptr<vector<Student>> getStudentsList(int startRow, int amount);
    shared_ptr<vector<Student>> getStudentsListByGroup(int groupId, int startRow, int amount);
    shared_ptr<vector<Teacher>> getTeachersList(int startRow, int amount);
    bool isUserExist(string login);

// Tokens table accessors
    bool addToken(string token, string login);
    bool deleteToken(string token);
    bool deleteTokenByUser(string login);
    bool deleteTokensByTime(int time);
    shared_ptr<pair<string, User::Permissions>> getLoginAndPermissionsByToken(string token);

// Lectures table accessors
    bool insertLecture(Lecture &);
    bool deleteLecture(int id);
    shared_ptr<vector<Lecture>> getLecturesIdByTeacher(string teacherLogin);
    shared_ptr<vector<Lecture>> getLecturesIdByGroup(int groupId);

// Practices tables accessors

    // Practice slots
    bool insertPracticeSlot(PracticeSlot &);
    bool deletePracticeSlot(int id);

    // Practice bookings
    bool insertPracticeBooking(PracticeBooking &);
    bool deletePracticeBooking(int id);

    // Practices getters
    shared_ptr<vector<FreePracticeSlot>> getFreePracticeSlotsForStudent(string studentLogin);
    shared_ptr<vector<BookedPracticeSlot>> getBookedPracticeSlotsForStudent(string studentLogin);
    shared_ptr<vector<FreePracticeSlot>> getFreePracticeSlotsByTeacher(string teacherLogin);
    shared_ptr<vector<BookedPracticeSlot>> getBookedPracticeSlotsByTeacher(string teacherLogin);

private:
    SQLite::Database db = SQLite::Database(DATABASE_NAME, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

    uint ticketsABamount = 0;
    uint ticketsCDamount = 0;
};

inline AppDatabase &AppDB(){
    return AppDatabase::getInstance();
}

#endif // APPDATABASE_H
