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
public:
    // Singleton preparation
    static AppDatabase &getInstance();
    AppDatabase(const AppDatabase &) = delete;
    AppDatabase operator=(const AppDatabase &) = delete;

//  ACCESSORS

// Ticket table accessors
    int insertTicket(tickets::Categories category, int num);
    bool deleteTicket(int id);
    shared_ptr<vector<pair<int, int>>> getTicketNumIdPairsByCategory(tickets::Categories category);

// Question table accessors
    int insertQuestion(Question &);
    bool changeQuestion(int id, Question &);
    bool deleteQuestion(int id);

    shared_ptr<Question> getQuestion(int id);
    int getRightAnswer(int id);
    shared_ptr<vector<string>> getQuestionsSubjects();
    shared_ptr<vector<pair<int, int>>> getQuestionNumIdPairsByTicket(int ticketId);
    shared_ptr<vector<int>> getQuestionIdVecBySubject(string subject);

// Groups table accessors
    bool insertGroup(string groupName);
    bool deleteGroup(int id);
    int getGroupIdByStudent(int studentId);
    shared_ptr<string> getGroupName(int id);
    int getGroupId(string groupName);
    shared_ptr<vector<pair<int, string>>> getGroups();

// Users table accessors
    bool insertUser(User &);
    bool deleteUser(int id);
    bool changeUserName(int id, string newName);
    bool changeUserPassword(int id, string newPass);
    bool changeUserPermissions(int id, User::Permissions newPerm);

    // Make student
    bool setUserStudent(int id, int groupId, int assignedTeacherId);
    bool changeStudentGroup(int studentId, int groupId);
    bool assignStudentToTeacher(int studentId, int teacherId);

    // Make teacher
    bool setUserTeacher(int id, string car);

    // Getters
    shared_ptr<User> getUser(int id);
    int getUserIdByLogin(string login);
    shared_ptr<string> getUserName(int id);
    shared_ptr<string> getUserPassword(int id);
    User::Permissions getUserPermissions(int id);
    shared_ptr<vector<User>> getUsersList(int startRow, int amount);
    shared_ptr<vector<Student>> getStudentsList(int startRow, int amount);
    shared_ptr<vector<Student>> getStudentsListByGroup(int groupId, int startRow, int amount);
    shared_ptr<vector<Teacher>> getTeachersList(int startRow, int amount);
    bool isUserExist(int id);

// Tokens table accessors
    bool addToken(string token, int user_id);
    bool deleteToken(string token);
    bool deleteTokenByUser(int userId);
    bool deleteTokensByTime(int time);
    shared_ptr<pair<int, User::Permissions>> getUserIdAndPermissionsByToken(string token);

// Lectures table accessors
    bool insertLecture(Lecture &);
    bool deleteLecture(int id);
    shared_ptr<Lecture> getLecture(int id);
    shared_ptr<vector<Lecture>> getLecturesByTeacher(int teacherId);
    shared_ptr<vector<Lecture>> getLecturesByGroup(int groupId);

// Practices tables accessors

    // Practice slots
    bool insertPracticeSlot(PracticeSlot &);
    bool deletePracticeSlot(int id);

    // Practice bookings
    bool insertPracticeBooking(PracticeBooking &);
    bool deletePracticeBooking(int id);

    // Practices getters
    shared_ptr<PracticeSlot> getPracticeSlot(int id);
    shared_ptr<PracticeBooking> getPracticeBooking(int id);
    shared_ptr<vector<FreePracticeSlot>> getFreePracticeSlotsForStudent(int studentId);
    shared_ptr<vector<BookedPracticeSlot>> getBookedPracticeSlotsForStudent(int studentId);
    shared_ptr<vector<FreePracticeSlot>> getFreePracticeSlotsByTeacher(int teacherId);
    shared_ptr<vector<BookedPracticeSlot>> getBookedPracticeSlotsByTeacher(int teacherId);

private:
    SQLite::Database db = SQLite::Database(DATABASE_NAME, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
};

inline AppDatabase &AppDB(){
    return AppDatabase::getInstance();
}

#endif // APPDATABASE_H
