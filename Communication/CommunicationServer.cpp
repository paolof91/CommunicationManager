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
#include "CommunicationServer.h"

CommunicationServer::CommunicationServer(const char *addressString, uint16_t port, uint16_t queueSize)
{
    // Creating message queue
    if ( queueSize == 0 )
    {
        perror("Error: Server queue size is 0");
        exit(EXIT_FAILURE);
    }
    m_queue = new CommunicationMessageQueue(queueSize);

    // Creating socket file descriptor
    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        perror("Error: server socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(m_server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("Error: server setsockopt failure");
        exit(EXIT_FAILURE);
    }
    m_serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, addressString, &(m_serv_addr.sin_addr));
    m_serv_addr.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(m_server_fd, (struct sockaddr*)&m_serv_addr,
             sizeof(m_serv_addr))
        < 0) {
        perror("Error: server bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(m_server_fd, 3) < 0) {
        perror("Error: server listen failure");
        exit(EXIT_FAILURE);
    }

    // Launch thread to handle the server state machine
    m_status = WAITING_FOR_CLIENT_CONNECTION;
    pthread_create(&m_thread, NULL, serverThread, (void*) this);
}

CommunicationServer::~CommunicationServer()
{
    pthread_join(m_thread, NULL);
    delete m_queue;

    // Closing server
    close(m_server_fd);
    printf("\n Server closed.\n");
}

void* CommunicationServer::serverThread(void *param)
{
    CommunicationServer *pThis = (CommunicationServer*)param;
    pThis->serverStateMachineHandler();
    return param;
}

void CommunicationServer::serverStateMachineHandler()
{
    while(1)
    {
        if(m_status==WAITING_FOR_CLIENT_CONNECTION)
        {
            waitForClientConnection();
        }
        if(m_status==CONNECTED)
        {
            sendMessagesFromQueue();
        }
        if(m_status==TERMINATED)
        {
            close(m_socket);
            printf("\n Client %d connection closed..\n",m_socket);
            break;
        }
    }
}

void CommunicationServer::waitForClientConnection()
{
    // accept() is a blocking call
    printf("\nWaiting for client connection...\n");
    int addrlen = sizeof(m_serv_addr);
    if ((m_socket = accept(m_server_fd, (struct sockaddr*)&m_serv_addr,
                  (socklen_t*)&addrlen)) < 0)
    {
        printf("Error in securing client connection.\n");
    }
    else
    {
        m_status = CONNECTED;
        printf("\nConnection to client secured.\n");
    }
}

void CommunicationServer::sendMessagesFromQueue()
{
    uint32_t valsent;
    pthread_mutex_lock( &m_queueMutex );
    bool isEmpty = m_queue->isEmpty();
    pthread_mutex_unlock( &m_queueMutex );

    if (!isEmpty)
    {
        CommunicationMessage m;
        pthread_mutex_lock( &m_queueMutex );
        m_queue->dequeue(m);
        pthread_mutex_unlock( &m_queueMutex );
        memcpy(m_buffer,&m,CommunicationMessage::getHeaderSize() + m.size);

        // send is a blocking call (it waits for the server to read, before continuing))
        valsent = send(m_socket, m_buffer, CommunicationMessage::getHeaderSize() + m.size, 0);
        printf("\nValsent: %d.\n", valsent);
    }
}

/*
 * returns -1 if queue is full, returns 0 if successful
 */
int CommunicationServer::sendMessage(CommunicationMessage message)
{
    int ret = 0;
    pthread_mutex_lock( &m_queueMutex );
    ret = m_queue->enqueue(message);
    pthread_mutex_unlock( &m_queueMutex );
    return ret;
}
