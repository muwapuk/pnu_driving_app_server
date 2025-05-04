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

int main(int argc, char** argv) {
    webserver ws = create_webserver(8080);

    AppDatabase::getInstance();


    Question quest = {
        1,
        2,
        "adsf123",
        "adsf123",
        "adsf123",
        1123,
        "adsf123"
    };
    AppDatabase::getInstance()->addQuestion(quest);

    std::shared_ptr<Question> q1 = AppDatabase::getInstance()->getQuestion(1, 3);


    hello_world_resource hwr;
    ws.register_resource("/hello", &hwr);
    ws.start(true);

    return 0;
}
