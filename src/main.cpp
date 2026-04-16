#include <QApplication>
#include <iostream>
#include "ChatGUI.h"
#include "ChatController.h"
#include "INetworkClient.h"

// a fake network client just to let the app run on your screen without crashing
class DummyNetwork : public INetworkClient {
public:
    bool connectToServer(const std::string& ip, int port) override {
        std::cout << "dummy network: pretending to connect to " << ip << ":" << port << "\n";
        return true; // always pretend it worked
    }
    
    void disconnect() override {
        std::cout << "dummy network: disconnected\n";
    }
    
    void sendJsonMessage(const std::string& jsonPayload) override {
        std::cout << "dummy network: sending json -> " << jsonPayload << "\n";
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // create our objects
    ChatGUI gui;
    DummyNetwork dummyNet;
    
    // the controller connects the gui and the dummy network
    ChatController controller(&gui, &dummyNet);

    // show the window
    gui.show();
    gui.resize(400, 500); // give it a nice starting size

    // start the qt event loop
    return app.exec();
}