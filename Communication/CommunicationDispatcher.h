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

#ifndef COMMUNICATIONDISPATCHER_H
#define COMMUNICATIONDISPATCHER_H

#include <pthread.h>
#include "CommunicationProtocol.h"

class CommunicationClient;

class CommunicationDispatcher
{
public:
    CommunicationDispatcher(CommunicationClient *client);
    void startWork();

protected:
    void dispatch(CommunicationMessage message);

private:
    static void* dispatcherThread(void *param);
    void doWork();

    CommunicationClient *m_client;
    pthread_t m_thread;
};

#endif // COMMUNICATIONDISPATCHER_H
