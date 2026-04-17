#ifndef REAL_NETWORK_CLIENT_H
#define REAL_NETWORK_CLIENT_H

#include "INetworkClient.h"
#include <QTcpSocket>
#include <QString>
#include <QObject>

class RealNetworkClient : public QObject, public INetworkClient {
    Q_OBJECT
public:
    explicit RealNetworkClient(QObject* parent = nullptr);
    ~RealNetworkClient() override;

    // the three required methods from the interface
    bool connectToServer(const std::string& ipAddress, int port) override;
    void disconnect() override;
    void sendJsonMessage(const std::string& jsonPayload) override;

private:
    QTcpSocket* socket;
};

#endif