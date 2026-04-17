#include <iostream>
#include <string>
#include <string_view>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;
using boost::asio::as_tuple;

// Coroutine to handle an individual client's connection
awaitable<void> handle_client(tcp::socket socket) {
    try {
        // This loop keeps the it open
        for (;;) {
            std::string data;
            
            // Read until the newline character 
            auto [ec, bytes_read] = co_await boost::asio::async_read_until(
                socket, boost::asio::dynamic_buffer(data), '\n', as_tuple(use_awaitable)
            );

            // If the client disconnects or an error occurs, break the loop
            if (ec) {
                if (ec == boost::asio::error::eof) {
                    std::cout << "\n--- Client Disconnected ---\n";
                }
                break;
            }

            // Print every message received on this connection
            std::cout << "\n[Incoming Message]\n" << data;
            std::cout << "------------------\n";
        }
    } catch (std::exception& e) {
        std::cout << "Server error: " << e.what() << "\n";
    }
}
 
// Coroutine to listen for incoming connections
awaitable<void> listener() {
    auto io_ctx = co_await boost::asio::this_coro::executor;

    // Listen on port 8080 to match the ChatApp client
    tcp::acceptor acceptor(io_ctx, { tcp::v4(), 8080 });

    std::cout << "Server is listening on 127.0.0.1: port 8080...\n";

    while (true) {
        // Accept a new connection
        auto [ec, socket] = co_await acceptor.async_accept(as_tuple(use_awaitable));

        if (!ec) {
            // Spawn a new coroutine to handle this specific client
            co_spawn(io_ctx, handle_client(std::move(socket)), detached);
        }
        else {
            std::cout << "Accept error: " << ec.message() << "\n";
        }
    }
}

int main() {
    boost::asio::io_context io_context;

    // Start the main listener coroutine
    co_spawn(io_context, listener(), detached);

    // Run the event loop
    io_context.run();

    return 0;
}