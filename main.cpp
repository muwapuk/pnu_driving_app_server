#include "jsonconverter.h"
#include "questions_resources.h"


void register_resources(webserver &server)
{
    qr::questions_resource hwr;

    server.register_resource("/questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+}", &hwr);
}


int main(int, char**)
{
    webserver ws = create_webserver(8080);
    ///
    auto u1 = User {
        "a","a","a",User::student
    };
    auto u2 = User {
        "b","b","b",User::student
    };

    Lecture lec {
        "aboba",
        "gr",
        "theme",
        "202k",
        DBDate("2025-11-11 10:10")
    };
    Practice prac {
        "aboba1",
        "aboba2",
        "theme",
        "car",
        DBDate("2025-11-11 10:10")
    };



    nlohmann::json j1, j2, j3;
    j3 = {};

    JsonConverter::userToJson(u1, j1);
    JsonConverter::userToJson(u2, j2);

    j3.push_back(j1);
    j3.push_back(j2);

    std::string str;
    JsonConverter::jsonToJsonString(j3, str);



    ///
    qr::questions_resource questions_src;

    ws.register_resource("/questions", &questions_src);
    ws.register_resource("/questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/{question|[0-9]+}", &questions_src);


    ws.start(true);

    return 0;
}
