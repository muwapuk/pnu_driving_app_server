#include "appdatabase_structs.h"
#include "authentication_resourses.h"
#include "questions_resources.h"
#include "users_resources.h"
#include "appdatabase.h"
#include <iostream>


// void register_resources(webserver &server)
// {

// }
// class digest_resource : public httpserver::http_resource {
// public:
//     std::shared_ptr<http_response> render_GET(const http_request& req) {
//         if (req.get_digested_user() != "1") {
//             return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL", "test@example.com", MY_OPAQUE, false));
//         }
//         else {
//             std::cout << req.get_digested_user() << std::endl;
//             bool reload_nonce = false;
//             if(!req.check_digest_auth("test@example.com", "mypass", 300, &reload_nonce)) {
//                 return std::shared_ptr<digest_auth_fail_response>(new digest_auth_fail_response("FAIL", "test@example.com", MY_OPAQUE, reload_nonce));
//             }
//         }
//         return std::shared_ptr<string_response>(new string_response("SUCCESS", 200, "text/plain"));
//     }
// };

int main(int, char**)
{
    webserver ws = create_webserver(8080);
    ///


    // digest_resource hwr;
    // ws.register_resource("/hello", &hwr);
    // ws.start(true);

    ///
    qr::questions_resource questions_src;
    ur::users_resources user_src;
    auth::signIn_resource signIn_src;

    ws.register_resource("/questions", &questions_src, true);
    ws.register_resource("/questions/categories/{category}/ticket/{tickets|[0-9]+}/question/{questions|[0-9]+}", &questions_src);

    ws.register_resource("/users", &user_src, true);
    ws.register_resource("/users/{page|[0-9]+}", &user_src);

    ws.register_resource("/users/signin/", &signIn_src);


    ws.start(true);

    return 0;
}
