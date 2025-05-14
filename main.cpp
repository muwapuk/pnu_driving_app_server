#include "appdatabase.h"
#include "jsonconverter.h"
#include "questions_resources.h"
#include "users_resources.h"

#include <iostream>


class hell_world_resource : public http_resource {
public:
    std::shared_ptr<http_response> render(const http_request& req) {
        auto a = req.get_arg("name").values;
        return std::shared_ptr<http_response>(new string_response("Hello: " + std::string(req.get_arg("ar"))));
    }
};

int main(int argc, char** argv) {
    webserver ws = create_webserver(8080);


    hell_world_resource hwr;
    ws.register_resource("/hello/{ar|[0-99]+}", &hwr);
    ws.start(true);

    return 0;
}
