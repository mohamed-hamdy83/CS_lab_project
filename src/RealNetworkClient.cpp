#include "RealNetworkClient.h"
#include <qcoronetwork.h>  
#include <qcoroiodevice.h> 
#include <iostream>

RealNetworkClient::RealNetworkClient(QObject* parent) : QObject(parent) {
    socket = new QTcpSocket(this);
}

RealNetworkClient::~RealNetworkClient() {
    disconnect();
}

bool RealNetworkClient::connectToServer(const std::string& ipAddress, int port) {
    socket->connectToHost(QString::fromStdString(ipAddress), port);
    
    if (socket->waitForConnected(3000)) {
        std::cout << "network: successfully connected to server!\n";
        
        // start the asynchronous qcoro read loop in the background!
        receiveMessages(); 
        
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
        QByteArray message = QString::fromStdString(jsonPayload + "\n").toUtf8();
        socket->write(message);
        socket->waitForBytesWritten(1000); 
    }
}

QCoro::Task<void> RealNetworkClient::receiveMessages() {
    // this loops continuously without freezing the gui
    while (socket->isOpen() && socket->state() == QAbstractSocket::ConnectedState) {
        
        // co_await yields control back to the app until data arrives
        QByteArray data = co_await qCoro(socket).readLine();
        
        if (data.isEmpty()) {
            break; 
        }
        
        std::cout << "[Client Received] " << data.toStdString() << "\n";
    }
}