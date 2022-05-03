#ifndef COMMUNICATIONSENDER_H
#define COMMUNICATIONSENDER_H

#include <pthread.h>
#include "CommunicationProtocol.h"

class CommunicationServer;

class CommunicationSender
{
public:
    CommunicationSender(CommunicationServer *server);
    void sendMessageTypeA(MessageTypeA m);
    void sendMessageTypeB(MessageTypeB m);
    bool isWorking();

private:
    CommunicationServer *m_server;
};

#endif // COMMUNICATIONSENDER_H
