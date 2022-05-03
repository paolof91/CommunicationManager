#ifndef COMMUNICATIONPROTOCOL_H
#define COMMUNICATIONPROTOCOL_H

#include <string.h>
#include <stdint.h>

#include "MessageTypes.h"

struct CommunicationMessage;

namespace CommunicationProtocol
{
    /*
     * For each different connection,
     * put IP addresses, ports, and message queue size on both communication sides
     */
    static constexpr char PRESENTER_IP_ADDRESS[] = "127.0.0.1";
    static constexpr uint16_t PRESENTER_IP_PORT = 1024;
    static constexpr uint16_t PRESENTER_SERVER_MESSAGE_QUEUE_SIZE = 16;
    static constexpr uint16_t PRESENTER_CLIENT_MESSAGE_QUEUE_SIZE = 16;

    /*
     * These constants define the size of the char[] buffer
     * by means of which sockets reads and sends are carried out
     */
    static constexpr uint16_t MESSAGE_DATA_MAX_SIZE = 1024;
    static constexpr uint16_t MESSAGE_HEADER_AND_DATA_MAX_SIZE = 2*1024;
}

struct CommunicationMessage
{
    uint32_t messageTypeIdentifier;
    uint32_t size;
    char data[CommunicationProtocol::MESSAGE_DATA_MAX_SIZE];

    static uint32_t getHeaderSize()
    {
        return sizeof(messageTypeIdentifier)+sizeof(size);
    }

    static uint32_t getHeaderAndDataMaxSize()
    {
        return getHeaderSize()+CommunicationProtocol::MESSAGE_DATA_MAX_SIZE;
    }
};

namespace CommunicationProtocol
{
    /*
     * For each type of message:
     * - put its unique Message Type Identifier here;
     * - write an encoder and decoder function
     * - add it to the map (identifier,encoder) and the map (identifier,decoder)
     * The maximum number of different message types goes in the first line
     * and corresponds to the maximum size of the array that maps
     * message types to encode and decode functions.
     */
    static constexpr uint16_t MTIs_MAX_NB = 1024;
    static constexpr uint16_t MTI_A = 0x01;
    static constexpr uint16_t MTI_B = 0x02;

    static CommunicationMessage encodeMessageTypeA(MessageTypeA m)
    {
        CommunicationMessage cm;
        cm.messageTypeIdentifier = MTI_A;
        cm.size = sizeof(MessageTypeA);
        memcpy(&(cm.data), &m, cm.size);
        return cm;
    };

    static MessageTypeA decodeMessageTypeA(CommunicationMessage cm)
    {
        MessageTypeA m;
        memcpy(&m, &(cm.data), cm.size);
        return m;
    };

    static CommunicationMessage encodeMessageTypeB(MessageTypeB m)
    {
        CommunicationMessage cm;
        cm.messageTypeIdentifier = MTI_B;
        cm.size = sizeof(MessageTypeB);
        memcpy(&(cm.data), &m, cm.size);
        return cm;
    };

    static MessageTypeB decodeMessageTypeB(CommunicationMessage cm)
    {
        MessageTypeB m;
        memcpy(&m, &(cm.data), cm.size);
        return m;
    };
};

#endif // COMMUNICATIONPROTOCOL_H
