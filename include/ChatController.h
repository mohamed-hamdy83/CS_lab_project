#ifndef CHAT_CONTROLLER_H
#define CHAT_CONTROLLER_H

#include <QObject>
#include <QString>
#include "ChatGUI.h"
#include "INetworkClient.h"

class ChatController : public QObject {
    Q_OBJECT // needed for qt signals and slots
public:
    ChatController(ChatGUI* gui, INetworkClient* network, QObject* parent = nullptr);

private slots:
    // these slots will catch the signals emitted by the gui
    void handleLogin(const QString& username);
    void handleSendMessage(const QString& message, const QString& recipient);
    void handleIncomingMessage(const std::string& jsonPayload);

private:
    ChatGUI* view;
    INetworkClient* network;
    QString currentUsername;
};

#endif