/*
 * MIT License
 *
 * Copyright (c) 2022 Paolo Forni
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "CommunicationMessageQueue.h"
#include "CommunicationClient.h"

CommunicationClient::CommunicationClient(const char *addressString, uint16_t port, uint16_t queueSize)
{
    // Creating message queue
    if ( queueSize == 0 )
    {
        perror("Error: Server queue size is 0");
        exit(EXIT_FAILURE);
    }
    m_queue = new CommunicationMessageQueue(queueSize);

    // opening socket
    setSocketAddress(addressString,port);
    openSocket();

    // Launch thread to handle the client state machine
    pthread_create(&m_thread, NULL, clientThread, (void*) this);
}

CommunicationClient::~CommunicationClient()
{
    pthread_join(m_thread, NULL);
    delete m_queue;
}

void CommunicationClient::openSocket()
{
    m_socket = 0;
    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }
    m_status = WAITING_FOR_SERVER_CONNECTION;
}

void CommunicationClient::setSocketAddress(const char *addressString, uint16_t port)
{
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, addressString, &m_serv_addr.sin_addr)
        <= 0)
    {
        printf(
            "\nClient socket, Invalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }
}

void* CommunicationClient::clientThread(void *param)
{
    CommunicationClient *pThis = (CommunicationClient*)param;
    pThis->clientStateMachineHandler();
    return param;
}

void CommunicationClient::clientStateMachineHandler()
{
    while(1)
    {
        if(m_status==WAITING_FOR_SERVER_CONNECTION)
        {
            waitForServerSocket();
        }
        if(m_status==CONNECTED)
        {
            receiveAndStoreMessages();
        }
        if(m_status==MUST_RESTORE_CONNECTION)
        {
            close(m_socket);
            openSocket();
        }
        if(m_status==TERMINATED)
        {
            close(m_socket);
            printf("\n Client %d connection closed..\n",m_socket);
            break;
        }
    }
}

void CommunicationClient::waitForServerSocket()
{
    // connect is NOT a blocking call
    if (connect(m_socket, (struct sockaddr*)&m_serv_addr, sizeof(m_serv_addr))
        < 0)
    {
        printf("\nServer not connected. Retrieving connection in 1s...\n");
        sleep(1);
    }
    else
    {
        m_status = CONNECTED;
        printf("\nConnection to server secured.\n");
    }
}

void CommunicationClient::receiveAndStoreMessages()
{
    uint32_t valread, valread_temp;
    CommunicationMessage message;

    // read is a blocking call (it waits for the client to send something, before continuing)
    valread = read(m_socket, m_buffer, CommunicationMessage::getHeaderSize());
    if (valread<=0)
    {
        handleReadingError(valread,"message header");
        return;
    }

    while( valread < CommunicationMessage::getHeaderSize() )
    {
        valread_temp = read(m_socket, m_buffer+valread, CommunicationMessage::getHeaderSize()-valread);
        if (valread_temp <= 0)
        {
            handleReadingError(valread,"message header");
            return;
        }
        valread += valread_temp;
    }

    // message header is complete, let's copy it in message and read message.size
    memcpy(&message,m_buffer,CommunicationMessage::getHeaderSize());

    valread = read(m_socket, m_buffer, message.size);
    if (valread<=0)
    {
        handleReadingError(valread,"message data");
        return;
    }

    while( valread < message.size)
    {
        valread_temp = read(m_socket, m_buffer+valread, message.size-valread);
        if (valread_temp<= 0)
        {
            handleReadingError(valread,"message data");
            return;
        }
        valread += valread_temp;
    }

    // message data is complete, let's copy it in message and enqueue it
    memcpy(&(message.data),m_buffer,message.size);

    pthread_mutex_lock( &m_queueMutex );
    m_queue->enqueue(message);
    pthread_mutex_unlock( &m_queueMutex );
}

/*
 * returns -1 if no message in the queue, otherwise returns 0
 */
int CommunicationClient::receiveMessageIfAvailable(CommunicationMessage &message)
{
    int ret = 0;

    pthread_mutex_lock( &m_queueMutex );
    bool isEmpty = m_queue->isEmpty();
    pthread_mutex_unlock( &m_queueMutex );

    if (!isEmpty)
    {
        pthread_mutex_lock( &m_queueMutex );
        ret = m_queue->dequeue(message);
        pthread_mutex_unlock( &m_queueMutex );
    }
    else
        ret = -1;

    return ret;
}

void CommunicationClient::handleReadingError(int valread, const char* source)
{
    if (valread<0)
    {
        printf("Unsuccessful read from client socket: [%s] reading failed.\n", source);
        m_status = MUST_RESTORE_CONNECTION;
        return;
    }
    if (valread == 0)
    {
        printf("read() in [%s] has returned 0 bytes. Connection has possibly been closed on the other side.\n", source);
        m_status = MUST_RESTORE_CONNECTION;
        return;
    }
}
