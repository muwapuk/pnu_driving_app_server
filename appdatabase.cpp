#include "appdatabase.h"

#include <iostream>


AppDatabase::AppDatabase(std::string name)
{
    initSQLite(name);
    createTables();
    countTickets();
}

int AppDatabase::getTicketsABamount()
{
    return ticketsABamount;
}

int AppDatabase::getTicketsCDamount()
{
    return ticketsCDamount;
}

AppDatabase::~AppDatabase()
{

}



AppDatabase *AppDatabase::getInstance(std::string databaseName) {
    static AppDatabase instance(databaseName);
    return &instance;
}

bool AppDatabase::initSQLite(std::string name)
{
    try {
        db = new SQLite::Database(name, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    } catch(std::exception &e) {
        std::cerr << "exeption " << e.what() << std::endl;
        return false;
    }
    return true;
}

bool AppDatabase::createTables()
{
    try {
        db->exec(
            "CREATE TABLE IF NOT EXISTS questions ("
            "id INTEGER PRIMARY KEY,"
            "category INTEGER,"
            "ticket_num INTEGER,"
            "question_num INTEGER,"
            "image TEXT,"
            "question TEXT,"
            "answers TEXT,"
            "comment TEXT,"
            "right_answer INTEGER);"
        );

        db->exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "login TEXT PRIMARY KEY,"
            "password TEXT,"
            "name TEXT,"
            "permissions INTEGER);"
        );

        db->exec(
            "CREATE TABLE IF NOT EXISTS users_errors ("
            "question_id INTEGER,"
            "user_id INTEGER,"
            "answer INTEGER,"
            "FOREIGN KEY(question_id) REFERENCES questions(id),"
            "FOREIGN KEY(user_id) REFERENCES users(id));"
        );


        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

void AppDatabase::countTickets()
{
    try {
        SQLite::Statement queryAB(*db, "SELECT MAX(ticket_num) FROM questions WHERE category = " + std::to_string(Question::Category::AB));

        queryAB.executeStep();
        ticketsABamount = queryAB.getColumn(0).getInt();
        queryAB.reset();

        SQLite::Statement queryCD(*db, "SELECT MAX(ticket_num) FROM questions WHERE category = " + std::to_string(Question::Category::CD));

        queryCD.executeStep();
        ticketsABamount = queryCD.getColumn(0).getInt();
        queryCD.reset();

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
    }
}


bool AppDatabase::addQuestion(Question &question)
{
    try {
        if (question.question_num >= 100 or
            question.question_num < 1 or
            question.ticket_num < 1 or
            question.category < 0
        ) return false;

        uint id = (question.ticket_num + question.question_num*100)*10 + question.category;

        // Prepare query
        SQLite::Statement query {*db, "INSERT OR IGNORE INTO questions VALUES (?,?,?,?,?,?,?,?,?)"};

        query.bind(1, id);
        query.bind(2, question.category);
        query.bind(3, question.ticket_num);
        query.bind(4, question.question_num);
        query.bind(5, question.image_base64);
        query.bind(6, question.question_text);
        query.bind(7, question.answers);
        query.bind(8, question.comment);
        query.bind(9, question.rightAnswer);

        query.exec();

        // query.reset();


        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::modifyQuestion(Question &question)
{
    try {
        if (question.question_num >= 100 or
            question.question_num < 1 or
            question.ticket_num < 1 or
            question.category < 0
        ) return false;

        uint id = (question.ticket_num + question.question_num*100)*10 + question.category;

        // Prepare query
        SQLite::Statement query {*db, "UPDATE questions SET "
                                      "category = :category,"
                                      "ticket_num = :ticket_num,"
                                      "question_num = :question_num,"
                                      "image = :image,"
                                      "question = :question,"
                                      "answers = :answers,"
                                      "comment = :comment,"
                                      "right_answer = :right_answer "
                                      "WHERE id = " + std::to_string(id)};

        query.bind(":category", question.category);
        query.bind(":ticket_num", question.ticket_num);
        query.bind(":question_num", question.question_num);
        query.bind(":image", question.image_base64);
        query.bind(":question", question.question_text);
        query.bind(":answers", question.answers);
        query.bind(":comment", question.comment);
        query.bind(":right_answer", question.rightAnswer);

        query.exec();

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteQuestion(Question::Category category, int ticketNum, int questionNum)
{
    try {
        if (questionNum >= 100 or
            questionNum < 1 or
            ticketNum < 1 or
            category < 0
        ) return false;

        uint id = (ticketNum + questionNum*100)*10 + category;

        SQLite::Statement query {*db, std::string("DELETE FROM questions WHERE id = " + std::to_string(id))};

        query.exec();

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<Question> AppDatabase::getQuestion(Question::Category category, int ticketNum, int questionNum)
{
    try {
        if (questionNum >= 100 or
            questionNum < 1 or
            ticketNum < 1 or
            category < 0
        ) return nullptr;

        std::shared_ptr<Question> requestedQuestion {new Question};

        uint id = (ticketNum + questionNum*100)*10 + category;
            requestedQuestion->question_num = id;

        SQLite::Statement query {*db, std::string("SELECT * FROM questions WHERE id = " + std::to_string(id))};

        if (!query.hasRow()) {
            return nullptr;
        }
        while (query.executeStep())
        {
                requestedQuestion->ticket_num = query.getColumn(1).getInt();
                requestedQuestion->question_num = query.getColumn(2).getInt();
                requestedQuestion->image_base64 = query.getColumn(3).getString();
                requestedQuestion->question_text = query.getColumn(4).getString();
                requestedQuestion->answers = query.getColumn(5).getString();
                requestedQuestion->comment = query.getColumn(6).getString();
                requestedQuestion->rightAnswer = query.getColumn(7).getInt();
        }

        return  requestedQuestion;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}
