#include "appdatabase.h"

#include <iostream>


AppDatabase::AppDatabase(string name)
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

AppDatabase *AppDatabase::getInstance(string databaseName) {
    static AppDatabase instance(databaseName);
    return &instance;
}

bool AppDatabase::initSQLite(string name)
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
            "question_id INTEGER NOT NULL,"
            "user_login INTEGER NOT NULL,"
            "answer INTEGER,"
            "FOREIGN KEY(question_id) REFERENCES questions(id),"
            "FOREIGN KEY(user_login) REFERENCES users(login),"
            "UNIQUE(question_id, user_login));"
        );

        db->exec(
            "CREATE TABLE IF NOT EXISTS tokens ("
            "token TEXT PRIMARY KEY,"
            "login TEXT,"
            "permissions INTEGER,"
            "time INTEGER);"
        );

        db->exec(
            "CREATE TABLE IF NOT EXISTS groups ("
            "name TEXT PRIMARY KEY);"
        );

        db->exec(
            "CREATE TABLE IF NOT EXISTS students_to_group ("
            "student_login TEXT,"
            "group_name TEXT,"
            "FOREIGN KEY(student_login) REFERENCES users(login),"
            "FOREIGN KEY(group_name) REFERENCES groups(name),"
            "UNIQUE(student_login, group_name));"
        );

        db->exec(
            "CREATE TABLE IF NOT EXISTS lectures ("
            "teacher_name TEXT,"
            "group_name TEXT,"
            "thematic TEXT,"
            "cabinet TEXT,"
            "date TEXT,"
            "FOREIGN KEY(teacher_name) REFERENCES users(name),"
            "FOREIGN KEY(group_name) REFERENCES groups(name)"
            "UNIQUE(teacher_name, date));"
        );

        db->exec(
            "CREATE TABLE IF NOT EXISTS practices ("
            "teacher_name TEXT,"
            "student_name TEXT,"
            "thematic TEXT,"
            "car TEXT,"
            "date TEXT,"
            "FOREIGN KEY(teacher_name) REFERENCES users(name),"
            "FOREIGN KEY(student_name) REFERENCES users(name)"
            "UNIQUE(teacher_name, date));"
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
        ticketsCDamount = queryCD.getColumn(0).getInt();
        queryCD.reset();

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
    }
}


bool AppDatabase::addQuestion(Question &question)
{
    try {
        if (question.ticket_num >= Question::MAX_TICKET_NUM or
            question.ticket_num < Question::MIN_TICKET_NUM or
            question.question_num < Question::MIN_QUESTION_NUM or
            question.category <= Question::UNDEFINED or
            question.category > Question::TOTAL_CATEGORIES
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

        // If no changes
        if(!query.exec())
            return false;

        // Change chosen category tickets amount if inserted question ticket num greater then old amount
        uint &ticketsAmount = (question.category == Question::AB) ? ticketsABamount : ticketsCDamount;
        if (question.ticket_num > ticketsAmount)
            ticketsAmount = question.ticket_num;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::modifyQuestion(Question &question)
{
    try {
        if (question.ticket_num >= Question::MAX_TICKET_NUM or
            question.ticket_num < Question::MIN_TICKET_NUM or
            question.question_num < Question::MIN_QUESTION_NUM or
            question.category <= Question::UNDEFINED or
            question.category > Question::TOTAL_CATEGORIES
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

        // If no changes
        if(!query.exec())
            return false;

        // Change chosen category tickets amount if updated question ticket num greater then old amount
        uint &ticketsAmount = (question.category == Question::AB) ? ticketsABamount : ticketsCDamount;
        if (question.ticket_num > ticketsAmount)
            ticketsAmount = question.ticket_num;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteQuestion(Question::Category category, int ticketNum, int questionNum)
{
    try {
        if (ticketNum >= Question::MAX_TICKET_NUM or
            ticketNum < Question::MIN_TICKET_NUM or
            questionNum < Question::MIN_QUESTION_NUM or
            category <= Question::UNDEFINED or
            category > Question::TOTAL_CATEGORIES
        ) return false;

        uint id = (ticketNum + questionNum*100)*10 + category;

        SQLite::Statement query {*db, string("DELETE FROM questions WHERE id = " + std::to_string(id))};

        // If no changes
        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

Question *AppDatabase::getQuestion(Question::Category category, int ticketNum, int questionNum)
{
    try {
        Question *requestedQuestion = nullptr;

        if (ticketNum >= Question::MAX_TICKET_NUM or
            ticketNum < Question::MIN_TICKET_NUM or
            questionNum < Question::MIN_QUESTION_NUM or
            category <= Question::UNDEFINED or
            category > Question::TOTAL_CATEGORIES
        ) return requestedQuestion;
        uint id = (ticketNum + questionNum*100)*10 + category;

        SQLite::Statement query {*db, string("SELECT * FROM questions WHERE id = " + std::to_string(id))};

        if (!query.executeStep()) {
            return requestedQuestion;
        }

        requestedQuestion = new Question;

        requestedQuestion->category = static_cast<Question::Category>(query.getColumn(1).getInt());
        requestedQuestion->ticket_num = query.getColumn(2).getInt();
        requestedQuestion->question_num = query.getColumn(3).getInt();
        requestedQuestion->image_base64 = query.getColumn(4).getString();
        requestedQuestion->question_text = query.getColumn(5).getString();
        requestedQuestion->answers = query.getColumn(6).getString();
        requestedQuestion->comment = query.getColumn(7).getString();
        requestedQuestion->rightAnswer = query.getColumn(8).getInt();

        return requestedQuestion;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

std::list<Question> *AppDatabase::getTicketQuestions(Question::Category category, int ticketNum)
{
    try {
        std::list<Question> *ticket = nullptr;

        if (ticketNum >= Question::MAX_TICKET_NUM or
            ticketNum < Question::MIN_TICKET_NUM or
            category <= Question::UNDEFINED or
            category > Question::TOTAL_CATEGORIES
        ) return ticket;


        SQLite::Statement query {*db, "SELECT * FROM questions "
                                      "WHERE category = :category "
                                      " AND ticket_num = :ticket_num"};
        query.bind(":category", category);
        query.bind(":ticket_num", ticketNum);

        ticket = new std::list<Question> {};

        while (query.executeStep())
        {
            Question requestedQuestion;

            requestedQuestion.category = static_cast<Question::Category>(query.getColumn(1).getInt());
            requestedQuestion.ticket_num = query.getColumn(2).getInt();
            requestedQuestion.question_num = query.getColumn(3).getInt();
            requestedQuestion.image_base64 = query.getColumn(4).getString();
            requestedQuestion.question_text = query.getColumn(5).getString();
            requestedQuestion.answers = query.getColumn(6).getString();
            requestedQuestion.comment = query.getColumn(7).getString();
            requestedQuestion.rightAnswer = query.getColumn(8).getInt();

            ticket->push_back(requestedQuestion);
        }
        if (ticket->empty()) {
            delete ticket;
            return nullptr;
        }

        return ticket;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return {};
    }
}

bool AppDatabase::addUser(User &user)
{
    try {
        SQLite::Statement query {*db, "INSERT OR IGNORE INTO users VALUES (?,?,?,?)"};
        query.bind(1, user.login);
        query.bind(2, user.password);
        query.bind(3, user.name);
        query.bind(4, user.permissions);

        // If no changes
        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteUser(string login)
{
    try {
        SQLite::Statement query {*db, string("DELETE FROM users WHERE login = :login")};
        query.bind(":login", login);

        // If no changes
        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::changeUserPassword(string login, string newPassword)
{
    try {
        SQLite::Statement query {*db, "UPDATE users SET "
                                      "password = :password "
                                      "WHERE login = :login"};
        query.bind(":password", newPassword);
        query.bind(":login", login);

        // If no changes
        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

User *AppDatabase::getUser(string login)
{
    try {
        SQLite::Statement query {*db, "SELECT * FROM users "
                                     "WHERE login = :login"};
        query.bind(":login", login);

        User *user = nullptr;
        if (!query.executeStep())
            return user;

        user = new User;
        user->login = query.getColumn(0).getString();
        user->password = query.getColumn(1).getString();
        user->name = query.getColumn(2).getString();
        user->permissions = static_cast<User::Permissions>(query.getColumn(3).getInt());

        return user;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

string *AppDatabase::getUserPassword(string login)
{
    try {
        SQLite::Statement query {*db, "SELECT password FROM users "
                                      "WHERE login = :login"};
        query.bind(":login", login);

        string *password = nullptr;
        if (!query.executeStep())
            return password;

        password = new string;
        *password = query.getColumn(0).getString();

        return password;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

string *AppDatabase::getUserName(string login)
{
    try {
        SQLite::Statement query {*db, "SELECT name FROM users "
                                     "WHERE login = :login"};
        query.bind(":login", login);

        string *name = nullptr;
        if (!query.executeStep())
            return name;

        name = new string;
        *name = query.getColumn(0).getString();

        return name;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

User::Permissions AppDatabase::getUserPermissions(string login)
{
    try {
        SQLite::Statement query {*db, "SELECT permissions FROM users "
                                     "WHERE login = :login"};
        query.bind(":login", login);


        if (!query.executeStep())
            return User::NONE;
        User::Permissions perm = static_cast<User::Permissions>(query.getColumn(0).getInt());

        return perm;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return User::NONE;
    }
}

vector<User> *AppDatabase::getUsers(int startIndx, int amount)
{
    try {
        vector<User> *users = nullptr;

        SQLite::Statement query {*db, "SELECT * FROM questions "
                                      " WHERE rowid >= :startIndx "
                                      " AND rowid < :amount"};
        query.bind(":startIndx", startIndx);
        query.bind(":amount", startIndx+amount);

        users = new vector<User> {};

        while (query.executeStep())
        {
            User requestedUser;

            requestedUser.login = query.getColumn(0).getString();
            requestedUser.password = query.getColumn(1).getString();
            requestedUser.name = query.getColumn(2).getString();
            requestedUser.permissions = static_cast<User::Permissions>(query.getColumn(3).getInt());

            users->push_back(requestedUser);
        }
        if (users->empty()) {
            delete users;
            return nullptr;
        }

        return users;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return {};
    }
}

vector<pair<string, string>> *AppDatabase::getUsersName(int startIndx, int amount)
{
    try {
        vector<pair<string, string>> *users = nullptr;

        SQLite::Statement query {*db, "SELECT login, name FROM questions "
                                     " WHERE rowid >= :startIndx "
                                     " AND rowid < :amount"};
        query.bind(":startIndx", startIndx);
        query.bind(":amount", startIndx+amount);

        users = new vector<pair<string, string>> {};

        while (query.executeStep())
        {
            users->push_back({
                query.getColumn(0).getString(),
                query.getColumn(1).getString()
            });
        }
        if (users->empty()) {
            delete users;
            return nullptr;
        }

        return users;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return {};
    }
}

vector<pair<string, string>> *AppDatabase::getUsersPassword(int startIndx, int amount)
{
    try {
        vector<pair<string, string>> *users = nullptr;

        SQLite::Statement query {*db, "SELECT login, password FROM questions "
                                     " WHERE rowid >= :startIndx "
                                     " AND rowid < :amount"};
        query.bind(":startIndx", startIndx);
        query.bind(":amount", startIndx+amount);

        users = new vector<pair<string, string>> {};

        while (query.executeStep())
        {
            users->push_back({
                query.getColumn(0).getString(),
                query.getColumn(1).getString()
            });
        }
        if (users->empty()) {
            delete users;
            return nullptr;
        }

        return users;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return {};
    }
}

vector<pair<string, User::Permissions>> *AppDatabase::getUsersPermissions(int startIndx, int amount)
{
    try {
        vector<pair<string, User::Permissions>> *users = nullptr;

        SQLite::Statement query {*db, "SELECT login, permissions FROM questions "
                                     " WHERE rowid >= :startIndx "
                                     " AND rowid < :amount"};
        query.bind(":startIndx", startIndx);
        query.bind(":amount", startIndx+amount);

        users = new vector<pair<string, User::Permissions>> {};

        while (query.executeStep())
        {
            users->push_back({
                query.getColumn(0).getString(),
                static_cast<User::Permissions>(query.getColumn(1).getInt())
            });
        }
        if (users->empty()) {
            delete users;
            return nullptr;
        }

        return users;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return {};
    }
}

bool AppDatabase::addError(string userLogin,
                           Question::Category category,
                           int ticketNum,
                           int questionNum,
                           int answer)
{
    try {
        if (ticketNum >= Question::MAX_TICKET_NUM or
            ticketNum < Question::MIN_TICKET_NUM or
            questionNum < Question::MIN_QUESTION_NUM or
            category <= Question::UNDEFINED or
            category > Question::TOTAL_CATEGORIES
        ) return false;

        uint question_id = (ticketNum + questionNum*100)*10 + category;

        // SQLite::Statement query {*db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {*db, "INSERT OR REPLACE INTO users_errors VALUES("
                                     "(SELECT id FROM questions WHERE id = :question_id),"
                                     "(SELECT login FROM users WHERE login = :user_login),"
                                     ":answer)"};
        query.bind(":question_id", question_id);
        query.bind(":user_login", userLogin);
        query.bind(":answer", answer);


        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteError(string userLogin,
                              Question::Category category,
                              int ticketNum,
                              int questionNum)
{
    try {
        if (ticketNum >= Question::MAX_TICKET_NUM or
            ticketNum < Question::MIN_TICKET_NUM or
            questionNum < Question::MIN_QUESTION_NUM or
            category <= Question::UNDEFINED or
            category > Question::TOTAL_CATEGORIES
        ) return false;

        uint question_id = (ticketNum + questionNum*100)*10 + category;

        SQLite::Statement query {*db, string("DELETE FROM users_errors "
                                                  "WHERE question_id = :question_id "
                                                  "AND user_login = :user_login")};
        query.bind(":user_login", userLogin);
        query.bind(":question_id", question_id);

        // If no changes
        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

//"CREATE TABLE IF NOT EXISTS tokens ("
 //   "token TEXT PRIMARY KEY,"
 //   "permissions INTEGER,"
  //  "time INTEGER);"

bool AppDatabase::addToken(string token, string login, User::Permissions perm)
{
    try {
        // SQLite::Statement query {*db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {*db, "INSERT OR REPLACE INTO tokens VALUES("
                                     "token = :token,"
                                     "permissions = :permissions,"
                                     "login = :login"
                                     "time = :time)"};
        query.bind(":token", token);
        query.bind(":login", login);
        query.bind(":permissions", perm);
        query.bind(":time", time(0));

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteToken(string token)
{
    try {
        SQLite::Statement query {*db, string("DELETE FROM tokens "
                                            "WHERE token = :token")};
        query.bind(":token", token);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteTokensByTime(int time)
{
    try {
        SQLite::Statement query {*db, string("DELETE FROM tokens "
                                            "WHERE time < :time")};
        query.bind(":time", time);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

User::Permissions  AppDatabase::getPermissionsByToken(string token)
{
    try {
        SQLite::Statement query {*db, "SELECT permissions FROM tokens "
                                     "WHERE token = :token"};
        query.bind(":token", token);

        if (!query.executeStep())
            return User::NONE;

        User::Permissions perm = static_cast<User::Permissions>(query.getColumn(0).getInt());

        return perm;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return User::NONE;
    }
}

bool AppDatabase::addGroup(string groupName)
{
    try {
        // SQLite::Statement query {*db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {*db, "INSERT OR IGNORE INTO groups VALUES("
                                     "name = :name)"};
        query.bind(":name", groupName);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteGroup(string groupName)
{
    try {
        SQLite::Statement query {*db, string("DELETE FROM group "
                                                 "WHERE name = :name")};
        query.bind(":name", groupName);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::addStudentToGroup(string studentLogin, string groupName)
{
    try {
        // SQLite::Statement query {*db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {*db, "INSERT OR IGNORE INTO students_to_group VALUES("
                                     "student_login = :student_login,"
                                     "group_name = :group_name)"};
        query.bind(":student_login", studentLogin);
        query.bind(":group_name", groupName);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteStudentFromGroup(string studentLogin, string groupName)
{
    try {
        SQLite::Statement query {*db, string("DELETE FROM students_to_group "
                                                 "WHERE student_login = :student_login AND "
                                                 "group_name = :group_name")};
        query.bind(":student_login", studentLogin);
        query.bind(":group_name", groupName);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

vector<string> *AppDatabase::getGroupStudentsLogins(string groupName)
{
    try {
        SQLite::Statement query {*db, "SELECT student_login FROM students_to_group "
                                      "WHERE group_name = :group_name"};
        query.bind(":group_name", groupName);

        vector<string> *students = nullptr;

        students = new vector<string>;

        while (query.executeStep()) {
            students->push_back(query.getColumn(0).getString());
        }

        if (students->empty()) {
            delete students;
            students = nullptr;
        }

        return students;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<string> *AppDatabase::getGroupStudentsNames(string groupName)
{
    try {
        SQLite::Statement query {*db, "SELECT users.name FROM students_to_group "
                                      "JOIN users ON login = student_login "
                                      "WHERE where group_name = :group_name"
                                      };
        query.bind(":group_name", groupName);

        vector<string> *students = nullptr;

        students = new vector<string>;

        while (query.executeStep()) {
            students->push_back(query.getColumn(0).getString());
        }

        if (students->empty()) {
            delete students;
            students = nullptr;
        }

        return students;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

int AppDatabase::addLecture(Lecture &lecture)
{
    try {
        SQLite::Statement query {*db, "INSERT INTO lectures VALUES(?,?,?,?,?)"};
        query.bind(1, lecture.teacher_name);
        query.bind(2, lecture.group_name);
        query.bind(3, lecture.thematic);
        query.bind(4, lecture.cabinet);
        query.bind(5, lecture.date.getDateString());

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteLecture(string teacherName, string date)
{
    try {
        // SQLite::Statement query {*db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {*db, "DELETE FROM lectures "
                                     "WHERE teacher_name = :teacher_name "
                                     "AND date = :date"};
        query.bind(":teacher_name", teacherName);
        query.bind(":date", date);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}
// "SELECT users.name FROM students_to_group "
//     "JOIN users ON login = student_login "
//     "WHERE where group_name = :group_name"
vector<Lecture> *AppDatabase::getLectures()
{
    try {
        SQLite::Statement query {*db, "SELECT * FROM lectures"};

        vector<Lecture> *lectures = new vector<Lecture>;

        while (query.executeStep()) {
            lectures->push_back(Lecture {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                DBDate(query.getColumn(4).getString()),
            });
        }

        if (lectures->empty()) {
            delete lectures;
            lectures = nullptr;
        }

        return lectures;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Lecture> *AppDatabase::getLecturesByGroup(string group)
{
    try {
        SQLite::Statement query {*db, "SELECT * FROM lectures "
                                     "WHERE group_name = :group_name"};
        query.bind(":group_name", group);

        vector<Lecture> *lectures = new vector<Lecture>;

        while (query.executeStep()) {
            lectures->push_back(Lecture {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                DBDate(query.getColumn(4).getString()),
            });
        }

        if (lectures->empty()) {
            delete lectures;
            lectures = nullptr;
        }

        return lectures;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Lecture> *AppDatabase::getLecturesByTeacher(string teacherName)
{
    try {
        SQLite::Statement query {*db, "SELECT * FROM lectures "
                                      "WHERE teacher_name = :teacher_name"};
        query.bind(":teacher_name", teacherName);

        vector<Lecture> *lectures = new vector<Lecture>;

        while (query.executeStep()) {
            lectures->push_back(Lecture {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                DBDate(query.getColumn(4).getString()),
            });
        }

        if (lectures->empty()) {
            delete lectures;
            lectures = nullptr;
        }

        return lectures;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

int AppDatabase::addPractice(Practice &practice)
{
    try {
        SQLite::Statement query {*db, "INSERT INTO practices VALUES(?,?,?,?,?)"};
        query.bind(1, practice.teacher_name);
        query.bind(2, practice.student_name);
        query.bind(3, practice.thematic);
        query.bind(4, practice.car);
        query.bind(5, practice.date.getDateString());

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deletePractice(string teacherName, string date)
{
    try {
        // SQLite::Statement query {*db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {*db, "DELETE FROM practices "
                                     "WHERE teacher_name = :teacher_name "
                                     "AND date = :date"};
        query.bind(":teacher_name", teacherName);
        query.bind(":date", date);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

vector<Practice> *AppDatabase::getPractices()
{
    try {
        SQLite::Statement query {*db, "SELECT * FROM practices"};

        vector<Practice> *practice = new vector<Practice>;

        while (query.executeStep()) {
            practice->push_back(Practice {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                DBDate(query.getColumn(4).getString()),
            });
        }

        if (practice->empty()) {
            delete practice;
            practice = nullptr;
        }

        return practice;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Practice> *AppDatabase::getPracticesByStudent(string studentName)
{
    try {
        SQLite::Statement query {*db, "SELECT * FROM practices "
                                      "WHERE student_name = :student_name"};
        query.bind(":student_name", studentName);

        vector<Practice> *practice = new vector<Practice>;

        while (query.executeStep()) {
            practice->push_back(Practice {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                DBDate(query.getColumn(4).getString()),
            });
        }

        if (practice->empty()) {
            delete practice;
            practice = nullptr;
        }

        return practice;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Practice> *AppDatabase::getPracticesByTeacher(string teacherName)
{
    try {
        SQLite::Statement query {*db, "SELECT * FROM practices "
                                     "WHERE teacher_name = :teacher_name"};
        query.bind(":teacher_name", teacherName);

        vector<Practice> *practice = new vector<Practice>;

        while (query.executeStep()) {
            practice->push_back(Practice {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                DBDate(query.getColumn(4).getString()),
            });
        }

        if (practice->empty()) {
            delete practice;
            practice = nullptr;
        }

        return practice;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}


