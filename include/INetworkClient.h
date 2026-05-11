#ifndef INETWORK_CLIENT_H
#define INETWORK_CLIENT_H

#include <string>

class INetworkClient
{
public:
    virtual ~INetworkClient() = default;

    virtual bool connectToServer(const std::string &ipAddress, int port) = 0;

    virtual void disconnect() = 0;

    virtual void sendJsonMessage(const std::string &jsonPayload) = 0;
};

#endif