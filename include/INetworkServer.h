#ifndef INETWORK_SERVER_H
#define INETWORK_SERVER_H

#include <string>

class INetworkServer
{
public:
    virtual ~INetworkServer() = default;
    virtual void broadcast(const std::string &message) = 0;
    virtual void sendToClient(int clientId, const std::string &message) = 0;
};

#endif