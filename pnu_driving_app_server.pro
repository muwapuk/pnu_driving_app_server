TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG -= qt


LIBS += -lhttpserver -ljsoncpp -lSQLiteCpp

SOURCES += \
        appdatabase.cpp \
        appwebserver.cpp \
        authentication_resourses.cpp \
        jsonconverter.cpp \
        lessons_resources.cpp \
        main.cpp \
        practices_resources.cpp \
        questions_resources.cpp \
        users_resources.cpp

HEADERS += \
    appdatabase.h \
    appdatabase_structs.h \
    appwebserver.h \
    authentication_resourses.h \
    jsonconverter.h \
    lessons_resources.h \
    practices_resources.h \
    questions_resources.h \
    users_resources.h

