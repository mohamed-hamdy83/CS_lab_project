cmake_minimum_required(VERSION 3.14)
project(CS_lab_project)

# Use C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Let CMake handle Qt's special macros automatically
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find the Qt libraries
find_package(Qt6 COMPONENTS Core Gui Widgets Network REQUIRED)

# --- QCORO SECTION ---
include(FetchContent)
FetchContent_Declare(
    QCoro
    GIT_REPOSITORY https://github.com/danvratil/qcoro.git
    GIT_TAG v0.6.0
)
set(QCORO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(QCORO_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(QCORO_WITH_QTWEBSOCKETS OFF CACHE BOOL "" FORCE)
set(QCORO_WITH_QTDBUS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(QCoro)

# --- GOOGLETEST SECTION ---
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) 
FetchContent_MakeAvailable(googletest)

# Tell CMake where to look for our header files
include_directories(include)

# ---------------------------------------------------------
# build 1: The Main Application
# ---------------------------------------------------------
add_executable(ChatApp 
    src/main.cpp 
    src/ChatGUI.cpp 
    src/ChatController.cpp
    src/RealNetworkClient.cpp
    include/ChatGUI.h
    include/ChatController.h
    include/RealNetworkClient.h
)
target_link_libraries(ChatApp PRIVATE 
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network 
    QCoro6::Core QCoro6::Network
)

# ---------------------------------------------------------
# build 2: The Unit Tests (Chat Logic)
# ---------------------------------------------------------
enable_testing()

add_executable(ChatTests 
    tests/ChatControllerTest.cpp 
    src/ChatGUI.cpp 
    src/ChatController.cpp
    src/RealNetworkClient.cpp
    include/ChatGUI.h
    include/ChatController.h
    include/RealNetworkClient.h
)
target_link_libraries(ChatTests PRIVATE 
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network 
    QCoro6::Core QCoro6::Network 
    gtest 
)

# ---------------------------------------------------------
# build 3: Server Logic Unit Tests
# ---------------------------------------------------------
add_executable(ServerTests
    tests/ServerControllerTest.cpp
    src/ServerController.cpp
)
target_link_libraries(ServerTests PRIVATE
    Qt6::Core
    gtest
    gtest_main
    Threads::Threads
)
add_test(NAME ServerLogicTests COMMAND ServerTests)

# ---------------------------------------------------------
# build 4: The Main Server Application
# ---------------------------------------------------------
add_executable(ServerApp 
    Server.cpp
    src/ServerController.cpp
)

# Boost configuration
set(BOOST_ROOT "C:/boost_1_84_0") 
find_package(Boost REQUIRED)
target_include_directories(ServerApp PRIVATE ${Boost_INCLUDE_DIRS})

# Link Windows Socket libraries required for Boost.Asio on MinGW
target_link_libraries(ServerApp PRIVATE 
    Qt6::Core 
    ws2_32 
    wsock32
)


## With help of Gemini