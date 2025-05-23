#ifndef APPWEBSERVER_H
#define APPWEBSERVER_H

#include <httpserver.hpp>

class AppWebserver
{
public:
    AppWebserver();

    static void printDebug(const httpserver::http_request &req);
};

#endif // APPWEBSERVER_H
