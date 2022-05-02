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
    m_queue = new CommunicationMessageQueue(queueSize);

    // Creating socket file descriptor
    if ((m_server_fd = socket(AF_INET, SOCK_STREAM, 0))
        == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    int opt = 1;
    if (setsockopt(m_server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    m_serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, addressString, &(m_serv_addr.sin_addr));
    m_serv_addr.sin_port = htons(port);

    // Forcefully attaching socket to the port 8080
    if (bind(m_server_fd, (struct sockaddr*)&m_serv_addr,
             sizeof(m_serv_addr))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Entering listening\n");

    if (listen(m_server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Launch thread to handle the server state machine
    status = WAITING_FOR_CLIENT_CONNECTION;
    pthread_create(&m_thread, NULL, serverThread, (void*) this);
    pthread_join(m_thread, NULL);
    printf("\n Server thread has joined.\n");
}

CommunicationServer::~CommunicationServer()
{
    delete m_queue;
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
        if(status==WAITING_FOR_CLIENT_CONNECTION)
        {
            waitForClientConnection();
        }
        if(status==CONNECTED)
        {
            sendMessagesFromQueue();

        }
        if(status==TERMINATED)
        {
            printf("\n Exiting\n");
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
        printf("\nError in securing client connection.\n");
    }
    else
    {
        status=CONNECTED;
        printf("\nConnection to client secured.\n");
    }
}

void CommunicationServer::sendMessagesFromQueue()
{
    int valread;
    char hello[] = "Hello from server";
    char buffer[CommunicationMessage::getHeaderAndDataMaxSize()] = { 0 };

    // send is a blocking call (it waits for the server to read, before continuing))
    send(m_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
    valread = read(m_socket, buffer, 1024);
    printf("%s\n", buffer);
    printf("%d\n", valread);

    printf("Server is about to send a CommunicationMessage. \n");
    CommunicationMessage m;
    m.messageType = 0x10;
    char a[] = "ciao";
    memcpy(m.data,a,4);
    m.size = sizeof(a);

    memcpy(buffer,&m,CommunicationMessage::getHeaderSize() + m.size);
    send(m_socket, buffer, CommunicationMessage::getHeaderSize() + m.size, 0);
    printf("\n size of m.data and a are %ld and %ld\n", sizeof(m.data), sizeof(a));
    printf("Server has sent a CommunicationMessage. \n");

    // change the state machine
    status = TERMINATED;
}

/*
 * returns -1 if queue is full, returns 0 if successful
 */
int CommunicationServer::sendMessage(CommunicationMessage message)
{
    return m_queue->enqueue(message);
}

