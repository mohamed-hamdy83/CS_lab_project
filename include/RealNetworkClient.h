#ifndef REAL_NETWORK_CLIENT_H
#define REAL_NETWORK_CLIENT_H

#include "INetworkClient.h"
#include <QTcpSocket>
#include <QString>
#include <QObject>
#include <qcorotask.h>

class RealNetworkClient : public QObject, public INetworkClient {
    Q_OBJECT
public:
    explicit RealNetworkClient(QObject* parent = nullptr);
    ~RealNetworkClient() override;

    bool connectToServer(const std::string& ipAddress, int port) override;
    void disconnect() override;
    void sendJsonMessage(const std::string& jsonPayload) override;

signals:
    void jsonMessageReceived(const std::string& jsonPayload);

private:
    QTcpSocket* socket;
    QCoro::Task<void> receiveMessages(); 
};

#endif