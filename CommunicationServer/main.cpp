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

#include <string.h>
#include <stdio.h>

#include "../Communication/CommunicationProtocol.h"
#include "../Communication/CommunicationServer.h"
#include "../Communication/CommunicationSender.h"

int main()
{
    CommunicationServer server(CommunicationProtocol::PRESENTER_IP_ADDRESS,
                               CommunicationProtocol::PRESENTER_IP_PORT,
                               CommunicationProtocol::PRESENTER_SERVER_MESSAGE_QUEUE_SIZE);
    CommunicationSender sender(&server);

    MessageTypeA mA;
    mA.w_f = 10; mA.w_o = 20;

    MessageTypeB mB;
    mB.v_x = 2.4; mB.v_y = -1.056; mB.v_z = -0.15; mB.v_o = 0.00012;

    while(!(sender.isWorking()))
    {

    }

    sender.sendMessageTypeA(mA);
    sender.sendMessageTypeB(mB);

    return 0;
}

