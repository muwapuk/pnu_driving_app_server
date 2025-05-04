TEMPLATE = app
CONFIG += console c++20
CONFIG -= app_bundle
CONFIG -= qt


LIBS += -lhttpserver -ljsoncpp -lSQLiteCpp

SOURCES += \
        appdatabase.cpp \
        main.cpp \
        tickets_resources.cpp

HEADERS += \
    appdatabase.h \
    tickets_resources.h

