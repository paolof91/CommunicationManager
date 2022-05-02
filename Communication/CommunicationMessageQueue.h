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

#ifndef COMMUNICATIONMESSAGEQUEUE_H
#define COMMUNICATIONMESSAGEQUEUE_H

#include "CommunicationProtocol.h"

class CommunicationMessageQueue
{
public:
    CommunicationMessageQueue(uint16_t queueMaxSize);
    ~CommunicationMessageQueue();
    int enqueue(CommunicationMessage m);
    int dequeue(CommunicationMessage &m);
    bool isEmpty() { return m_size == 0; };
    bool isFull() { return m_size == m_queueMaxSize; };
    uint16_t size() { return m_size; };

private:
    struct QueueNode {
        QueueNode *next = nullptr;
        CommunicationMessage message;
    };

    QueueNode *m_front;
    QueueNode *m_rear;
    uint16_t m_size;
    uint16_t m_queueMaxSize;
};

#endif // COMMUNICATIONMESSAGEQUEUE_H
