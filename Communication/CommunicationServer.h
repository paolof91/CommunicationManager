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

#ifndef COMMUNICATIONSERVER_H
#define COMMUNICATIONSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>

#include "../Communication/CommunicationProtocol.h"

class CommunicationMessageQueue;

enum CommunicationServerStatus
{
    WAITING_FOR_CLIENT_CONNECTION,
    CONNECTED,
    TERMINATED,
};

class CommunicationServer
{
public:
    CommunicationServer(const char *addressString, uint16_t port, uint16_t queueSize);
    ~CommunicationServer();
    int sendMessage(CommunicationMessage message);
    void terminateConnection() { m_status = TERMINATED; };
    CommunicationServerStatus status() { return m_status; }

private:
    static void* serverThread(void *param);
    void serverStateMachineHandler();
    void waitForClientConnection();
    void sendMessagesFromQueue();

    struct sockaddr_in m_serv_addr;
    int m_server_fd;
    int m_socket;
    pthread_t m_thread;
    pthread_mutex_t m_queueMutex = PTHREAD_MUTEX_INITIALIZER;

    CommunicationMessageQueue* m_queue;
    CommunicationServerStatus m_status;

    char m_buffer[CommunicationProtocol::MESSAGE_HEADER_AND_DATA_MAX_SIZE] = { 0 };
};

#endif // COMMUNICATIONSERVER_H
