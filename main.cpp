#include "appdatabase.h"
#include "json_converter.h"


int main(int, char**) {
    //webserver ws = create_webserver(8080);

    AppDatabase::getInstance();

    Question q {
        Question::AB,
        1,
        1,
        "asdf",
        "asdf",
        "asdf",
        1,
        "asdf",
    };

    std::string *qjsonstr = json_converter::questionToJsonString(q);
    auto q2 = json_converter::jsonStringToQuestion(*qjsonstr);

    // hello_world_resource hwr;
    // ws.register_resource("/hello", &hwr);
    // ws.start(true);

    return 0;
}
