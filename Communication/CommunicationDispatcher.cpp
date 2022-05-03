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
            {
                dispatch(message);
            }
        }
    }
}

void* CommunicationDispatcher::dispatcherThread(void *param)
{
    CommunicationDispatcher *pThis = (CommunicationDispatcher*)param;
    pThis->doWork();
    return param;
}

void CommunicationDispatcher::dispatch(CommunicationMessage cm)
{
    switch(cm.messageTypeIdentifier)
    {
        case CommunicationProtocol::MTI_A:
            {
                MessageTypeA mA = CommunicationProtocol::decodeMessageTypeA(cm);
                printf("read MessageTypeA: \t(%f, %f)\n", mA.w_f, mA.w_o);
            }
            break;
        case CommunicationProtocol::MTI_B:
            {
                MessageTypeB mB = CommunicationProtocol::decodeMessageTypeB(cm);
                printf("read MessageTypeB: \t(%f, %f, %f, %f)\n", mB.v_x, mB.v_y, mB.v_z, mB.v_o);
            }
            break;
        default:
            {
                printf("read message whose MessageTypeIdentifier is not recognized A\n");
            }
    }
}
