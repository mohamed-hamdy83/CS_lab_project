#include <gtest/gtest.h>
#include <fakeit.hpp>
#include <QApplication>
#include "ChatController.h"
#include "ChatGUI.h"
#include "INetworkClient.h"

using namespace fakeit;

// test 1: empty username should not trigger network connect
TEST(ChatControllerTest, EmptyUsernameFails) {
    Mock<INetworkClient> mockNetwork;
    
    ChatGUI gui;
    ChatController controller(&gui, &mockNetwork.get());
    
    // simulate gui emitting empty login
    emit gui.loginRequested(""); 
    
    // fakeit checks that connect was NEVER called
    Verify(Method(mockNetwork, connectToServer)).Exactly(0);
}

// test 2: valid username should attempt connection and send json
TEST(ChatControllerTest, ValidLoginSendsJson) {
    Mock<INetworkClient> mockNetwork;
    // fake the server accepting the connection
    When(Method(mockNetwork, connectToServer)).Return(true);
    // fake the sending method doing nothing
    When(Method(mockNetwork, sendJsonMessage)).AlwaysReturn();
    
    ChatGUI gui;
    ChatController controller(&gui, &mockNetwork.get());
    
    emit gui.loginRequested("student123");
    
    // verify connection was attempted and a message was sent
    Verify(Method(mockNetwork, connectToServer).Using("127.0.0.1", 8080)).Exactly(1);
    Verify(Method(mockNetwork, sendJsonMessage)).Exactly(1);
}

// test 3: failed connection should not send json
TEST(ChatControllerTest, FailedConnectionDoesNotSend) {
    Mock<INetworkClient> mockNetwork;
    // fake the server being down
    When(Method(mockNetwork, connectToServer)).Return(false);
    
    ChatGUI gui;
    ChatController controller(&gui, &mockNetwork.get());
    
    emit gui.loginRequested("student123");
    
    // verify it tried to connect, but did NOT send a message
    Verify(Method(mockNetwork, connectToServer)).Exactly(1);
    Verify(Method(mockNetwork, sendJsonMessage)).Exactly(0);
}

// test 4: empty chat message should not be sent over network
TEST(ChatControllerTest, EmptyMessageNotSent) {
    Mock<INetworkClient> mockNetwork;
    
    ChatGUI gui;
    ChatController controller(&gui, &mockNetwork.get());
    
    emit gui.sendMessageRequested("   "); 
    
    Verify(Method(mockNetwork, sendJsonMessage)).Exactly(0);
}

// test 5: valid chat message sends correct json broadcast
TEST(ChatControllerTest, ValidMessageSendsJson) {
    Mock<INetworkClient> mockNetwork;
    When(Method(mockNetwork, sendJsonMessage)).AlwaysReturn();
    
    ChatGUI gui;
    ChatController controller(&gui, &mockNetwork.get());
    
    emit gui.sendMessageRequested("hello world");
    
    // verify that the network interface was told to send exactly 1 message
    Verify(Method(mockNetwork, sendJsonMessage)).Exactly(1);
}

// qt needs a qapplication to create gui elements, even in tests
int main(int argc, char **argv) {
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}