#include "../Communication/CommunicationProtocol.h"
#include "../Communication/CommunicationServer.h"

int main()
{
    CommunicationServer server(CommunicationProtocol::PRESENTER_IP_ADDRESS,
                               CommunicationProtocol::PRESENTER_IP_PORT,
                               CommunicationProtocol::PRESENTER_MESSAGE_QUEUE_SIZE);
    return 0;
}
