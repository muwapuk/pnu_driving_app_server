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
            "id INTEGER PRIMARY KEY,"
            "login TEXT,"
            "password TEXT,"
            "name TEXT,"
            "permissions INTEGER,"
            "UNIQUE(login));"
        );
        db.exec(
            "CREATE TABLE IF NOT EXISTS tokens ("
            "token TEXT PRIMARY KEY,"
            "user_id INTEGER,"
            "creation_time INTEGER,"
            "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE);"
        );
        db.exec(
            "CREATE TABLE IF NOT EXISTS teachers ("
            "user_id INTEGER PRIMARY KEY,"
            "car TEXT,"
            "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE);"
        );
        db.exec(
            "CREATE TABLE IF NOT EXISTS students ("
            "user_id INTEGER,"
            "group_id INTEGER,"
            "assigned_teacher INTEGER,"
            "FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE,"
            "FOREIGN KEY(group_id) REFERENCES groups(name) ON DELETE SET NULL,"
            "FOREIGN KEY(assigned_teacher) REFERENCES teachers(user_id) ON DELETE SET NULL,"
            "UNIQUE(user_id, group_id));"
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
            "teacher_id INTEGER,"
            "group_id INTEGER,"
            "title TEXT,"
            "classroom TEXT,"
            "time INTEGER,"
            "FOREIGN KEY(teacher_id) REFERENCES teachers(user_id) ON DELETE CASCADE,"
            "FOREIGN KEY(group_id) REFERENCES groups(id) ON DELETE CASCADE,"
            "UNIQUE(teacher_id, time),"
            "UNIQUE(group_id, time));"
        );
        db.exec(
            "CREATE TABLE IF NOT EXISTS practice_slots ("
            "teacher_id TEXT,"
            "time INTEGER,"
            "FOREIGN KEY(teacher_id) REFERENCES teachers(user_id) ON DELETE CASCADE,"
            "UNIQUE(teacher_id, time));"
        );
        db.exec(
            "CREATE TABLE IF NOT EXISTS practice_bookings ("
            "student_id TEXT,"
            "slot_id TEXT,"
            "FOREIGN KEY(student_id) REFERENCES students(user_id) ON DELETE CASCADE,"
            "FOREIGN KEY(slot_id) REFERENCES practice_slots(id) ON DELETE CASCADE,"
            "UNIQUE(slot_id));"
        );

        transaction.commit();

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
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
            "   SELECT"
            "       CASE"
            "           WHEN NEW.number NOT BETWEEN 1 AND (SELECT COALESCE(MAX(number), 0) + 1 FROM tickets) THEN"
            "               RAISE(ABORT, 'Ticket number out of range.')"
            "       END;"
            "END;"
        );
        // Shift tickets numbers on insertion of a new one
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS tickets_numbers_shift_on_insert "
            "BEFORE INSERT ON tickets "
            "WHEN NEW.number <= (SELECT COALESCE(MAX(number), 0) FROM tickets) + 1 "
            "BEGIN "
            "   UPDATE tickets "
            "   SET number = number + 1 "
            "   WHERE number >= NEW.number;"
            "END;"
        );
        // Shift tickets numbers on deletion
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS tickets_numbers_shift_on_delete "
            "AFTER DELETE ON tickets "
            "WHEN OLD.number < (SELECT COALESCE(MAX(number), 0) FROM tickets) "
            "BEGIN "
            "    UPDATE tickets "
            "    SET number = number - 1 "
            "    WHERE number > OLD.number;"
            "END;"
        );
        // Abort insertion of question with nummber < 1 or > max of existing number + 1
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS questions_insert_range_check "
            "BEFORE INSERT ON questions "
            "WHEN NEW.number NOT BETWEEN 1 AND (SELECT COALESCE(MAX(number), 0) + 1 FROM questions) "
            "BEGIN "
            "    SELECT RAISE(ABORT, 'Question number out of range.'); "
            "END;"
        );
        // Shift questions numbers on insertion of a new one
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS questions_numbers_shift_on_insert "
            "BEFORE INSERT ON questions "
            "WHEN NEW.number <= (SELECT COALESCE(MAX(number), 0) FROM questions) + 1 "
            "BEGIN "
            "    UPDATE questions "
            "    SET number = number + 1 "
            "    WHERE number >= NEW.number;"
            "END;"
        );
        // Shift questions numbers on updating one's number
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS shift_questions_on_update "
            "BEFORE UPDATE ON questions "
            "WHEN OLD.number != NEW.number "
            "BEGIN "
            "    SELECT CASE "
            "        WHEN NEW.number NOT BETWEEN 1 AND (SELECT COALESCE(MAX(number), 0) FROM questions) + 1 THEN "
            "            RAISE(ABORT, 'Question number out of range') "
            "    END;"
            "    UPDATE questions "
            "    SET number = CASE "
            "        WHEN NEW.number > OLD.number AND number > OLD.number AND number <= NEW.number AND id != OLD.id THEN number - 1 "
            "        WHEN NEW.number < OLD.number AND number >= NEW.number AND number < OLD.number AND id != OLD.id THEN number + 1 "
            "        ELSE number "
            "    END;"
            "END;"
        );
        // Shift questions numbers on deletion
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS questions_numbers_shift_on_delete "
            "AFTER DELETE ON questions "
            "WHEN OLD.number < (SELECT COALESCE(MAX(number), 0) FROM questions) "
            "BEGIN "
            "    UPDATE questions "
            "    SET number = number - 1 "
            "    WHERE number > OLD.number;"
            "END;"
        );
        // Check students book with their assigned teacher
        db.exec (
            "CREATE TRIGGER IF NOT EXISTS assigned_teacher_check "
            "BEFORE INSERT ON practice_bookings "
            "WHEN NOT EXISTS ("
            "    SELECT 1 FROM students "
            "    WHERE user_id = NEW.student_id "
            "    AND assigned_teacher = ("
            "        SELECT teacher_id FROM practice_slots "
            "        WHERE id = NEW.slot_id"
            "    )"
            ") "
            "BEGIN "
            "    SELECT RAISE(ABORT, 'Student must book with assigned teacher only'); "
            "END;"
        ); 

        transaction.commit();

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}



int AppDatabase::insertTicket(tickets::Categories category, int num)
{
    try {
        SQLite::Statement query {db, "INSERT INTO tickets VALUES (NULL, ?,?) RETURNING id"};

        query.bind(1, category);
        query.bind(2, num);

        if (query.executeStep()) {
            return query.getColumn(0).getInt();
        }
        return 0;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return 0;
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
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

shared_ptr<vector<pair<int, int>>>
AppDatabase::getTicketNumIdPairsByCategory(tickets::Categories category)
{
    try {
        SQLite::Statement query {db, string("SELECT number, id FROM tickets "
                                           "WHERE category = :category")};
        query.bind(":category", category);

        shared_ptr<vector<pair<int, int>>> pairs(new vector<pair<int, int>>());

        while (query.executeStep()) {
            pairs->push_back({
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt()
            });
        }

        return pairs;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<string>>
AppDatabase::getQuestionsSubjects()
{
    try {
        SQLite::Statement query {db, string("SELECT DISTINCT subject FROM questions")};

        shared_ptr<vector<string>> subjects(new vector<string>());

        while (query.executeStep()) {
            subjects->push_back(query.getColumn(0).getString());
        }

        return subjects;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<pair<int, int>>>
AppDatabase::getQuestionNumIdPairsByTicket(int ticketId)
{
    try {
        SQLite::Statement query {db, string("SELECT number, id FROM questions "
                                           "WHERE ticket_id = :ticket_id")};
        query.bind(":ticket_id", ticketId);

        shared_ptr<vector<pair<int, int>>> pairs(new vector<pair<int, int>>());

        while (query.executeStep()) {
            pairs->push_back({
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt()
            });
        }

        return pairs;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<int>>
AppDatabase::getQuestionIdVecBySubject(string subject)
{
    try {
        SQLite::Statement query {db, string("SELECT id FROM questions "
                                           "WHERE subject = :subject")};
        query.bind(":subject", subject);

        shared_ptr<vector<int>> ids(new vector<int>());

        while (query.executeStep()) {
            ids->push_back(query.getColumn(0).getInt());
        }

        return ids;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

int AppDatabase::insertQuestion(Question &question)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO questions VALUES (NULL, ?,?,?,?,?,?,?,?) RETURNING id"};

        query.bind(1, question.number);
        query.bind(2, question.ticketId);
        query.bind(3, question.subject);
        query.bind(4, question.image_base64);
        query.bind(5, question.questionText);
        query.bind(6, question.answers);
        query.bind(7, question.comment);
        query.bind(8, question.rightAnswer);

        if (query.executeStep()) {
            return query.getColumn(0).getInt();
        }
        return 0;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return 0;
    }
}

bool AppDatabase::changeQuestion(int id, Question &question)
{
    try {
         SQLite::Statement query {db, "UPDATE questions SET "
                                      "number = :number,"
                                      "subject = :subject,"
                                      "image = :image,"
                                      "question = :question,"
                                      "comment = :comment,"
                                      "answers = :answers,"
                                      "right_answer = :right_answer "
                                      "WHERE id = :id"};
        query.bind(":id", id);
        query.bind(":number", question.number);
        query.bind(":subject", question.subject);
        query.bind(":image", question.image_base64);
        query.bind(":question", question.questionText);
        query.bind(":comment", question.comment);
        query.bind(":answers", question.answers);
        query.bind(":right_answer", question.rightAnswer);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
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
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}
shared_ptr<Question> AppDatabase::getQuestion(int id)
{
    try {
        SQLite::Statement query {db, string("SELECT * FROM questions "
                                           "WHERE id = :id")};
        query.bind(":id", id);

        if (!query.executeStep()) {
            return nullptr;
        }

        shared_ptr<Question> question(new Question);
        question->id = query.getColumn(0).getInt();
        question->number = query.getColumn(1).getInt();
        question->ticketId = query.getColumn(2).getInt();
        question->subject = query.getColumn(3).getString();
        question->image_base64 = query.getColumn(4).getString();
        question->questionText = query.getColumn(5).getString();
        question->comment = query.getColumn(6).getString();
        question->answers = query.getColumn(7).getString();
        question->rightAnswer = query.getColumn(8).getInt();

        return question;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
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
        std::cerr << "exception: " << e.what() << std::endl;
        return 0;
    }
}

bool AppDatabase::insertGroup(string groupName)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO groups VALUES (NULL, ?)"};
        query.bind(1, groupName);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
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
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

int AppDatabase::getGroupIdByStudent(int studentId)
{
    try {
        SQLite::Statement query {db, "SELECT group_id FROM students WHERE user_id = :user_id"};
        query.bind(":user_id", studentId);

        if (!query.executeStep()) {
            return 0;
        }

        return query.getColumn(0).getInt();
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return 0;
    }
}

shared_ptr<string> AppDatabase::getGroupName(int id)
{
    try {
        SQLite::Statement query {db, "SELECT name FROM groups WHERE id = :id"};
        query.bind(":id", id);

        if (!query.executeStep()) {
            return nullptr;
        }

        return shared_ptr<string>(new string(query.getColumn(0).getString()));
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
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
        std::cerr << "exception: " << e.what() << std::endl;
        return 0;
    }
}

shared_ptr<vector<pair<int, string>>>
AppDatabase::getGroups()
{
    try {
        SQLite::Statement query {db, "SELECT * FROM groups"};

        shared_ptr<vector<pair<int, string>>> groups(new vector<pair<int, string>>);
        while (query.executeStep()) {
            int id = query.getColumn(0).getInt();
            string name = query.getColumn(1).getString();
            groups->push_back(pair<int,string>{id, name});
        }

        return groups;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

bool AppDatabase::insertUser(User &user)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO users VALUES (NULL, ?,?,?,?)"};
        query.bind(1, user.login);
        query.bind(2, user.password);
        query.bind(3, user.name);
        query.bind(4, user.permissions);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteUser(int id)
{
    try {
        SQLite::Statement query {db, string("DELETE FROM users WHERE id = :id")};
        query.bind(":id", id);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::changeUserName(int id, string newName)
{
    try {
        SQLite::Statement query {db, "UPDATE users SET "
                                    "name = :name "
                                    "WHERE id = :id"};
        query.bind(":name", newName);
        query.bind(":id", id);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::changeUserPassword(int id, string newPass)
{
    try {
        SQLite::Statement query {db, "UPDATE users SET "
                                      "password = :password "
                                      "WHERE id = :id"};
        query.bind(":password", newPass);
        query.bind(":id", id);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::changeUserPermissions(int id, User::Permissions newPerm)
{
    try {
        SQLite::Statement query {db, "UPDATE users SET "
                                    "permissions = :permissions "
                                    "WHERE id = :id"};
        query.bind(":permissions", newPerm);
        query.bind(":id", id);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::setUserStudent(int id, int groupId, int assignedTeacherId)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO students VALUES (?,?,?)"};
        query.bind(1, id);
        query.bind(2, groupId);
        query.bind(3, assignedTeacherId);

        if(!query.exec())
            return false;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
    if (!changeUserPermissions(id, User::STUDENT))
        return false;
    return true;
}

bool AppDatabase::changeStudentGroup(int studentId, int groupId)
{
    try {
        SQLite::Statement query {db, "UPDATE students SET "
                                    "group_id = :group_id "
                                    "WHERE user_id = :user_id"};
        query.bind(":group_id", groupId);
        query.bind(":user_id", studentId);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::assignStudentToTeacher(int studentId, int teacherId)
{
    try {
        SQLite::Statement query {db, "UPDATE students SET "
                                    "assigned_teacher = :assigned_teacher "
                                    "WHERE user_id = :user_id"};
        query.bind(":assigned_teacher", teacherId);
        query.bind(":user_id", studentId);

        if(!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::setUserTeacher(int id, string car)
{
    try {
        SQLite::Statement query {db, "INSERT OR IGNORE INTO teachers VALUES (?,?)"};
        query.bind(1, id);
        query.bind(2, car);

        if(!query.exec())
            return false;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
    if (!changeUserPermissions(id, User::TEACHER)) {
        return false;
    }
    return true;
}

int AppDatabase::getUserIdByLogin(string login)
{
    try {
        SQLite::Statement query {db, "SELECT id FROM users "
                                    "WHERE login = :login"};
        query.bind(":login", login);

        if (!query.executeStep())
            return 0;

        int id = query.getColumn(0).getInt();

        return id;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return 0;
    }
}

shared_ptr<User> AppDatabase::getUser(int id)
{
    try {
        SQLite::Statement query {db, "SELECT name, permissions FROM users "
                                     "WHERE id = :id"};
        query.bind(":id", id);

        if (!query.executeStep())
            return nullptr;

        shared_ptr<User> user(new User);
        user->name = query.getColumn(0).getString();
        user->permissions = static_cast<User::Permissions>(query.getColumn(1).getInt());

        return user;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<string> AppDatabase::getUserName(int id)
{
    try {
        SQLite::Statement query {db, "SELECT name FROM users "
                                    "WHERE id = :id"};
        query.bind(":id", id);

        shared_ptr<string> name(new string);

        if (!query.executeStep())
            return nullptr;

        *name = query.getColumn(0).getString();
        return name;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<string> AppDatabase::getUserPassword(int id)
{
    try {
        SQLite::Statement query {db, "SELECT password FROM users "
                                     "WHERE id = :id"};
        query.bind(":id", id);

        shared_ptr<string> password(new string);

        if (!query.executeStep())
            return nullptr;

        *password = query.getColumn(0).getString();
        return password;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

User::Permissions AppDatabase::getUserPermissions(int id)
{
    try {
        SQLite::Statement query {db, "SELECT permissions FROM users "
                                    "WHERE id = :id"};
        query.bind(":id", id);

        if (!query.executeStep())
            return User::NONE;

        User::Permissions permissions;
        permissions = static_cast<User::Permissions>(query.getColumn(0).getInt());
        return permissions;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return User::NONE;
    }
}

shared_ptr<vector<User>>
AppDatabase::getUsersList(int startRow, int amount)
{
    try {
        SQLite::Statement query {db, "SELECT id, name, permissions FROM users "
                                    " WHERE rowid >= :start_row "
                                    " AND rowid < :end_row"};
        query.bind(":start_row", startRow);
        query.bind(":end_row", startRow + amount);

        shared_ptr<vector<User>> users(new vector<User>);

        while (query.executeStep()) {
            User user;
            user.id = query.getColumn(0).getInt();
            user.name = query.getColumn(1).getString();
            user.permissions = static_cast<User::Permissions>(query.getColumn(2).getInt());
            users->push_back(user);
        }

        return users;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<Student>>
AppDatabase::getStudentsList(int startRow, int amount)
{
    try {
        SQLite::Statement query {db,
            "SELECT "
            "   s.user_id AS student_id,"
            "   u.name AS student_name,"
            "   g.group_name AS student_group_name,"
            "   t.user_id AS teacher_id,"
            "   ut.name AS teacher_name "
            "FROM "
            "   students s "
            "JOIN "
            "   users u ON s.user_id = u.id "
            "JOIN "
            "   groups g ON s.group_id = g.id "
            "JOIN "
            "   teachers t ON s.assigned_teacher = t.user_id "
            "JOIN "
            "   users ut ON t.user_id = ut.id "
            "WHERE rowid >= :start_row "
            "AND rowid < :end_row"
        };
        query.bind(":start_row", startRow);
        query.bind(":end_row", startRow + amount);

        shared_ptr<vector<Student>> students(new vector<Student>);

        while (query.executeStep()) {
            Student student;
            Teacher teacher;
            student.userId = query.getColumn("student_id").getInt();
            student.name = query.getColumn("student_name").getString();
            student.groupName = query.getColumn("student_group_name").getString();
            student.teacher = teacher;
            teacher.userId = query.getColumn("teacher_id").getInt();
            teacher.name = query.getColumn("teacher_name").getString();
            teacher.car = "";
            student.teacher = teacher;

            students->push_back(student);
        }

        return students;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<Student>>
AppDatabase::getStudentsListByGroup(int groupId, int startRow, int amount)
{
    try {
        SQLite::Statement query {db,
            "SELECT "
            "   s.user_id AS student_id,"
            "   u.name AS student_name,"
            "   t.user_id AS teacher_id,"
            "   ut.name AS teacher_name "
            "FROM "
            "   students s "
            "JOIN "
            "   users u ON s.user_id = u.id "
            "JOIN "
            "   groups g ON s.group_id = g.id "
            "JOIN "
            "   teachers t ON s.assigned_teacher = t.user_id "
            "JOIN "
            "   users ut ON t.user_id = ut.id "
            "WHERE group_id = :group_id"
            "AND rowid >= :start_row "
            "AND rowid < :end_row"
        };
        query.bind(":group_id", groupId);
        query.bind(":start_row", startRow);
        query.bind(":end_row", startRow + amount);

        shared_ptr<vector<Student>> students(new vector<Student>);

        while (query.executeStep()) {
            Student student;
            Teacher teacher;
            student.userId = query.getColumn("student_id").getInt();
            student.name = query.getColumn("student_name").getString();
            student.teacher = teacher;
            teacher.userId = query.getColumn("teacher_id").getInt();
            teacher.name = query.getColumn("teacher_name").getString();
            teacher.car = "";
            student.teacher = teacher;

            students->push_back(student);
        }

        return students;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<Teacher>>
AppDatabase::getTeachersList(int startRow, int amount)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM teachers "
                                    " WHERE rowid >= :start_row "
                                    " AND rowid < :end_row"};
        query.bind(":start_row", startRow);
        query.bind(":end_row", startRow + amount);

        shared_ptr<vector<Teacher>> teachers(new vector<Teacher>);

        while (query.executeStep()) {
            Teacher teacher;
            teacher.userId = query.getColumn(0).getInt();
            teacher.name = query.getColumn(1).getString();
            teacher.car = query.getColumn(2).getString();
            teachers->push_back(teacher);
        }

        return teachers;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

bool AppDatabase::isUserExist(int id)
{
    try {
        SQLite::Statement query {db, "SELECT 1 FROM users "
                                     "WHERE id = :id"};
        query.bind(":id", id);

        if (!query.executeStep())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::addToken(string token, int user_id)
{
    try {
        SQLite::Statement query {db, "INSERT OR REPLACE INTO tokens VALUES("
                                     ":token,"
                                     ":user_id,"
                                     ":time)"};
        query.bind(":token", token);
        query.bind(":user_id", user_id);
        query.bind(":time", time(0));

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
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
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteTokenByUser(int userId)
{
    try {
        SQLite::Statement query {db, string("DELETE FROM tokens "
                                           "WHERE user_id = :user_id")};
        query.bind(":user_id", userId);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
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
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

shared_ptr<pair<int, User::Permissions>>
AppDatabase::getUserIdAndPermissionsByToken(string token)
{
    try {
        SQLite::Statement query {db, "SELECT tokens.user_id, users.permissions FROM tokens "
                                      "JOIN users ON users.id = tokens.user_id "
                                      "WHERE token = :token"};
        query.bind(":token", token);

        if (!query.executeStep())
            return nullptr;

        shared_ptr<pair<int, User::Permissions>> idAndPerms(
            new pair<int, User::Permissions>(
                query.getColumn(0).getInt(),
                static_cast<User::Permissions>(query.getColumn(1).getInt())
            )
        );

        return idAndPerms;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

bool AppDatabase::insertLecture(Lecture &lecture)
{
    try {
        SQLite::Statement query {db, "INSERT INTO lectures VALUES(NULL, ?,?,?,?,?)"};
        query.bind(1, lecture.teacherId);
        query.bind(2, lecture.groupId);
        query.bind(3, lecture.title);
        query.bind(4, lecture.classroom);
        query.bind(5, lecture.time);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deleteLecture(int id)
{
    try {
        // SQLite::Statement query {db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {db, "DELETE FROM lectures "
                                     "WHERE id = :id"};
        query.bind(":id", id);

        if (!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

shared_ptr<Lecture> AppDatabase::getLecture(int id)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM lectures "
                                    "WHERE id = :id"};
        query.bind(":id", id);

        shared_ptr<Lecture> lecture;

        if (!query.executeStep()) {
            return nullptr;
        }

        lecture  = shared_ptr<Lecture>(new Lecture {
            id,
            query.getColumn(1).getInt(),
            query.getColumn(2).getInt(),
            query.getColumn(3).getString(),
            query.getColumn(4).getString(),
            query.getColumn(5).getInt(),
        });

        return lecture;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<Lecture>>
AppDatabase::getLecturesByTeacher(int teacherId)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM lectures "
                                    "WHERE teacher_id = :teacher_id"};
        query.bind(":teacher_id", teacherId);

        shared_ptr<vector<Lecture>> lectures(new vector<Lecture>);

        while (query.executeStep()) {
            lectures->push_back(Lecture {
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString(),
                query.getColumn(5).getInt(),
            });
        }

        return lectures;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<Lecture>>
AppDatabase::getLecturesByGroup(int groupId)
{
    try {
        SQLite::Statement query {db, "SELECT * FROM lectures "
                                     "WHERE group_id = :group_id"};
        query.bind(":group_id", groupId);

        shared_ptr<vector<Lecture>> lectures(new vector<Lecture>);

        while (query.executeStep()) {
            lectures->push_back(Lecture {
                query.getColumn(0).getInt(),
                query.getColumn(1).getInt(),
                query.getColumn(2).getInt(),
                query.getColumn(3).getString(),
                query.getColumn(4).getString(),
                query.getColumn(5).getInt(),
            });
        }

        return lectures;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

bool AppDatabase::insertPracticeSlot(PracticeSlot &slot)
{
    try {
        SQLite::Statement query {db, "INSERT INTO practice_slots VALUES(NULL, ?,?)"};
        query.bind(1, slot.teacherId);
        query.bind(2, slot.time);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deletePracticeSlot(int id)
{
    try {
        // SQLite::Statement query {db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {db, "DELETE FROM practice_slots "
                                    "WHERE id = :id"};
        query.bind(":id", id);

        if (!query.exec())
            return false;

        return true;

    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::insertPracticeBooking(PracticeBooking &booking)
{
    try {
        SQLite::Statement query {db, "INSERT INTO practice_bookings VALUES(NULL, ?,?)"};
        query.bind(1, booking.studentId);
        query.bind(2, booking.slotId);

        if (!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

bool AppDatabase::deletePracticeBooking(int id)
{
    try {
        // SQLite::Statement query {db, "INSERT OR REPLACE INTO users_errors VALUES(?,?,?)"};
        SQLite::Statement query {db, "DELETE FROM practice_bookings "
                                    "WHERE id = :id"};
        query.bind(":id", id);

        if (!query.exec())
            return false;

        return true;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return false;
    }
}

shared_ptr<PracticeSlot> AppDatabase::getPracticeSlot(int id)
{
    try {
        SQLite::Statement query {db, "SELECT teacher_id, time FROM practice_slots "
                                    "WHERE id = :id"};
        query.bind(":id", id);

        shared_ptr<PracticeSlot> slot(new PracticeSlot);

        while (query.executeStep()) {
            slot->teacherId = query.getColumn(0).getInt();
            slot->time = query.getColumn(1).getInt();
        }
        return slot;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<PracticeBooking> AppDatabase::getPracticeBooking(int id)
{
    try {
        SQLite::Statement query {db, "SELECT student_id, slot_id FROM practice_bookings "
                                    "WHERE id = :id"};
        query.bind(":id", id);

        shared_ptr<PracticeBooking> booking(new PracticeBooking);

        while (query.executeStep()) {
            booking->studentId = query.getColumn(0).getInt();
            booking->slotId = query.getColumn(1).getInt();
        }
        return booking;
    } catch(std::exception &e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<FreePracticeSlot>>
AppDatabase::getFreePracticeSlotsForStudent(int studentId)
{
    try {
        SQLite::Statement query(db,
                                "SELECT ps.id, ps.time "
                                "FROM practice_slots ps "
                                "JOIN students s ON ps.teacher_id = s.assigned_teacher "
                                "WHERE s.user_id = :student_id "
                                "AND NOT EXISTS ("
                                "   SELECT 1 FROM practice_bookings pb "
                                "   WHERE pb.slot_id = ps.id"
                                ")");

        query.bind(":student_id", studentId);

        shared_ptr<vector<FreePracticeSlot>> freeSlots(new vector<FreePracticeSlot>);
        while (query.executeStep()) {
            FreePracticeSlot slot;
            slot.slotId = query.getColumn(0).getInt();
            slot.time = query.getColumn(1).getInt();
            freeSlots->push_back(slot);
        }
        return freeSlots;
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<BookedPracticeSlot>>
AppDatabase::getBookedPracticeSlotsForStudent(int studentId)
{
    try {
        SQLite::Statement query(db,
                                "SELECT ps.id, pb.id, pb.student_id, u.name, ps.time "
                                "FROM practice_slots ps "
                                "JOIN students s ON ps.teacher_id = s.assigned_teacher "
                                "JOIN practice_bookings pb ON ps.id = pb.slot_id "
                                "JOIN users u ON pb.student_id = u.id "
                                "WHERE s.user_id = :student_id");

        query.bind(":student_id", studentId);

        shared_ptr<vector<BookedPracticeSlot>> bookedSlots(new vector<BookedPracticeSlot>);
        while (query.executeStep()) {
            BookedPracticeSlot slot;
            slot.slotId = query.getColumn(0).getInt();
            slot.bookingId = query.getColumn(1).getInt();
            slot.studentId = query.getColumn(2).getInt();
            slot.studentName = query.getColumn(3).getString();
            slot.time = query.getColumn(4).getInt();
            bookedSlots->push_back(slot);
        }
        return bookedSlots;
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<FreePracticeSlot>>
AppDatabase::getFreePracticeSlotsByTeacher(int teacherId)
{
    try {
        SQLite::Statement query(db,
                                "SELECT id, time FROM practice_slots "
                                "WHERE teacher_id = :teacher_id "
                                "AND id NOT IN (SELECT slot_id FROM practice_bookings)");
        query.bind(":teacher_id", teacherId);

        shared_ptr<vector<FreePracticeSlot>> freeSlots(new vector<FreePracticeSlot>);
        while (query.executeStep()) {
            FreePracticeSlot slot;
            slot.slotId = query.getColumn(0).getInt();
            slot.time = query.getColumn(1).getInt();
            freeSlots->push_back(slot);
        }
        return freeSlots;
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}

shared_ptr<vector<BookedPracticeSlot>>
AppDatabase::getBookedPracticeSlotsByTeacher(int teacherId)
{
    try {
        SQLite::Statement query(db,
                                "SELECT ps.id, pb.id, pb.student_id, u.name, ps.time "
                                "FROM practice_slots ps "
                                "JOIN practice_bookings pb ON ps.id = pb.slot_id "
                                "JOIN users u ON pb.student_id = u.id "
                                "WHERE ps.teacher_id = :teacher_id");
        query.bind(":teacher_id", teacherId);

        shared_ptr<vector<BookedPracticeSlot>> bookedSlots(new vector<BookedPracticeSlot>);
        while (query.executeStep()) {
            BookedPracticeSlot slot;
            slot.slotId = query.getColumn(0).getInt();
            slot.studentId = query.getColumn(1).getInt();
            slot.studentName = query.getColumn(2).getString();
            slot.time = query.getColumn(3);
            bookedSlots->push_back(slot);
        }
        return bookedSlots;
    } catch (const std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
        return nullptr;
    }
}
