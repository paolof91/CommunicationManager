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

#ifndef COMMUNICATIONCLIENT_H
#define COMMUNICATIONCLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <pthread.h>

#include "CommunicationProtocol.h"

class CommunicationMessageQueue;

enum CommunicationClientStatus
{
    WAITING_FOR_SERVER_CONNECTION,
    CONNECTED,
    MUST_RESTORE_CONNECTION,
    TERMINATED,
};

class CommunicationClient
{
public:
    CommunicationClient(const char *addressString, uint16_t port, uint16_t queueSize);
    ~CommunicationClient();
    void terminateConnection() { m_status = TERMINATED; };
    int receiveMessageIfAvailable(CommunicationMessage &message);
    CommunicationClientStatus status() { return m_status; }

private:
    void openSocket();
    void setSocketAddress(const char *addressString, uint16_t port);
    static void* clientThread(void *param);
    void clientStateMachineHandler();
    void waitForServerSocket();
    void receiveAndStoreMessages();
    void handleReadingError(int valread, const char* source);

    struct sockaddr_in m_serv_addr;
    int m_socket;
    pthread_t m_thread;
    pthread_mutex_t m_queueMutex = PTHREAD_MUTEX_INITIALIZER;

    CommunicationMessageQueue* m_queue;
    CommunicationClientStatus m_status;

    char m_buffer[CommunicationProtocol::MESSAGE_HEADER_AND_DATA_MAX_SIZE] = { 0 };
};

#endif // COMMUNICATIONCLIENT_H
