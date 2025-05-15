#include "appdatabase.h"
#include "jsonconverter.h"
#include "questions_resources.h"
#include "users_resources.h"


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
        DBData("2025-11-11 10:10")
    };
    Practice prac {
        "aboba1",
        "aboba2",
        "theme",
        "car",
        DBData("2025-11-11 10:10")
    };

    AppDB()->addPractice(prac);
    auto l = AppDB()->getPracticesByStudent("aboba1");
    auto ll = AppDB()->getPracticesByTeacher("aboba1");
    auto lll = AppDB()->getPracticesByStudent("aboba2");
    auto llll = AppDB()->getPracticesByTeacher("aboba2");



    ///
    qr::questions_resource questions_src;

    ws.register_resource("/questions", &questions_src);
    ws.register_resource("/questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+}", &questions_src);


    ws.start(true);

    return 0;
}
