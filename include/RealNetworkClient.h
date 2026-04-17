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

private:
    QTcpSocket* socket;
    
    // qcoro coroutine to listen for messages asynchronously
    QCoro::Task<void> receiveMessages(); 
};

#endif