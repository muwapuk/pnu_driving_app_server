#include "appdatabase.h"

#include <iostream>


AppDatabase::AppDatabase()
{
    createTables();
    createTriggers();
}

AppDatabase::~AppDatabase(){}


AppDatabase &AppDatabase::getInstance() {
    static AppDatabase instance;
    return instance;
}

bool AppDatabase::createTables()
{
    try {
        SQLite::Transaction transaction(db);

        db.exec(
            "CREATE TABLE IF NOT EXISTS tickets ("
            "id INTEGER PRIMARY KEY,"
            "number INTEGER,"
            "category INTEGER,"
            "UNIQUE(number));"
        );

        db.exec(
            "CREATE TABLE IF NOT EXISTS questions ("
            "id INTEGER PRIMARY KEY,"
            "number INTEGER,"
            "ticket_id INTEGER,"
            "subject TEXT,"
            "image TEXT,"
            "question TEXT,"
            "comment TEXT,"
            "answers TEXT,"
            "right_answer INTEGER,"
            "FOREIGN KEY(ticket_id) REFERENCES tickets(id) ON DELETE CASCADE);"
        );

        db.exec(
            "CREATE TABLE IF NOT EXISTS users ("
            "login TEXT PRIMARY KEY,"
            "password TEXT,"
            "name TEXT,"
            "permissions INTEGER);"
        );

        db.exec(
            "CREATE TABLE IF NOT EXISTS tokens ("
            "token TEXT PRIMARY KEY,"
            "login TEXT,"
            "creation_time INTEGER,"
            "FOREIGN KEY(login) REFERENCES users(login) ON DELETE CASCADE);"
        );

        db.exec(
            "CREATE TABLE IF NOT EXISTS teachers ("
            "user_login TEXT PRIMARY KEY,"
            "car TEXT,"
            "FOREIGN KEY(user_login) REFERENCES users(login) ON DELETE CASCADE);"
        );

        db.exec(
            "CREATE TABLE IF NOT EXISTS students ("
            "user_login TEXT,"
            "group_id TEXT,"
            "assigned_teacher TEXT,"
            "FOREIGN KEY(user_login) REFERENCES users(login) ON DELETE CASCADE,"
            "FOREIGN KEY(group_id) REFERENCES groups(name) ON DELETE SET NULL,"
            "FOREIGN KEY(assigned_teacher) REFERENCES teachers(user_login) ON DELETE SET NULL,"
            "UNIQUE(user_login, group_id));"
        );

        db.exec(
            "CREATE TABLE IF NOT EXISTS groups ("
            "id INTEGER PRIMARY KEY,"
            "name TEXT,"
            "UNIQUE(name));"
            );

        db.exec(
            "CREATE TABLE IF NOT EXISTS lectures ("
            "id INTEGER PRIMARY KEY,"
            "teacher_login TEXT,"
            "group_id INTEGER,"
            "title TEXT,"
            "classroom TEXT,"
            "time INTEGER,"
            "FOREIGN KEY(teacher_login) REFERENCES teachers(user_login) ON DELETE CASCADE,"
            "FOREIGN KEY(group_id) REFERENCES groups(id) ON DELETE CASCADE,"
            "UNIQUE(teacher_login, time),"
            "UNIQUE(group_id, time));"
        );

        db.exec(
            "CREATE TABLE IF NOT EXISTS practices ("
            "student_login TEXT,"
            "teacher_login TEXT,"
            "title TEXT,"
            "time INTEGER,"
            "FOREIGN KEY(student_login) REFERENCES students(user_login) ON DELETE SET NULL,"
            "FOREIGN KEY(teacher_login) REFERENCES teachers(user_login) ON DELETE CASCADE,"
            "UNIQUE(student_login, time),"
            "UNIQUE(teacher_login, time));"
        );

        transaction.commit();

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::createTriggers()
{
    try {
        SQLite::Transaction transaction(db);

        // Abort insertion of ticket with nummber < 1 or > max of existing number + 1
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS tickets_insert_range_check "
            "BEFORE INSERT ON tickets "
            "BEGIN "
            "   SELECT COALESCE(MAX(number), 0) INTO @max_number FROM tickets;"
            "   SELECT"
            "       CASE"
            "           WHEN NEW.number NOT BETWEEN 1 AND (@max_number + 1) THEN"
            "               RAISE(ABORT, 'Ticket number out of range. Must be between 1 and ' || (@max_number + 1))"
            "       END;"
            "END;"
        );

        // Shift tickets numbers on insertion of a new one
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS tickets_numbers_shift_on_insert "
            "BEFORE INSERT ON tickets "
            "BEGIN "
            "   SELECT COALESCE(MAX(number), 0) INTO @max_number FROM tickets;"
            "   IF NEW.number <= @max_number THEN "
            "       UPDATE tickets "
            "       SET number = number + 1 "
            "       WHERE number >= NEW.number;"
            "   END IF;"
            "END;"
        );

        // Shift tickets numbers on deletion
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS tickets_numbers_shift_on_insert "
            "AFTER DELETE ON tickets "
            "BEGIN "
            "SELECT COALESCE(MAX(number), 0) INTO @current_max FROM tickets;"
            "IF OLD.number < @current_max THEN"
            "   UPDATE tickets"
            "   SET number = number - 1"
            "   WHERE number > OLD.number;"
            "   END IF;"
            "END;"
        );

        // Abort insertion of question with nummber < 1 or > max of existing number + 1
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS questions_insert_range_check "
            "BEFORE INSERT ON questions "
            "BEGIN "
            "   SELECT COALESCE(MAX(number), 0) INTO @max_number FROM questions;"
            "   SELECT"
            "       CASE"
            "           WHEN NEW.number NOT BETWEEN 1 AND (@max_number + 1) THEN"
            "               RAISE(ABORT, 'Ticket number out of range. Must be between 1 and ' || (@max_number + 1))"
            "       END;"
            "END;"
        );

        // Shift questions numbers on insertion of a new one
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS questions_numbers_shift_on_insert "
            "BEFORE INSERT ON questions "
            "BEGIN "
            "   SELECT COALESCE(MAX(number), 0) INTO @max_number FROM questions;"
            "   IF NEW.number <= @max_number THEN "
            "       UPDATE questions "
            "       SET number = number + 1 "
            "       WHERE number >= NEW.number;"
            "   END IF;"
            "END;"
        );

        // Shift questions numbers on updating one's number
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS shift_questions_on_update "
            "BEFORE UPDATE ON questions "
            "WHEN OLD.number != NEW.number "
            "BEGIN "
            "   SELECT COALESCE(MAX(number), 0) INTO @max_number FROM questions;"
            "   SELECT CASE "
            "       WHEN NEW.number NOT BETWEEN 1 AND (@max_number + 1) THEN "
            "   END;"
            "   IF NEW.number > OLD.number THEN "
            "       UPDATE questions "
            "       SET number = number - 1 "
            "       WHERE number > OLD.number AND number <= NEW.number "
            "       AND id != OLD.id;  -- Exclude the question being updated "
            "   ELSE "
            "       UPDATE questions "
            "       SET number = number + 1 "
            "       WHERE number >= NEW.number AND number < OLD.number "
            "       AND id != OLD.id;  -- Exclude the question being updated "
            "   END IF;"
            "END;"
            );

        // Shift questions numbers on deletion
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS questions_numbers_shift_on_insert "
            "AFTER DELETE ON questions "
            "BEGIN "
            "SELECT COALESCE(MAX(number), 0) INTO @current_max FROM questions;"
            "IF OLD.number < @current_max THEN"
            "   UPDATE questions"
            "   SET number = number - 1"
            "   WHERE number > OLD.number;"
            "   END IF;"
            "END;"
        );

        // Check students sign for their assigned teacher's practices
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS assigned_teacher_check "
            "BEFORE INSERT ON practices "
            "BEGIN "
            "   SELECT RAISE(ABORT, 'Not assigned teacher!') "
            "   FROM practices "
            "       WHERE "
            "           (SELECT assigned_teacher FROM students "
            "               WHERE user_login = student_login"
            "           ) != teacher_login;"
            "END;"
        );
        transaction.commit();

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}



bool AppDatabase::insertTicket(tickets::Categories category, int num)
{
    try {
        SQLite::Statement query {db, "INSERT INTO tickets VALUES (?,?)"};

        query.bind(1, category);
        query.bind(2, num);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteTicket(int id)
{
    try {
        SQLite::Statement query {db, "DELETE FROM tickets WHERE id = :id"};
        query.bind(":id", id);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<vector<int>> AppDatabase::getTicketsIdByCategory(tickets::Categories category)
{
    try {
        SQLite::Statement query {db, string("SELECT id FROM tickets "
                                           "WHERE category = :category")};
        query.bind(":category", category);

        std::shared_ptr<vector<int>> ids(new vector<int>());

        while (query.executeStep()) {
            ids->push_back(query.getColumn(1).getInt());
        }

        return ids;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<vector<string>> AppDatabase::getQuestionsSubjects()
{
    try {
        SQLite::Statement query {db, string("SELECT DISTINCT subject FROM questions")};

        std::shared_ptr<vector<string>> subjects(new vector<string>());

        while (query.executeStep()) {
            subjects->push_back(query.getColumn(0).getString());
        }

        return subjects;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<vector<int>> AppDatabase::getQuestionsIdByTicket(int ticketId)
{
    try {
        SQLite::Statement query {db, string("SELECT id FROM questions "
                                           "WHERE ticket_id = :ticket_id")};
        query.bind(":ticket_id", ticketId);

        std::shared_ptr<vector<int>> ids(new vector<int>());

        while (query.executeStep()) {
            ids->push_back(query.getColumn(0).getInt());
        }

        return ids;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<vector<int>> AppDatabase::getQuestionsIdBySubject(string subject)
{
    try {
        SQLite::Statement query {db, string("SELECT id FROM questions "
                                           "WHERE subject = :subject")};
        query.bind(":subject", subject);

        std::shared_ptr<vector<int>> ids(new vector<int>());

        while (query.executeStep()) {
            ids->push_back(query.getColumn(0).getInt());
        }

        return ids;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

bool AppDatabase::insertQuestion(Question &question)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO questions VALUES (?,?,?,?,?,?,?,?)"};

        query.bind(2, question.number);
        query.bind(3, question.ticketId);
        query.bind(4, question.subject);
        query.bind(5, question.image_base64);
        query.bind(6, question.questionText);
        query.bind(7, question.answers);
        query.bind(8, question.comment);
        query.bind(9, question.rightAnswer);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::changeQuestion(int id, Question &question)
{
    try {
        SQLite::Statement query {db, "UPDATE questions SET "
                                      "number = :number,"
                                      "subject = :subject,"
                                      "image_base64 = :image_base64,"
                                      "question = :question,"
                                      "comment = :comment,"
                                      "answers = :answers,"
                                      "right_answer = :right_answer "
                                      "WHERE id = " + std::to_string(id)};
        query.bind(":number", question.number);
        query.bind(":subject", question.subject);
        query.bind(":image_base64", question.image_base64);
        query.bind(":question", question.questionText);
        query.bind(":comment", question.comment);
        query.bind(":answers", question.answers);
        query.bind(":right_answer", question.rightAnswer);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteQuestion(int id)
{
    try {
        SQLite::Statement query {db, string("DELETE FROM questions WHERE id = :id")};
        query.bind("id", id);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

int AppDatabase::getRightAnswer(int id)
{
    try {
        SQLite::Statement query {db, string("SELECT right_answer FROM questions "
                                           "WHERE id = :id")};
        query.bind(":id", id);

        if (!query.executeStep()) {
            return 0;
        }

        return query.getColumn(0).getInt();

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return 0;
    }
}

bool AppDatabase::insertGroup(string groupName)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO groups VALUES (?)"};
        query.bind(1, groupName);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteGroup(int id)
{
    try {
        SQLite::Statement query {db, "DELETE FROM groups WHERE id = :id"};
        query.bind(":id", id);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

std::shared_ptr<string> AppDatabase::getGroupName(int id)
{
    try {
        SQLite::Statement query {db, "SELECT name FROM groups WHERE id = :id"};
        query.bind(":id", id);

        if (!query.executeStep()) {
            return nullptr;
        }

        return std::shared_ptr<string>(new string(query.getColumn(0).getString()));
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

int AppDatabase::getGroupId(string groupName)
{
    try {
        SQLite::Statement query {db, "SELECT id FROM groups WHERE name = :name"};
        query.bind(":name", groupName);

        if (!query.executeStep()) {
            return 0;
        }

        return query.getColumn(0).getInt();
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return 0;
    }
}

bool AppDatabase::insertUser(User &user)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO users VALUES (?,?,?,?)"};
        query.bind(1, user.login);
        query.bind(2, user.password);
        query.bind(3, user.name);
        query.bind(4, user.permissions);

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
        SQLite::Statement query {db, string("DELETE FROM users WHERE login = :login")};
        query.bind(":login", login);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::changeUserName(string login, string newName)
{
    try {
        SQLite::Statement query {db, "UPDATE users SET "
                                    "name = :name "
                                    "WHERE login = :login"};
        query.bind(":name", newName);
        query.bind(":login", login);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::changeUserPassword(string login, string newPass)
{
    try {
        SQLite::Statement query {db, "UPDATE users SET "
                                      "password = :password "
                                      "WHERE login = :login"};
        query.bind(":password", newPass);
        query.bind(":login", login);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::changeUserPermissions(string login, User::Permissions newPerm)
{
    try {
        SQLite::Statement query {db, "UPDATE users SET "
                                    "permissions = :permissions "
                                    "WHERE login = :login"};
        query.bind(":permissions", newPerm);
        query.bind(":login", login);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::setUserStudent(string login, int groupId, string assignedTeacherLogin)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO students VALUES (?,?,?)"};
        query.bind(1, login);
        query.bind(2, groupId);
        query.bind(3, assignedTeacherLogin);

        if(!query.exec())
            return false;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
    if (changeUserPermissions(login, User::STUDENT))
        return false;
    return true;
}

bool AppDatabase::changeStudentGroup(string studentLogin, int groupId)
{
    try {
        SQLite::Statement query {db, "UPDATE students SET "
                                    "group_id = :group_id "
                                    "WHERE user_login = :user_login"};
        query.bind(":group_id", groupId);
        query.bind(":user_login", studentLogin);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::assignStudentTeacher(string studentLogin, string teacherLogin)
{
    try {
        SQLite::Statement query {db, "UPDATE students SET "
                                    "assigned_teacher = :assigned_teacher "
                                    "WHERE user_login = :user_login"};
        query.bind(":assigned_teacher", teacherLogin);
        query.bind(":user_login", studentLogin);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::setUserTeacher(string login, string car)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO teachers VALUES (?,?)"};
        query.bind(1, login);
        query.bind(2, car);

        if(!query.exec())
            return false;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
    if (changeUserPermissions(login, User::TEACHER)) {
        return false;
    }
    return true;
}

std::shared_ptr<User> AppDatabase::getUser(string login)
{
    try {
        SQLite::Statement query {db, "SELECT name, permissions FROM users "
                                     "WHERE login = :login"};
        query.bind(":login", login);

        if (!query.executeStep())
            return nullptr;

        std::shared_ptr<User> user;
        user->login = login;
        user->name = query.getColumn(2).getString();
        user->permissions = static_cast<User::Permissions>(query.getColumn(3).getInt());

        return user;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<vector<User>> AppDatabase::getUsersList(int startRow, int amount)
{
    try {
        SQLite::Statement query {db, "SELECT login, name, permissions FROM users "
                                    " WHERE rowid >= :start_row "
                                    " AND rowid < :end_row"};
        query.bind(":start_row", startRow);
        query.bind(":end_row", startRow + amount);

        std::shared_ptr<vector<User>> users;

        while (query.executeStep()) {
            User user;
            user.login = query.getColumn(0).getString();
            user.name = query.getColumn(1).getString();
            user.permissions = static_cast<User::Permissions>(query.getColumn(2).getInt());
            users->push_back(user);
        }

        return users;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<vector<Student>> AppDatabase::getStudentsList(int startRow, int amount)
{
    try {
        SQLite::Statement query {db,
            "SELECT "
            "   s.user_login AS student_login,"
            "   u.name AS student_name,"
            "   g.group_name AS student_group_name,"
            "   t.user_login AS teacher_login,"
            "   ut.name AS teacher_name "
            "FROM "
            "   students s "
            "JOIN "
            "   users u ON s.user_login = u.login "
            "JOIN "
            "   groups g ON s.group_id = g.id "
            "JOIN "
            "   teachers t ON s.assigned_teacher = t.user_login "
            "JOIN "
            "   users ut ON t.user_login = ut.login "
            "WHERE rowid >= :start_row "
            "AND rowid < :end_row"
        };
        query.bind(":start_row", startRow);
        query.bind(":end_row", startRow + amount);

        std::shared_ptr<vector<Student>> students;

        while (query.executeStep()) {
            Student student;
            Teacher teacher;
            student.login = query.getColumn("student_login").getString();
            student.name = query.getColumn("student_name").getString();
            student.groupName = query.getColumn("student_group_name").getString();
            student.teacher = teacher;
            teacher.login = query.getColumn("teacher_login").getString();
            teacher.name = query.getColumn("teacher_name").getString();
            teacher.car = "";
            student.teacher = teacher;

            students->push_back(student);
        }

        return students;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<vector<Student> > AppDatabase::getStudentsListByGroup(int groupId, int startRow, int amount)
{
    try {
        SQLite::Statement query {db,
            "SELECT "
            "   s.user_login AS student_login,"
            "   u.name AS student_name,"
            "   t.user_login AS teacher_login,"
            "   ut.name AS teacher_name "
            "FROM "
            "   students s "
            "JOIN "
            "   users u ON s.user_login = u.login "
            "JOIN "
            "   groups g ON s.group_id = g.id "
            "JOIN "
            "   teachers t ON s.assigned_teacher = t.user_login "
            "JOIN "
            "   users ut ON t.user_login = ut.login "
            "WHERE group_id = :group_id"
            "AND rowid >= :start_row "
            "AND rowid < :end_row"
        };
        query.bind(":group_id", groupId);
        query.bind(":start_row", startRow);
        query.bind(":end_row", startRow + amount);

        std::shared_ptr<vector<Student>> students;

        while (query.executeStep()) {
            Student student;
            Teacher teacher;
            student.login = query.getColumn("student_login").getString();
            student.name = query.getColumn("student_name").getString();
            student.teacher = teacher;
            teacher.login = query.getColumn("teacher_login").getString();
            teacher.name = query.getColumn("teacher_name").getString();
            teacher.car = "";
            student.teacher = teacher;

            students->push_back(student);
        }

        return students;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

std::shared_ptr<vector<Teacher>> AppDatabase::getTeachersList(int startRow, int amount)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM teachers "
                                    " WHERE rowid >= :start_row "
                                    " AND rowid < :end_row"};
        query.bind(":start_row", startRow);
        query.bind(":end_row", startRow + amount);

        std::shared_ptr<vector<Teacher>> teachers;

        while (query.executeStep()) {
            Teacher teacher;
            teacher.login = query.getColumn(0).getString();
            teacher.name = query.getColumn(1).getString();
            teacher.car = query.getColumn(2).getString();
            teachers->push_back(teacher);
        }

        return teachers;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

bool AppDatabase::isUserExist(string login)
{
    try {
        SQLite::Statement query {db, "SELECT 1 FROM users "
                                     "WHERE login = :login"};
        query.bind(":login", login);

        if (!query.executeStep())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::addToken(string token, string login)
{
    try {
        SQLite::Statement query {db, "INSERT OR REPLACE INTO tokens VALUES("
                                     ":token,"
                                     ":login,"
                                     ":time)"};
        query.bind(":token", token);
        query.bind(":login", login);
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
        SQLite::Statement query {db, string("DELETE FROM tokens "
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
        SQLite::Statement query {db, string("DELETE FROM tokens "
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

std::shared_ptr<pair<string, User::Permissions>> AppDatabase::getLoginAndPermissionsByToken(string token)
{
    try {
        SQLite::Statement query {db, "SELECT tokens.login, users.permissions FROM tokens "
                                      "JOIN users ON users.login = tokens.login "
                                      "WHERE token = :token"};
        query.bind(":token", token);

        if (!query.executeStep())
            return nullptr;

        std::shared_ptr<pair<string, User::Permissions>> loginAndPerms(
            new pair<string, User::Permissions>(
                query.getColumn(0).getString(),
                static_cast<User::Permissions>(query.getColumn(1).getInt())
            )
        );

        return loginAndPerms;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}



bool AppDatabase::addLecture(Lecture &lecture)
{
    try {
        SQLite::Statement query {db, "INSERT INTO lectures VALUES(?,?,?,?,?)"};
        query.bind(1, lecture.teacher_login);
        query.bind(2, lecture.group_name);
        query.bind(3, lecture.thematic);
        query.bind(4, lecture.cabinet);
        query.bind(5, lecture.date);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteLecture(string teacherlogin, int date)
{
    try {
        // SQLite::Statement query {db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {db, "DELETE FROM lectures "
                                     "WHERE teacher_login = :teacher_login "
                                     "AND date = :date"};
        query.bind(":teacher_login", teacherlogin);
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
        SQLite::Statement query {db, "SELECT * FROM lectures"};

        vector<Lecture> *lectures = new vector<Lecture>{};

        while (query.executeStep()) {
            lectures->push_back(Lecture {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getInt(),
            });
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
        SQLite::Statement query {db, "SELECT * FROM lectures "
                                     "WHERE group_name = :group_name"};
        query.bind(":group_name", group);

        vector<Lecture> *lectures = new vector<Lecture>{};

        while (query.executeStep()) {
            lectures->push_back(Lecture {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getInt(),
            });
        }

        return lectures;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Lecture> *AppDatabase::getLecturesByTeacher(string teacherLogin)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM lectures "
                                      "WHERE teacher_login = :teacher_login"};
        query.bind(":teacher_login", teacherLogin);

        vector<Lecture> *lectures = new vector<Lecture>{};

        while (query.executeStep()) {
            lectures->push_back(Lecture {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getInt(),
            });
        }

        return lectures;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

bool AppDatabase::addStudentsToInstructorsLink(string studentLogin, string teacherLogin)
{
    try {
        SQLite::Statement query {db, "INSERT INTO students_to_instructors VALUES(?,?)"};
        query.bind(1, studentLogin);
        query.bind(2, teacherLogin);


        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteStudentsToInstructorsLink(string studentLogin, string teacherLogin)
{
    try {
        SQLite::Statement query {db, "DELETE FROM students_to_instructors "
                                     "WHERE student_login = :student_login "
                                     "AND teacher_login = :teacher_login"};
        query.bind(1, studentLogin);
        query.bind(2, teacherLogin);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

string *AppDatabase::getStudentsInstructorLogin(string studentLogin)
{
    try {
        SQLite::Statement query {db, "SELECT teacher_login FROM students_to_instructors "
                                     "WHERE student_login = :student_login"};
        query.bind(":student_login", studentLogin);

        if(!query.executeStep())
            return nullptr;
        string *teacherLogin = new string;
        *teacherLogin = query.getColumn(0).getString();

        return teacherLogin;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<string> *AppDatabase::getInstructorsStudentsLogins(string teacherLogin)
{
    try {
        SQLite::Statement query {db, "SELECT student_login FROM students_to_instructors "
                                     "WHERE teacher_login = :teacher_login"};
        query.bind(":teacher_login", teacherLogin);

        vector<string> *studentLogins = new vector<string>{};

        while (query.executeStep()) {
            studentLogins->push_back(query.getColumn(0).getString());
        }

        return studentLogins;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

bool AppDatabase::addPractice(Practice &practice)
{
    try {
        SQLite::Statement query {db, "INSERT INTO practices VALUES(?,?,?,?,?)"};
        query.bind(1, practice.teacher_login);
        query.bind(2, practice.student_login);
        query.bind(3, practice.thematic);
        query.bind(4, practice.car);
        query.bind(5, practice.date);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::changePractice(Practice &practice)
{
    try {
        SQLite::Statement query {db, "UPDATE practices SET "
                                     "student = :student,"
                                     "car = :car "
                                     "WHERE teacher = :teacher "
                                     "AND date = :date"};
        query.bind(":student", practice.student_login);
        query.bind(":car", practice.car);
        query.bind(":teacher", practice.teacher_login);
        query.bind(":date", practice.date);

        // If no changes
        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deletePractice(string teacherLogin, int date)
{
    try {
        // SQLite::Statement query {db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {db, "DELETE FROM practices "
                                     "WHERE teacher_login = :teacher_login "
                                     "AND date = :date"};
        query.bind(":teacher_login", teacherLogin);
        query.bind(":date", date);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return false;
    }
}

Practice *AppDatabase::getPractice(string teacherLogin, int date)
{
    try {
        // SQLite::Statement query {db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {db, "SELECT * FROM practices "
                                     "WHERE teacher_login = :teacher_login "
                                     "AND date = :date"};
        query.bind(":teacher_login", teacherLogin);
        query.bind(":date", date);

        Practice *practice;

        if (!query.executeStep())
            return nullptr;

        practice = new Practice {
            query.getColumn(0).getString(),
            query.getColumn(1).getString(),
            query.getColumn(2).getString(),
            query.getColumn(3).getString(),
            query.getColumn(4).getInt(),
        };

        return practice;
    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Practice> *AppDatabase::getPractices()
{
    try {
        SQLite::Statement query {db, "SELECT * FROM practices"};

        vector<Practice> *practice = new vector<Practice>{};

        while (query.executeStep()) {
            practice->push_back(Practice {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getInt(),
            });
        }

        return practice;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Practice> *AppDatabase::getPracticesByStudent(string studentLogin)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM practices "
                                      "WHERE student_login = :student_login"};
        query.bind(":student_login", studentLogin);

        vector<Practice> *practice = new vector<Practice>{};

        while (query.executeStep()) {
            practice->push_back(Practice {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getInt(),
            });
        }

        return practice;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Practice> *AppDatabase::getPracticesByUserInstructor(string studentLogin)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM practices "
                                     "WHERE teacher_login = "
                                     "SELECT teacher_login FROM students_to_instructors "
                                     "WHERE student_login = :student_login"};
        query.bind(":student_login", studentLogin);

        vector<Practice> *practice = new vector<Practice>{};

        while (query.executeStep()) {
            practice->push_back(Practice {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getInt(),
            });
        }

        return practice;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Practice> *AppDatabase::getPracticesByTeacher(string teacherLogin)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM practices "
                                     "WHERE teacher_login = :teacher_login"};
        query.bind(":teacher_login", teacherLogin);

        vector<Practice> *practice = new vector<Practice>{};

        while (query.executeStep()) {
            practice->push_back(Practice {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getInt(),
            });
        }

        return practice;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}

vector<Practice> *AppDatabase::getPracticesByTime(int fromTime, int toTime)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM practices "
                                     "WHERE date >= :fromTime "
                                     "AND date <= :toTime"};
        query.bind(":fromTime", fromTime);
        query.bind(":toTime", toTime);

        vector<Practice> *practice = new vector<Practice>{};

        while (query.executeStep()) {
            practice->push_back(Practice {
                query.getColumn(0).getString(),
                query.getColumn(1).getString(),
                query.getColumn(2).getString(),
                query.getColumn(3).getString(),
                query.getColumn(4).getInt(),
            });
        }

        return practice;

    } catch(std::exception &e) {
        std::cerr << "exeption: " << e.what() << std::endl;
        return nullptr;
    }
}


