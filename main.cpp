#include "appdatabase.h"
#include "authentication_resourses.h"
#include "lectures_resources.h"
#include "practices_resources.h"
#include "testing_resources.h"
#include "users_resources.h"
#include <iostream>

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

    // Authentication resources
    auth::signUp_resource signUp_src;
    auth::signIn_resource signIn_src;

    ws.register_resource("/users/signup", &signUp_src);
    ws.register_resource("/users/signin", &signIn_src);

    // Question resources
    qr::subjects_resource subjects_res;
    qr::questions_by_subject_resource questions_subj_res;
    qr::tickets_by_category_resource tickets_cat_res;
    qr::questions_by_ticket_resource questions_ticket_res;
    qr::question_by_id_resource question_id_res;
    qr::create_ticket_resource create_ticket_res;
    qr::create_question_resource create_question_res;
    qr::delete_ticket_resource delete_ticket_res;

    // Get questions subjects
    ws.register_resource("/testing/subjects", &subjects_res); // -> JSON [subject1, subject2...]

    // Get questions number and id pairs from specific {subject}
    ws.register_resource("/testing/subjects/{subject}/questions-list", &questions_subj_res); // GET -> JSON [{id, num}, id, num},...]

    // Get ticket number and id pairs from specific {category}
    ws.register_resource("/testing/categories/{category}/tickets", &tickets_cat_res); // GET -> JSON [{id, num}, id, num},...]

    // Get questions number and id pair from specific {ticket}
    ws.register_resource("/testing/tickets/by-id/{ticket-id}/questions-list", &questions_ticket_res); // GET -> JSON [{id, num}, id, num},...]

    // Get or delete specific {question_id}
    ws.register_resource("/testing/questions/by-id/{question-id}", &question_id_res); // GET/PATCH/DELETE -> JSON {question}

    // Create ticket in {category} with {number}
    ws.register_resource("/testing/categories/{category}/tickets/{number}", &create_ticket_res); // PUT

    // Create question in ticket with subject in json body
    ws.register_resource("/testing/questions", &create_question_res); // PUT <- JSON {question}

    // Delete {ticket}
    ws.register_resource("/testing/tickets/by-id/{ticket-id}", &delete_ticket_res); // DELETE

    // Practice resources
    pr::student_slots_resource student_slots_res;
    pr::teacher_slots_resource teacher_slots_res;
    pr::create_slot_resource create_slot_res;
    pr::create_booking_resource create_booking_res;
    pr::delete_slot_resource delete_slot_res;
    pr::delete_booking_resource delete_booking_res;

    ws.register_resource("/practices/by-student", &student_slots_res);
    ws.register_resource("/practices/by-teacher", &teacher_slots_res);
    ws.register_resource("/practices/slots", &create_slot_res);
    ws.register_resource("/practices/bookings", &create_booking_res);
    ws.register_resource("/practices/slots/{slot_id}", &delete_slot_res);
    ws.register_resource("/practices/bookings/{booking_id}", &delete_booking_res);

    // Lecture resources
    lc::lectures_by_teacher_resource lectures_teacher_res;
    lc::lectures_by_group_resource lectures_group_res;
    lc::create_lecture_resource create_lecture_res;
    lc::delete_lecture_resource delete_lecture_res;

    ws.register_resource("/lectures/by-teacher/{teacher_id}", &lectures_teacher_res);
    ws.register_resource("/lectures/by-group/{group_id}", &lectures_group_res);
    ws.register_resource("/lectures", &create_lecture_res);
    ws.register_resource("/lectures/{lecture_id}", &delete_lecture_res);
    ws.start(true);


    return 0;
}
