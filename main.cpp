#include "jsonconverter.h"
#include "questions_resources.h"
#include <iostream>
#include <ctime>


void register_resources(webserver &server)
{

}


int main(int, char**)
{
    webserver ws = create_webserver(8080);
    ///


    std::cout << time(0) << std::endl;

    ///
    qr::questions_resource questions_src;

    ws.register_resource("/questions", &questions_src);
    ws.register_resource("/questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/{question|[0-9]+}", &questions_src);


    ws.start(true);

    return 0;
}
