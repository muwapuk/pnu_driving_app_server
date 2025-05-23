#include "appwebserver.h"
#include <iostream>

AppWebserver::AppWebserver() {}

void AppWebserver::printDebug(const httpserver::http_request &req)
{
    std::cout << req.get_requestor() << '\n';
    std::cout << req.get_user() << '\n';
    std::cout << req.get_pass() << '\n';

    for (auto &str : req.get_headers()) {
        std::cout << std::string(str.first) << ": " << std::string(str.second) << '\n';
    }
    std::cout << req.get_content() << '\n';
    std::cout << req.get_content().size() << std::endl;
}
