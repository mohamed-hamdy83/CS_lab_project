#ifndef CHAT_CONTROLLER_H
#define CHAT_CONTROLLER_H

#include <QObject>
#include <QString>
#include "ChatGUI.h"
#include "INetworkClient.h"

class ChatController : public QObject
{
    Q_OBJECT
public:
    ChatController(ChatGUI *gui, INetworkClient *network, QObject *parent = nullptr);

private slots:

    void handleLogin(const QString &username);
    void handleSendMessage(const QString &message, const QString &recipient);
    void handleIncomingMessage(const std::string &jsonPayload);

private:
    ChatGUI *view;
    INetworkClient *network;
    QString currentUsername;
};

#endif