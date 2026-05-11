#include "ServerController.h"
#include <QJsonArray>
#include <set>

ServerController::ServerController(INetworkServer* net) : network(net) {}

void ServerController::clientConnected(int clientId) {
    activeUsers[clientId] = "Unknown";
}

void ServerController::clientDisconnected(int clientId) {
    activeUsers.erase(clientId);
    broadcastUserList();
}

void ServerController::processMessage(int clientId, const std::string& jsonMessage) {
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(jsonMessage).toUtf8());
    if (doc.isNull() || !doc.isObject()) return; 
    
    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();
    QJsonObject payload = obj["payload"].toObject();
    
    if (type == "login") {
        QString baseUsername = payload["username"].toString();
        if (!baseUsername.isEmpty()) {
            std::string finalName = baseUsername.toStdString();
            int counter = 1;
            
            // --- FIX 1: DUPLICATE NAME RESOLUTION ---
            // Keep checking until we find a name that isn't taken
            while (true) {
                bool nameTaken = false;
                for (const auto& [id, name] : activeUsers) {
                    if (name == finalName && id != clientId) {
                        nameTaken = true;
                        break;
                    }
                }
                if (!nameTaken) break;
                
                counter++;
                finalName = baseUsername.toStdString() + "_" + std::to_string(counter);
            }
            
            activeUsers[clientId] = finalName;
            broadcastUserList(); 
        }
    } 
    else if (type == "broadcast") {
        network->broadcast(jsonMessage);
    }
    else if (type == "private_chat") {
        std::string targetUsername = payload["recipient"].toString().toStdString();
        
        int targetId = -1;
        for (const auto& [id, name] : activeUsers) {
            if (name == targetUsername) {
                targetId = id;
                break;
            }
        }
        
        if (targetId != -1) {
            network->sendToClient(targetId, jsonMessage);
            if (targetId != clientId) {
                network->sendToClient(clientId, jsonMessage); 
            }
        } else {
            // --- FIX 2: THE GHOST MESSAGE BOUNCE-BACK ---
            // Construct a system error and send it back to the person who typed the bad name
            QJsonObject errorPayload;
            errorPayload["sender"] = "System";
            errorPayload["content"] = "Error: User '" + payload["recipient"].toString() + "' is offline or does not exist.";
            
            QJsonObject errorMessage;
            errorMessage["type"] = "private_chat"; 
            errorMessage["payload"] = errorPayload;
            
            QJsonDocument errDoc(errorMessage);
            network->sendToClient(clientId, errDoc.toJson(QJsonDocument::Compact).toStdString());
        }
    }
    else if (type == "group_chat") {
        QJsonArray recipientsArray = payload["recipients"].toArray();
        std::set<int> targetIds; 
        
        for (const QJsonValue& value : recipientsArray) {
            std::string targetUsername = value.toString().toStdString();
            for (const auto& [id, name] : activeUsers) {
                if (name == targetUsername) {
                    targetIds.insert(id);
                    break;
                }
            }
        }
        
        if (!targetIds.empty()) {
            targetIds.insert(clientId); 
            for (int id : targetIds) {
                network->sendToClient(id, jsonMessage);
            }
        }
    }
}

std::string ServerController::getUsername(int clientId) const {
    auto it = activeUsers.find(clientId);
    return it != activeUsers.end() ? it->second : "";
}

void ServerController::broadcastUserList() {
    QJsonArray usersArray;
    for (const auto& [id, name] : activeUsers) {
        if (name != "Unknown") {
            usersArray.append(QString::fromStdString(name));
        }
    }
    
    QJsonObject payload;
    payload["users"] = usersArray;
    
    QJsonObject message;
    message["type"] = "user_list";
    message["payload"] = payload;
    
    QJsonDocument doc(message);
    network->broadcast(doc.toJson(QJsonDocument::Compact).toStdString());
}