#include "../Communication/CommunicationProtocol.h"
#include "../Communication/CommunicationClient.h"

int main()
{
    CommunicationClient client(CommunicationProtocol::PRESENTER_IP_ADDRESS,
                               CommunicationProtocol::PRESENTER_IP_PORT);
    return 0;
}
