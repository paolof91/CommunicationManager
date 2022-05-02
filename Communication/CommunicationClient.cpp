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

#include "CommunicationClient.h"
#include "CommunicationProtocol.h"

CommunicationClient::CommunicationClient(const char *addressString, const unsigned short port)
{
    // opening socket
    m_socket = 0;
    if ((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        exit(EXIT_FAILURE);
    }

    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, addressString, &m_serv_addr.sin_addr)
        <= 0)
    {
        printf(
            "\nInvalid address/ Address not supported \n");
        exit(EXIT_FAILURE);
    }

    // Launch thread to handle the client state machine
    status = WAITING_FOR_SERVER_SOCKET;
    pthread_create(&m_thread, NULL, clientThread, (void*) this);
    pthread_join(m_thread, NULL);
    printf("\n Client thread has joined.\n");
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
        if(status==WAITING_FOR_SERVER_SOCKET)
        {
            waitForServerSocket();
        }
        if(status==CONNECTED)
        {
            receiveAndStoreMessages();
        }
        if(status==TERMINATED)
        {
            printf("\n Exiting\n");
            break;
        }
    }
}

void CommunicationClient::waitForServerSocket()
{
    // connect is not a blocking call
    if (connect(m_socket, (struct sockaddr*)&m_serv_addr, sizeof(m_serv_addr))
        < 0)
    {
        printf("\nServer not connected. Retrieving connection in 1s...\n");
        sleep(1);
    }
    else
    {
        status=CONNECTED;
        printf("\nConnection to server secured.\n");
    }
}

void CommunicationClient::receiveAndStoreMessages()
{
    int valread;
    char buffer[CommunicationMessage::getHeaderAndDataMaxSize()] = { 0 };
    char hello[] = "Hello from server";

    // read is a blocking call (it waits for the client to send something, before continuing)
    valread = read(m_socket, buffer, 1024);
    printf("%s\n", buffer);
    printf("%d\n", valread);
    send(m_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");

    printf("Client is about to receive a CommunicationMessage. \n");

    CommunicationMessage cm;
    valread = read(m_socket, buffer, CommunicationMessage::getHeaderSize());
    memcpy(&cm,buffer,CommunicationMessage::getHeaderSize());
    printf("CommunicationMessage messageType: %d\n",cm.messageType);
    printf("CommunicationMessage data size: %d\n",cm.size);
    valread = read(m_socket, buffer, cm.size);
    memcpy(&(cm.data),buffer,cm.size);
    printf("CommunicationMessage actual data: %s\n",cm.data);

    // change the state machine
    status = TERMINATED;
}
