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
#include "CommunicationMessageQueue.h"

CommunicationMessageQueue::CommunicationMessageQueue(uint16_t queueMaxSize)
{

    m_queueMaxSize = queueMaxSize;
    printf("queue max size: %d \n ",m_queueMaxSize );
    m_front = nullptr;
    m_rear = nullptr;
    m_size = 0;
}

CommunicationMessageQueue::~CommunicationMessageQueue()
{
    CommunicationMessage m;
    while (!isEmpty())
        dequeue(m);
}

/*
 * returns -1 if queue is full, otherwise 0 if successfull
 */
int CommunicationMessageQueue::enqueue(CommunicationMessage message)
{
    int ret = 0;

    // insert new node
    QueueNode* n = new QueueNode();
    if (m_size == 0)
    {
        m_front = n;
        m_rear = n;
    }
    else
    {
        m_rear->next = n;
        m_rear = n;
    }
    n->message = message;
    printf("enqueuing %s\n", message.data);

    // if max size has been reached, discard front
    if (m_size == m_queueMaxSize)
    {
        ret = -1;
        QueueNode *temp = m_front;
        m_front = m_front->next;
        delete temp;
        printf("\nQueue is full, discarding last messages.\n");
    }
    else
    {
        ret = 0;
        m_size++;
    }

    printf("queue size: %d", m_size);

    return ret;
}

/*
 * returns -1 if queue is empty, returns 0 if successful
 */
int CommunicationMessageQueue::dequeue(CommunicationMessage &m)
{
    if (m_size==0)
        return -1;
    m = m_front->message;
    QueueNode *temp = m_front;
    m_front = m_front->next;
    delete temp;
    m_size--;
    return 0;
}
