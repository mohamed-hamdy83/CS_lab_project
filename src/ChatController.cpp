#include "ChatController.h"
#include "RealNetworkClient.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>

ChatController::ChatController(ChatGUI *gui, INetworkClient *net, QObject *parent)
    : QObject(parent), view(gui), network(net)
{

    connect(view, &ChatGUI::loginRequested, this, &ChatController::handleLogin);
    connect(view, &ChatGUI::sendMessageRequested, this, &ChatController::handleSendMessage);

    auto *realNet = dynamic_cast<RealNetworkClient *>(network);
    if (realNet)
    {
        connect(realNet, &RealNetworkClient::jsonMessageReceived,
                this, &ChatController::handleIncomingMessage);
    }

    view->showLoginScreen();
}

void ChatController::handleLogin(const QString &username)
{
    // input validation: don't allow empty usernames
    if (username.trimmed().isEmpty())
    {
        view->showErrorPopup("username cannot be empty.");
        return;
    }

    currentUsername = username;
    view->setWindowTitleByUsername(username);

    bool connected = network->connectToServer("127.0.0.1", 8080);

    if (connected)
    {
        QJsonObject payload;
        payload["username"] = username;

        QJsonObject message;
        message["type"] = "login";
        message["payload"] = payload;

        QJsonDocument doc(message);
        network->sendJsonMessage(doc.toJson(QJsonDocument::Compact).toStdString());

        // switch the gui to the chat room
        view->showChatScreen();
    }
    else
    {
        view->showErrorPopup("failed to connect to server.");
    }
}

void ChatController::handleSendMessage(const QString &msgText, const QString &recipient)
{
    if (msgText.trimmed().isEmpty())
        return;

    QJsonObject payload;
    payload["sender"] = currentUsername;
    payload["content"] = msgText;

    QJsonObject message;
    QString target = recipient.trimmed();

    // Logic 1: No recipient = Broadcast
    if (target.isEmpty())
    {
        message["type"] = "broadcast";
    }
    // Logic 2: Has commas = Group Chat
    else if (target.contains(","))
    {
        message["type"] = "group_chat";

        QStringList userList = target.split(",");
        QJsonArray recipientsArray;
        for (const QString &user : userList)
        {
            QString trimmedUser = user.trimmed();
            if (!trimmedUser.isEmpty())
            {
                recipientsArray.append(trimmedUser);
            }
        }
        payload["recipients"] = recipientsArray;
    }
    // Logic 3: Single name = Private Chat
    else
    {
        message["type"] = "private_chat";
        payload["recipient"] = target;
    }

    message["payload"] = payload;

    QJsonDocument doc(message);
    network->sendJsonMessage(doc.toJson(QJsonDocument::Compact).toStdString());
}

void ChatController::handleIncomingMessage(const std::string& jsonPayload) {
    QJsonDocument doc = QJsonDocument::fromJson(QString::fromStdString(jsonPayload).toUtf8());

    if (doc.isNull()) return;

    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();
    QJsonObject payload = obj["payload"].toObject();
    
    if (type == "user_list") {
        QJsonArray usersArray = payload["users"].toArray();
        QStringList activeUsers;
        for (const QJsonValue& val : usersArray) {
            activeUsers << val.toString();
        }
        view->updateOnlineUsers(activeUsers, currentUsername);
    } 
    // Accept all three types of messages
    else if (type == "broadcast" || type == "private_chat" || type == "group_chat") {
        QString originalSender = payload["sender"].toString();
        QString content = payload["content"].toString();
        
        // We use a new variable for the display name so we don't overwrite the original
        QString displaySender = originalSender;
        
        // Add visual tags to the GUI so the user knows the privacy level
        if (type == "private_chat") {
            displaySender = "[Private] " + originalSender;
        } 
        else if (type == "group_chat") {
            QJsonArray recipientsArray = payload["recipients"].toArray();
            QStringList otherMembers;
            
            // 1. Add the sender to the group list (if the viewer isn't the sender)
            if (originalSender != currentUsername) {
                otherMembers << originalSender;
            }
            
            // 2. Loop through the recipients, SKIP the viewer AND prevent duplicates
            for (const QJsonValue& val : recipientsArray) {
                QString memberName = val.toString();
                if (memberName != currentUsername && memberName != originalSender) {
                    otherMembers << memberName;
                }
            }
            
            // 3. Format it nicely
            if (otherMembers.isEmpty()) {
                displaySender = "[Group] " + originalSender;
            } else {
                displaySender = "[Group with " + otherMembers.join(", ") + "] " + originalSender;
            }
        }
        
        // Send the formatted string to the GUI
        view->appendChatMessage(displaySender, content);
    }
}