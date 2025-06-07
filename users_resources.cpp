// #include "users_resources.h"
// #include "appdatabase.h"
// #include "authentication_resourses.h"
// #include "jsonconverter.h"

// #define HEADER_RESPONSE

// using json = nlohmann::json;
// using namespace ur;

// int MAX_JSON_ARRAY_SIZE = 10;

// namespace ur {
// // Used pass string arguments in switch expression

// std::map <std::string, UserAttributes> user_attributes = {
//     {"name", NAME},
//     {"permissions", PERMISSIONS},
//     {"", BLANK}
// };

// }

// std::shared_ptr<http_response> users_resources::getUser(std::string login)
// {
//     std::string userString;
//     json j;
//     auto user = AppDB().getUser(login);
//     if (!user)
//         return std::shared_ptr<http_response>(new string_response("User does not exist!", 404));

//     JsonConverter::userToJson(*user, j);
//     JsonConverter::jsonObjectToJsonString(j, userString);

//     auto response = std::shared_ptr<string_response>(new string_response("SUCCESS"));
// #ifdef HEADER_RESPONSE
//     response->with_header("user", userString);
// #else
//     response->with_footer("user", userString);
// #endif
//     return response;
// }

// std::shared_ptr<http_response> users_resources::getUsersPage(int page)
// {
//     std::string userArrayString;
//     json j_user, j_userArray = {};
//     auto users = AppDB().getUsersList(1 + (page-1)*MAX_JSON_ARRAY_SIZE, MAX_JSON_ARRAY_SIZE);

//     for (auto &user : *users) {
//         JsonConverter::userToJson(user, j_user);
//         j_userArray.push_back(j_user);
//     }

//     JsonConverter::jsonObjectToJsonString(j_userArray, userArrayString);

//     auto response = std::shared_ptr<string_response>(new string_response("SUCCESS"));

// #ifdef HEADER_RESPONSE
//     response->with_header("users", userArrayString);
// #else
//     response->with_footer("users", userArrayString);
// #endif

//     return response;
// }

// std::shared_ptr<http_response> users_resources::getUserAttribute(std::string login, UserAttributes attribute)
// {
//     std::string userString;
//     json j_response;

//     if (attribute == NAME) {
//         auto name = AppDB().getUserName(login);
//         if (!name) {
//             return std::shared_ptr<http_response>(new string_response("User does not exist!", 404));
//         }
//         j_response["name"] = *name;
//     } else if (attribute == PERMISSIONS){
//         auto perm = AppDB().getUserPermissions(login);
//         if (perm == User::NONE) {
//             return std::shared_ptr<http_response>(new string_response("User does not exist!", 404));
//         }
//         j_response["permissions"] = perm;
//     }
//     JsonConverter::jsonObjectToJsonString(j_response, userString);

//     auto response = std::shared_ptr<string_response>(new string_response("SUCCESS"));

// #ifdef HEADER_RESPONSE
//     response->with_header("user", userString);
// #else
//     response->with_footer("user", userString);
// #endif
//     return response;
// }

// std::shared_ptr<http_response> users_resources::getUsersAttributesPage(UserAttributes attribute, int page)
// {
//     std::string userArrayString;
//     json j_user, j_userArray = {};

//     if (attribute == NAME) {
//         auto users = AppDB().getUsersName(1 + (page-1)*MAX_JSON_ARRAY_SIZE, MAX_JSON_ARRAY_SIZE);
//         for (auto &user : *users) {
//             j_user["login"] = user.first;
//             j_user["names"] = user.second;
//             j_userArray.push_back(j_user);
//         }
//     } else if (attribute == PERMISSIONS){
//         auto users = AppDB().getUsersPermissions(1 + (page-1)*MAX_JSON_ARRAY_SIZE, MAX_JSON_ARRAY_SIZE);
//         for (auto &user : *users) {
//             j_user["login"] = user.first;
//             j_user["permissions"] = static_cast<int>(user.second);
//             j_userArray.push_back(j_user);
//         }
//     }
//     JsonConverter::jsonObjectToJsonString(j_userArray, userArrayString);

//     auto response = std::shared_ptr<string_response>(new string_response("SUCCESS"));

// #ifdef HEADER_RESPONSE
//     response->with_header("users", userArrayString);
// #else
//     response->with_footer("users", userArrayString);
// #endif
//     return response;
// }

// ////////////////////
// // USER RESOURSES //
// ////////////////////

// // .../users/{page|[0-9]+}?login="..."&attribute="..." -> json
// std::shared_ptr<http_response> users_resources::render_GET(const http_request &req)
// {
//     std::shared_ptr<http_response> response;
//     int page = 1;

//     // std::string token = std::string(req.get_header("token"));
//     // auto userAndPerms = auth::tokenToUserAndPermenissions(token);
//     // if (userAndPerms == nullptr || userAndPerms->second == User::NONE) {
//     //     return std::shared_ptr<http_response>(new string_response("Authorization failed!", 401));
//     // }

//     if (!req.get_arg("page").values.empty()) {
//         page = std::stoi(std::string(req.get_arg("page")));
//         if(page == 0) page = 1;
//     }

//     UserAttributes attribute = user_attributes[std::string(req.get_arg("attribute"))];

//     if (attribute == UNDEFINED)
//         return std::shared_ptr<http_response>(new string_response("Incorrect attribute!", 400));

//     // No login
//     if (req.get_arg("login").values.empty()) {
//         // Perm check
//         // if (userAndPerms->second != User::SUPERUSER)
//         //     return std::shared_ptr<http_response>(new string_response("No perminission to access resource!", 403));
//         // No attribute
//         if (attribute == BLANK) {
//             response = getUsersPage(page);
//         // Attribute given
//         } else {
//             response = getUsersAttributesPage(attribute, page);
//         }
//     // Login given
//     } else {
//         // Perm check
//         // if (userAndPerms->second != User::SUPERUSER &&
//         //     userAndPerms->first != std::string(req.get_arg("login"))
//         // ) return std::shared_ptr<http_response>(new string_response("No perminission to access resource!", 403));
//         // No attribute
//         if (attribute == BLANK) {
//             response = getUser(req.get_arg("login"));
//         // Attribute given
//         } else {
//             response = getUserAttribute(req.get_arg("login"), attribute);
//         }
//     }

//     return response;
// }

// std::shared_ptr<http_response> users_resources::render_PATCH(const http_request &req)
// {

// }

// std::shared_ptr<http_response> users_resources::render(const http_request &)
// {
//     return std::shared_ptr<http_response>(new string_response("Bad request!", 400));
// }
