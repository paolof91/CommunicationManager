#include <stdio.h>

#include "CommunicationClient.h"
#include "CommunicationDispatcher.h"

CommunicationDispatcher::CommunicationDispatcher(CommunicationClient *client)
    : m_client(client)
{
}

void CommunicationDispatcher::doWork()
{
    while(1)
    {
        if (m_client==nullptr)
            return;
        if (m_client->status() != CONNECTED)
            return;
        CommunicationMessage message;
        m_client->receiveAvailableMessage(message);
        printf("Received: %s.\n",message.data);
        // CommunicationProtocol::decode
    }
}
