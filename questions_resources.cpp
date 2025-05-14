#include "questions_resources.h"
#include <iostream>

using namespace qr;
/////////////////////////
// QUESTIONS RESOURCES //
/////////////////////////

std::map <std::string, Categories> categories_strings = {
    {"AB", AB},
    {"CD", CD}
};

// .../questions/category/{category}/ticket/{ticket|[0-9]+}/question/{question|[0-9]+}?attribute={"..."}
std::shared_ptr<http_response>
questions_resource::render_GET(const http_request &req) {
    Categories category = categories_strings[std::string(req.get_arg("category"))];
    int ticket_num = std::stoi(std::string(req.get_arg("ticket")));
    int question_num = std::stoi(std::string(req.get_arg("question")));

    switch (category) {
    case AB:

        break;
    case CD:

        break;
    default:
        return std::shared_ptr<http_response>(new string_response("Category not found", 404));
    }



    std::string(req.get_arg("category"));
    req.get_arg("ticket");
    req.get_arg("question");


    return std::shared_ptr<http_response>(
        new string_response("Hello: " + std::string(req.get_arg("name"))));
}

std::shared_ptr<http_response>
questions_resource::render_POST(const http_request &req) {
  return std::shared_ptr<http_response>(
      new string_response("Hello: " + std::string(req.get_arg("name"))));
}

std::shared_ptr<http_response>
questions_resource::render_PUT(const http_request &req) {
    return std::shared_ptr<http_response>(
        new string_response("Hello: " + std::string(req.get_arg("name"))));
}

std::shared_ptr<http_response>
questions_resource::render_PATCH(const http_request &req) {
    return std::shared_ptr<http_response>(
        new string_response("Hello: " + std::string(req.get_arg("name"))));
}

std::shared_ptr<http_response>
questions_resource::render(const http_request &req) {
  return std::shared_ptr<http_response>(
      new string_response("Hello: " + std::string(req.get_arg("name"))));
}

/////////////////////////////////
// QUESTIONS RESULTS RESOURCES //
/////////////////////////////////

std::shared_ptr<http_response>
questions_results_resource::render_GET(const http_request &req)
{
    return std::shared_ptr<http_response>(
        new string_response("Hello: " + std::string(req.get_arg("name"))));
}

std::shared_ptr<http_response>
questions_results_resource::render_PUT(const http_request &req)
{
    return std::shared_ptr<http_response>(
        new string_response("Hello: " + std::string(req.get_arg("name"))));
}

std::shared_ptr<http_response>
questions_results_resource::render(const http_request &req)
{
    return std::shared_ptr<http_response>(
        new string_response("Hello: " + std::string(req.get_arg("name"))));
}
