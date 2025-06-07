#include "appdatabase.h"
#include "authentication_resourses.h"
#include "practices_resources.h"
#include "questions_resources.h"
#include "users_resources.h"
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

    User u1 {
        0,
        "User",
        "Password",
        "Name",
        User::STUDENT
    };

    AppDB();

    ///
    qr::questions_resource questions_src;
    ur::users_resources user_src;
    auth::signUp_resource signUp_src;
    auth::signIn_resource signIn_src;
    pr::practices_resource practices_src;

    ws.register_resource("/questions", &questions_src, true);
    ws.register_resource("/questions/categories/{category}/tickets/amount", &questions_src);
    ws.register_resource("/questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/amount", &questions_src);
    ws.register_resource("/questions/categories/{category}/tickets/{ticket|[0-9]+}/questions/{question|[0-9]+}", &questions_src);

    ws.register_resource("/questions/themes", &questions_src);
    ws.register_resource("/questions/themes/{theme}/questions/ids", &questions_src);
    ws.register_resource("/questions/themes/{theme}/questions/amount", &questions_src);

    ws.register_resource("/users", &user_src, true);
    ws.register_resource("/users/{page|[0-9]+}", &user_src);

    ws.register_resource("/users/signup", &signUp_src);
    ws.register_resource("/users/signin", &signIn_src);

    ws.register_resource("/practices", &practices_src);
    ws.register_resource("/practices/by-student/{student}", &practices_src);
    ws.register_resource("/practices/by-teacher/{teacher}", &practices_src);
    ws.register_resource("/practices/by-time/from/{fromTime|[0-9]+}/to/{toTime|[0-9]+}", &practices_src);

    ws.start(true);

    return 0;
}
