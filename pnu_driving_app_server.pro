TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG -= qt


LIBS += -lhttpserver -ljsoncpp -lSQLiteCpp

SOURCES += \
        appdatabase.cpp \
        authentication.cpp \
        jsonconverter.cpp \
        main.cpp \
        questions_resources.cpp \
        users_resources.cpp

HEADERS += \
    appdatabase.h \
    appdatabase_structs.h \
    authentication.h \
    jsonconverter.h \
    questions_resources.h \
    users_resources.h

