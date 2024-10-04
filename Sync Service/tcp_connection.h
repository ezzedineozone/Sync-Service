#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H
#include "console.h"
#include <string>
#include "dependencies/asio/asio.hpp"
#include "stdlib.h"
class tcp_connection
    : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;
    static pointer create(asio::io_context& io_context)
    {
        return pointer(new tcp_connection(io_context));
    }

    asio::ip::tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        message_ = "hi";
        asio::async_write(socket_, asio::buffer(message_),
            std::bind(&tcp_connection::handle_write, shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }
    void notify_removal(std::string name)
    {
        nlohmann::json j;
        j["command"] = "remove";
        j["data"] = name;
        message_ = j.dump();
        asio::async_write(socket_, asio::buffer(message_), std::bind(&tcp_connection::notify_success, shared_from_this(), "remove", std::placeholders::_1, std::placeholders::_2));
    }
    void notify_add(const SyncModule& module)
    {
        nlohmann::json j;
        j["command"] = "add";
        j["data"] = module.to_json();
        message_ = j.dump();
        asio::async_write(socket_, asio::buffer(message_), std::bind(&tcp_connection::notify_success, shared_from_this(), "add", std::placeholders::_1, std::placeholders::_2));
    }
    void notify_success(std::string type, const std::error_code& ec, std::size_t bytes_transferred) {
        if (!ec) {
            Console::notify("Command '" + type + "' successfully sent. Bytes transferred: " + std::to_string(bytes_transferred) + "\n");
        }
        else {
            Console::notify("Error sending command '" + type + "': " + ec.message() + "\n");
        }
    }
private:
    tcp_connection(asio::io_context& io_context)
        : socket_(io_context)
    {

    }

    void handle_write(const std::error_code& error, std::size_t bytes_transferred)
    {
        if (error)
        {
            Console::notify_concurrent("Error during write: " + error.message() + "\n");
            handle_disconnect(error);
        }
        else
        {
            Console::notify_concurrent("Data sent successfully, bytes transferred: " + std::to_string(bytes_transferred) + "\n");
        }
    }
    void handle_disconnect(const std::error_code& ec)
    {
        if (ec) {
            Console::notify_concurrent("Connection closed with error: " + ec.message());
        }
        else {
            Console::notify_concurrent("Connection closed gracefully.");
        }
    }

    asio::ip::tcp::socket socket_;
    std::string message_;
};
#endif