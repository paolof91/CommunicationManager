TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += \
        ../Communication/CommunicationClient.cpp \
        ../Communication/CommunicationMessageQueue.cpp \
        ../Communication/CommunicationProtocol.cpp \
        main.cpp

HEADERS += \
    ../Communication/CommunicationClient.h \
    ../Communication/CommunicationMessageQueue.h \
    ../Communication/CommunicationProtocol.h
