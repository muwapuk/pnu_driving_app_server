#include <httpserver.hpp>
#include <nlohmann/json.hpp>

#include "appdatabase.h"

using namespace httpserver;

class hello_world_resource : public http_resource {
public:
    std::shared_ptr<http_response> render(const http_request& req) {
        return std::shared_ptr<http_response>(new string_response("Hello: " + std::string(req.get_arg("name"))));
    }
};

int main(int, char**) {
    //webserver ws = create_webserver(8080);

    AppDatabase::getInstance();

    bool success = false;
    Question q {
        Question::AB,
        1,
        2,
        "12516",
        "123321",
        "123",
        2,
        "321"
    };
    User u {
        "1",
        "321",
        "sadf",
        User::student,
    };
    success = AppDatabase::getInstance()->addQuestion(q);
    success = AppDatabase::getInstance()->addUser(u);

    success = AppDatabase::getInstance()->addError("2", Question::AB, 2, 2, 1);
    success = AppDatabase::getInstance()->deleteError("1", Question::AB, 1, 2);
    success = AppDatabase::getInstance()->addError("1", Question::AB, 1, 2, 1);
    success = AppDatabase::getInstance()->addError("1", Question::AB, 1, 2, 2);
    success = AppDatabase::getInstance()->addError("1", Question::AB, 1, 2, 2);

    // hello_world_resource hwr;
    // ws.register_resource("/hello", &hwr);
    // ws.start(true);

    return 0;
}
