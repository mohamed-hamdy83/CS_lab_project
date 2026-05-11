#include <QApplication>
#include "ChatGUI.h"
#include "ChatController.h"
#include "RealNetworkClient.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ChatGUI gui;
    RealNetworkClient realNet;

    // pass the real network to the controller
    ChatController controller(&gui, &realNet);

    gui.show();
    gui.resize(400, 500);

    return app.exec();
}