#include "ChatController.h"
#include <QJsonObject>
#include <QJsonDocument>

ChatController::ChatController(ChatGUI* gui, INetworkClient* net, QObject* parent) 
    : QObject(parent), view(gui), network(net) {
    
    // connect the gui signals to our logic slots
    connect(view, &ChatGUI::loginRequested, this, &ChatController::handleLogin);
    connect(view, &ChatGUI::sendMessageRequested, this, &ChatController::handleSendMessage);
    
    // start the app by showing the login screen
    view->showLoginScreen();
}

void ChatController::handleLogin(const QString& username) {
    // input validation: don't allow empty usernames
    if (username.trimmed().isEmpty()) {
        view->showErrorPopup("username cannot be empty.");
        return;
    }
    
    currentUsername = username;
    
    // try to connect to the server (we will mock this to always return true later)
    bool connected = network->connectToServer("127.0.0.1", 8080);
    
    if (connected) {
        // build the json login request
        QJsonObject payload;
        payload["username"] = username;
        
        QJsonObject message;
        message["type"] = "login";
        message["payload"] = payload;
        
        QJsonDocument doc(message);
        // convert json to string and send it
        network->sendJsonMessage(doc.toJson(QJsonDocument::Compact).toStdString());
        
        // switch the gui to the chat room
        view->showChatScreen();
    } else {
        view->showErrorPopup("failed to connect to server.");
    }
}

void ChatController::handleSendMessage(const QString& msgText) {
    // ignore empty messages
    if (msgText.trimmed().isEmpty()) {
        return; 
    }
    
    // build the json message
    QJsonObject payload;
    payload["sender"] = currentUsername;
    payload["content"] = msgText;
    
    QJsonObject message;
    message["type"] = "broadcast";
    message["payload"] = payload;
    
    QJsonDocument doc(message);
    network->sendJsonMessage(doc.toJson(QJsonDocument::Compact).toStdString());
    
    // display the message on our own screen right away so we know it sent
    view->appendChatMessage(currentUsername, msgText);
}