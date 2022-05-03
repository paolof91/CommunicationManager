#include "CommunicationServer.h"
#include "CommunicationSender.h"

CommunicationSender::CommunicationSender(CommunicationServer *server)
    : m_server(server)
{
}

void CommunicationSender::sendMessageTypeA(MessageTypeA m)
{
    if ( !isWorking() )
        return;
    CommunicationMessage cm = CommunicationProtocol::encodeMessageTypeA(m);
    m_server->sendMessage(cm);
}

void CommunicationSender::sendMessageTypeB(MessageTypeB m)
{
    if ( !isWorking() )
        return;
    CommunicationMessage cm = CommunicationProtocol::encodeMessageTypeB(m);
    m_server->sendMessage(cm);
}

bool CommunicationSender::isWorking()
{
    if (m_server==nullptr)
        return false;
    if (m_server->status()!=CONNECTED)
        return false;
    return true;
}
