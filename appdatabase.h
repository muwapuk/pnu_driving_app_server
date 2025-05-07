#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include <string>

struct Question
{
    enum Category {
        AB,
        CD
    } category;

    uint ticket_num;
    uint question_num;


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
    } permissions;
};

// Singleton
class AppDatabase
{
    AppDatabase(std::string name);
    ~AppDatabase();

    // Opens the database
    bool initSQLite(std::string name);
    bool createTables();
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
    std::shared_ptr<Question> getQuestion(Question::Category, int ticketNum, int questionNum);

/*  NOT IMPLEMENTED */

    // User table accessors
    bool addUser(User &);
    bool deleteUser(std::string login, std::string password);
    bool changeUserPassword(std::string login, std::string oldPassword, std::string newPassword);
    bool getUserPermissions(std::string login);

    // User errors table accessors
    bool addError(std::string userLogin, Question::Category category, int ticketNum, int questionNum, int answer);
    bool removeError(std::string userLogin, Question::Category category, int ticketNum, int questionNum);

private:
    SQLite::Database *db;

    int ticketsABamount = 0;
    int ticketsCDamount = 0;
};

#endif // APPDATABASE_H
