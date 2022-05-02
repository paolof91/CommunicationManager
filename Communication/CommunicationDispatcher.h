#ifndef COMMUNICATIONDISPATCHER_H
#define COMMUNICATIONDISPATCHER_H

class CommunicationClient;

class CommunicationDispatcher
{
public:
    CommunicationDispatcher(CommunicationClient *client);
    void doWork();

private:
    CommunicationClient *m_client;
};

#endif // COMMUNICATIONDISPATCHER_H
