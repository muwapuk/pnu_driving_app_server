#include "appdatabase.h"
#include "jsonconverter.h"
#include <iostream>

using namespace nlohmann;

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
    std::string str = "{\"idt\": \"1\"}";


    json j;
    try {
        j = json::parse(str);
    } catch (json::exception &e) {
        std::cerr << e.what() << std::endl;
    }


    int num;

    std::cout << JsonConverter::intToJson("num1", 1, j);
    std::cout << JsonConverter::jsonObjectToInt(j, "num1", num);


    // hello_world_resource hwr;
    // ws.register_resource("/hello", &hwr);
    // ws.start(true);

    return 0;
}
