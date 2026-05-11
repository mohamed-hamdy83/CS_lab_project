#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <mutex>
#include <boost/asio.hpp>
#include "ServerController.h"
#include "INetworkServer.h"

using boost::asio::as_tuple;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::ip::tcp;

class BoostNetworkServer : public INetworkServer
{
public:
    BoostNetworkServer(boost::asio::io_context &io) : io_context(io) {}

    void addClient(std::shared_ptr<tcp::socket> socket, int clientId)
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients[clientId] = socket;
    }

    void removeClient(int clientId)
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(clientId);
    }

    void broadcast(const std::string &message) override
    {
        std::lock_guard<std::mutex> lock(clients_mutex);

        auto payload = std::make_shared<std::string>(message);
        if (payload->empty() || payload->back() != '\n')
        {
            *payload += "\n";
        }

        for (auto const &[id, socket] : clients)
        {
            if (socket->is_open())
            {
                boost::asio::async_write(*socket, boost::asio::buffer(*payload),
                                         [payload](boost::system::error_code ec, std::size_t /*length*/)
                                         {
                                             if (ec)
                                                 std::cerr << "Broadcast error: " << ec.message() << "\n";
                                         });
            }
        }
    }

    void sendToClient(int clientId, const std::string &message) override
    {
        std::lock_guard<std::mutex> lock(clients_mutex);

        auto it = clients.find(clientId);
        if (it != clients.end() && it->second->is_open())
        {
            auto payload = std::make_shared<std::string>(message);
            if (payload->empty() || payload->back() != '\n')
            {
                *payload += "\n";
            }

            boost::asio::async_write(*(it->second), boost::asio::buffer(*payload),
                                     [payload](boost::system::error_code ec, std::size_t)
                                     {
                                         if (ec)
                                             std::cerr << "Direct send error: " << ec.message() << "\n";
                                     });
        }
    }

private:
    boost::asio::io_context &io_context;
    std::map<int, std::shared_ptr<tcp::socket>> clients;
    std::mutex clients_mutex;
};

boost::asio::io_context io_context;
BoostNetworkServer networkServer(io_context);
ServerController controller(&networkServer);
int nextClientId = 1;

awaitable<void> handle_client(std::shared_ptr<tcp::socket> socket, int clientId)
{
    try
    {
        networkServer.addClient(socket, clientId);
        controller.clientConnected(clientId);
        std::cout << "[Server] Client " << clientId << " connected.\n";

        for (;;)
        {
            std::string data;
            auto [ec, bytes_read] = co_await boost::asio::async_read_until(
                *socket, boost::asio::dynamic_buffer(data), '\n', as_tuple(use_awaitable));

            if (ec)
                break;

            std::cout << "[Received from " << clientId << "] " << data;
            controller.processMessage(clientId, data);
        }
    }
    catch (std::exception &e)
    {
        std::cout << "[Server] Client " << clientId << " error: " << e.what() << "\n";
    }

    std::cout << "[Server] Client " << clientId << " disconnected.\n";
    controller.clientDisconnected(clientId);
    networkServer.removeClient(clientId);
}

awaitable<void> listener()
{
    auto io_ctx = co_await boost::asio::this_coro::executor;
    tcp::acceptor acceptor(io_ctx, {tcp::v4(), 8080});

    std::cout << "==========================================\n";
    std::cout << "Boost Server is listening on port 8080...\n";
    std::cout << "==========================================\n";

    while (true)
    {
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));

        if (!ec)
        {
            auto shared_socket = std::make_shared<tcp::socket>(std::move(socket));
            co_spawn(io_ctx, handle_client(shared_socket, nextClientId++), detached);
        }
        else
        {
            std::cout << "Accept error: " << ec.message() << "\n";
        }
    }
}

int main()
{
    co_spawn(io_context, listener(), detached);
    io_context.run();
    return 0;
}