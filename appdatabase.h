#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include <string>
#include <vector>
#include <list>

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

// Singleton
class AppDatabase
{
    AppDatabase(std::string name);
    ~AppDatabase();

    // Opens the database
    bool initSQLite(std::string name);
    bool createTables(); // If not exist
    // Counts tickets amount from different categories and moves result into specific variables
    void countTickets();

public:
    // Singleton preparation
    static AppDatabase *getInstance(std::string databaseName = "data.db3");
    AppDatabase(const AppDatabase &) = delete;
    AppDatabase operator=(const AppDatabase &) = delete;

    // Returns tickets amount from variables
    int getTicketsABamount();
    int getTicketsCDamount();

    // Question table accessors
    bool addQuestion(Question &);
    bool modifyQuestion(Question &);
    bool deleteQuestion(Question::Category, int ticketNum, int questionNum);
    Question *getQuestion(Question::Category, int ticketNum, int questionNum);
    std::list<Question> *getTicketQuestions(Question::Category, int ticketNum);

    // User table accessors
    bool addUser(User &);
    bool deleteUser(std::string login);
    bool changeUserPassword(std::string login, std::string newPassword);
    User *getUser(std::string login);
    std::string *getUserPassword(std::string login);
    std::string *getUserName(std::string login);
    User::Permissions *getUserPermissions(std::string login);

    /*  NOT IMPLEMENTED */

    // User errors table accessors
    bool addError(std::string userLogin,
                  Question::Category category,
                  int ticketNum,
                  int questionNum,
                  int answer);z
    bool deleteError(std::string userLogin,
                     Question::Category category,
                     int ticketNum,
                     int questionNum);


private:
    SQLite::Database *db;

    uint ticketsABamount = 0;
    uint ticketsCDamount = 0;
};

#endif // APPDATABASE_H
