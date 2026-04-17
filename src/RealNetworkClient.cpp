#include "RealNetworkClient.h"
#include <iostream>

RealNetworkClient::RealNetworkClient(QObject* parent) : QObject(parent) {
    socket = new QTcpSocket(this);
}

RealNetworkClient::~RealNetworkClient() {
    disconnect();
}

bool RealNetworkClient::connectToServer(const std::string& ipAddress, int port) {
    // connect to the ip and port
    socket->connectToHost(QString::fromStdString(ipAddress), port);
    
    // wait up to 3 seconds for the server to respond
    if (socket->waitForConnected(3000)) {
        std::cout << "network: successfully connected to server!\n";
        return true;
    }
    
    std::cout << "network: failed to connect.\n";
    return false;
}

void RealNetworkClient::disconnect() {
    if (socket->isOpen()) {
        socket->close();
        std::cout << "network: disconnected from server.\n";
    }
}

void RealNetworkClient::sendJsonMessage(const std::string& jsonPayload) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        // we add a newline at the end so the server knows when the message finishes
        QByteArray message = QString::fromStdString(jsonPayload + "\n").toUtf8();
        socket->write(message);
        
        // ensure it actually sends over the network
        socket->waitForBytesWritten(1000); 
    }
}