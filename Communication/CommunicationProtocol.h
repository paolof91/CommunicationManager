#ifndef COMMUNICATIONPROTOCOL_H
#define COMMUNICATIONPROTOCOL_H

#include <stdint.h>

class CommunicationProtocol
{
public:
    CommunicationProtocol();

    static constexpr char PRESENTER_IP_ADDRESS[] = "127.0.0.1";
    static constexpr uint16_t PRESENTER_IP_PORT = 1024;
    static constexpr uint16_t MESSAGE_DATA_MAX_SIZE = 1024;
    static constexpr uint16_t PRESENTER_MESSAGE_QUEUE_SIZE = 16;
};

struct CommunicationMessage
{
    uint32_t messageType;
    uint32_t size;
    char data[CommunicationProtocol::MESSAGE_DATA_MAX_SIZE];

    static uint32_t getHeaderSize()
    {
        return sizeof(messageType)+sizeof(size);
    }

    static uint32_t getHeaderAndDataMaxSize()
    {
        return getHeaderSize()+CommunicationProtocol::MESSAGE_DATA_MAX_SIZE;
    }
};

#endif // COMMUNICATIONPROTOCOL_H
