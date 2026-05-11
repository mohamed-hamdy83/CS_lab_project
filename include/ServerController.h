#ifndef SERVER_CONTROLLER_H
#define SERVER_CONTROLLER_H

#include <string>
#include <map>
#include <QJsonObject>
#include <QJsonDocument>
#include "INetworkServer.h"

class ServerController {
public:
    ServerController(INetworkServer* network);
    
    void clientConnected(int clientId);
    void clientDisconnected(int clientId);
    void processMessage(int clientId, const std::string& jsonMessage);
    std::string getUsername(int clientId) const;

private:
    INetworkServer* network;
    std::map<int, std::string> activeUsers;
    
    // --- THIS IS THE LINE THE COMPILER WAS LOOKING FOR ---
    void broadcastUserList(); 
};

#endif