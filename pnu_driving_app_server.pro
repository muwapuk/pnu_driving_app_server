TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG -= qt


LIBS += -lhttpserver -ljsoncpp -lSQLiteCpp

SOURCES += \
        appdatabase.cpp \
        jsonconverter.cpp \
        main.cpp \
        tickets_resources.cpp

HEADERS += \
    appdatabase.h \
    appdatabase_structs.h \
    jsonconverter.h \
    tickets_resources.h

