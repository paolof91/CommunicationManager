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
