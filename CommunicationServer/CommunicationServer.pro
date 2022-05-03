TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += \
        ../Communication/CommunicationMessageQueue.cpp \
        ../Communication/CommunicationServer.cpp \
        ../Communication/CommunicationSender.cpp \
        main.cpp

HEADERS += \
    ../Communication/CommunicationMessageQueue.h \
    ../Communication/CommunicationProtocol.h \
    ../Communication/CommunicationServer.h \
    ../Communication/CommunicationSender.h
