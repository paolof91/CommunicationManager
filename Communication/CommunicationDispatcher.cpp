#include <stdio.h>

#include "CommunicationClient.h"
#include "CommunicationDispatcher.h"

CommunicationDispatcher::CommunicationDispatcher(CommunicationClient *client)
    : m_client(client)
{
}

void CommunicationDispatcher::startWork()
{
    pthread_create(&m_thread, NULL, dispatcherThread, (void*) this);
}

void CommunicationDispatcher::doWork()
{
    while(1)
    {
        if ( (m_client!=nullptr) &  (m_client->status() == CONNECTED))
        {
            CommunicationMessage message;
            int ret;
            ret = m_client->receiveMessageIfAvailable(message);
            if (ret==0)
                printf("Received: %s.\n",message.data);
        }
    }
}

void* CommunicationDispatcher::dispatcherThread(void *param)
{
    CommunicationDispatcher *pThis = (CommunicationDispatcher*)param;
    pThis->doWork();
    return param;
}
