#include <gtest/gtest.h>
#include <fakeit.hpp>
#include "ServerController.h"
#include "INetworkServer.h"

using namespace fakeit;

// Test 1: New connection is tracked
TEST(ServerControllerTest, ClientConnects) {
    Mock<INetworkServer> mockNet;
    When(Method(mockNet, broadcast)).AlwaysReturn();
    ServerController server(&mockNet.get());
    
    server.clientConnected(1);
    EXPECT_EQ(server.getUsername(1), "Unknown");
}

// Test 2: Valid login updates the username state
TEST(ServerControllerTest, ClientLoginSavesUsername) {
    Mock<INetworkServer> mockNet;
    When(Method(mockNet, broadcast)).AlwaysReturn();
    ServerController server(&mockNet.get());
    
    server.clientConnected(1);
    std::string loginJson = "{\"type\":\"login\",\"payload\":{\"username\":\"mohamed.hawas\"}}";
    server.processMessage(1, loginJson);
    
    EXPECT_EQ(server.getUsername(1), "mohamed.hawas");
}

// Test 3: Disconnect removes the user from state
TEST(ServerControllerTest, ClientDisconnects) {
    Mock<INetworkServer> mockNet;
    When(Method(mockNet, broadcast)).AlwaysReturn();
    ServerController server(&mockNet.get());
    
    server.clientConnected(1);
    server.clientDisconnected(1);
    
    EXPECT_EQ(server.getUsername(1), "");
}

// Test 4: Bad JSON doesn't crash the logic
TEST(ServerControllerTest, InvalidJsonIgnored) {
    Mock<INetworkServer> mockNet;
    When(Method(mockNet, broadcast)).AlwaysReturn();
    ServerController server(&mockNet.get());
    
    server.clientConnected(1);
    server.processMessage(1, "not real json");
    
    EXPECT_EQ(server.getUsername(1), "Unknown");
}

// Test 5: A broadcast type triggers the network broadcast function
TEST(ServerControllerTest, BroadcastRoutesCorrectly) {
    Mock<INetworkServer> mockNet;
    When(Method(mockNet, broadcast)).AlwaysReturn();
    
    ServerController server(&mockNet.get());
    
    std::string broadcastJson = "{\"type\":\"broadcast\",\"payload\":{\"sender\":\"mohamed.hawas\",\"content\":\"hello\"}}";
    server.processMessage(1, broadcastJson);
    
    Verify(Method(mockNet, broadcast).Using(broadcastJson)).Exactly(1);
}