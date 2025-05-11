TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG -= qt


LIBS += -lhttpserver -ljsoncpp -lSQLiteCpp

SOURCES += \
        appdatabase.cpp \
        json_converter.cpp \
        main.cpp \
        tickets_resources.cpp

HEADERS += \
    appdatabase.h \
    appdatabase_structs.h \
    json_converter.h \
    tickets_resources.h

