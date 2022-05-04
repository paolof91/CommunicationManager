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

#include <stdio.h>

#include "CommunicationClient.h"
#include "CommunicationDispatcher.h"

CommunicationDispatcher::CommunicationDispatcher(CommunicationClient *client)
    : m_client(client)
{
}

void CommunicationDispatcher::startWork()
{
    pthread_create(&m_thread, NULL, dispatcherThread, (void*) this);
}

void CommunicationDispatcher::doWork()
{
    while(1)
    {
        if ( (m_client!=nullptr) &  (m_client->status() == CONNECTED))
        {
            CommunicationMessage message;
            int ret;
            ret = m_client->receiveMessageIfAvailable(message);
            if (ret==0)
            {
                dispatch(message);
            }
        }
    }
}

void* CommunicationDispatcher::dispatcherThread(void *param)
{
    CommunicationDispatcher *pThis = (CommunicationDispatcher*)param;
    pThis->doWork();
    return param;
}

void CommunicationDispatcher::dispatch(CommunicationMessage cm)
{
    switch(cm.messageTypeIdentifier)
    {
        case CommunicationProtocol::MTI_A:
            {
                MessageTypeA mA = CommunicationProtocol::decodeMessageTypeA(cm);
                printf("read MessageTypeA: \t(%f, %f)\n", mA.w_f, mA.w_o);
            }
            break;
        case CommunicationProtocol::MTI_B:
            {
                MessageTypeB mB = CommunicationProtocol::decodeMessageTypeB(cm);
                printf("read MessageTypeB: \t(%f, %f, %f, %f)\n", mB.v_x, mB.v_y, mB.v_z, mB.v_o);
            }
            break;
        default:
            {
                printf("read message whose MessageTypeIdentifier is not recognized A\n");
            }
    }
}
