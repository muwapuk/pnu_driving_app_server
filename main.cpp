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

    Question q{
        Question::AB,
        1,
        1,
        "a",
        "b",
        "c",
        "d",
        2
    };
    AppDB()->addQuestion(q);

    qr::questions_resource questions_src;

    ws.register_resource("/questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+}", &questions_src);
    ws.register_resource("/questions", &questions_src);


    ws.start(true);

    return 0;
}
