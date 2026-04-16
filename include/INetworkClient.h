#ifndef INETWORK_CLIENT_H
#define INETWORK_CLIENT_H

#include <string>

class INetworkClient {
public:
    // a virtual destructor is required so the program cleans up memory correctly
    virtual ~INetworkClient() = default;

    // the "= 0" makes these "pure virtual functions". 
    
    // connects to the server IP and port
    virtual bool connectToServer(const std::string& ipAddress, int port) = 0;
    
    // disconnects from the server
    virtual void disconnect() = 0;
    
    // sends a formatted json string to the server
    virtual void sendJsonMessage(const std::string& jsonPayload) = 0;
};

#endif