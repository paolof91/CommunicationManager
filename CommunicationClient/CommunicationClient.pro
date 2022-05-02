TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += \
        ../Communication/CommunicationClient.cpp \
        ../Communication/CommunicationDispatcher.cpp \
        ../Communication/CommunicationMessageQueue.cpp \
        main.cpp

HEADERS += \
    ../Communication/CommunicationClient.h \
    ../Communication/CommunicationDispatcher.h \
    ../Communication/CommunicationMessageQueue.h \
    ../Communication/CommunicationProtocol.h \
    ../Communication/MessageTypes.h
