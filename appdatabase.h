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
    AppDatabase(string name);
    ~AppDatabase();

    // Opens the database
    bool initSQLite(string name);
    bool createTables(); // If not exist
    // Counts tickets amount from different categories and moves result into specific variables
    void countTickets();

    bool decreaseQuestionId(int id);
public:
    // Singleton preparation
    static AppDatabase *getInstance(string databaseName = "data.db3");
    AppDatabase(const AppDatabase &) = delete;
    AppDatabase operator=(const AppDatabase &) = delete;

    // Returns tickets amount from variables
    int getTicketsABamount();
    int getTicketsCDamount();

    // Question table accessors
    bool addQuestion(Question &);
    bool modifyQuestion(Question &);
    bool deleteQuestion(Question::Category, int ticketNum, int questionNum);
    int getTicketQuestionsAmount(Question::Category, int ticketNum);
    int getThemeQuestionsAmount(string theme);
    vector<string> *getQuestionsThemes(); //////////////////////////////
    vector<int> *getQuestionsIdsByTheme(string theme);
    Question *getQuestion(Question::Category, int ticketNum, int questionNum);
    std::list<Question> *getTicketQuestions(Question::Category, int ticketNum);

    // User table accessors
    bool addUser(User &);
    bool deleteUser(string login);
    bool changeUserPassword(string login, string newPassword);
    User *getUser(string login);
    string *getUserName(string login);
    string *getUserPassword(string login);
    User::Permissions getUserPermissions(string login);
    bool isUserExist(string login);

    vector<User> *getUsers(int startIndx, int amount);
    vector<pair<string, string>> *getUsersName(int startIndx, int amount);
    vector<pair<string, string>> *getUsersPassword(int startIndx, int amount);
    vector<pair<string, User::Permissions>> *getUsersPermissions(int startIndx, int amount);

    // User errors table accessors
    bool addError(string userLogin,
                  Question::Category category,
                  int ticketNum,
                  int questionNum,
                  int answer);
    bool deleteError(string userLogin,
                     Question::Category category,
                     int ticketNum,
                     int questionNum);

    // Tokens table accessors
    bool addToken(string token, string login);
    bool deleteToken(string token);
    bool deleteTokenByUser(string login);
    bool deleteTokensByTime(int time);
    std::shared_ptr<pair<string, User::Permissions>> getLoginAndPermissionsByToken(string token);

    // Groups and students_to_group tables accessors
    bool addGroup(string groupName);
    bool deleteGroup(string groupName);
    bool addStudentToGroup(string studentLogin, string groupName);
    bool deleteStudentFromGroup(string studentLogin, string groupName);
    vector<string> *getGroupStudentsLogins(string groupName);
    vector<string> *getGroupStudentsNames(string groupName);

    // Lectures table accessors
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
    SQLite::Database *db;

    uint ticketsABamount = 0;
    uint ticketsCDamount = 0;
};

inline AppDatabase *AppDB(){
    return AppDatabase::getInstance();
}

#endif // APPDATABASE_H
