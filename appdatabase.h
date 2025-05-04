#ifndef APPDATABASE_H
#define APPDATABASE_H

#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include <string>

struct Question
{
    uint ticket_num;
    uint question_num;


    std::string image_base64;
    std::string question_text;
    std::string answers;
    uint rightAnswer;
    std::string comment;
};

class AppDatabase
{
    AppDatabase(std::string name);
    ~AppDatabase();

    bool initSQLite(std::string name);
    bool createTables();
    void countTickets();

public:
    static AppDatabase *getInstance(std::string databaseName = "data.db3");
    AppDatabase(const AppDatabase &) = delete;
    AppDatabase operator=(const AppDatabase &) = delete;

    int getTicketsAmount();

    bool addQuestion(Question &);
    bool modifyQuestion(Question &);
    bool deleteQuestion(int ticketNum, int questionNum);
    std::shared_ptr<Question> getQuestion(int ticketNum, int questionNum);


private:
    SQLite::Database *db;

    int ticketsAmount = 0;
};

#endif // APPDATABASE_H
