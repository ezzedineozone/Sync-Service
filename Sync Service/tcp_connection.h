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
    int index_;
    static pointer create(asio::io_context& io_context, int index)
    {
        return pointer(new tcp_connection(io_context, index));
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
    void start_reading() {
        for (;;) {
            std::string msg;
            std::array<char, 512> buf;
            std::error_code error;

            size_t len = socket_.read_some(asio::buffer(buf), error);
            if (error == asio::error::eof)
            {
                std::cout << "connection closed";
                break;
            }
            else if (error)
                throw std::system_error(error); // Some other error.

            msg.append(buf.data(), len);
            handle_recieved_message(msg);
        }
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
    tcp_connection(asio::io_context& io_context, int index)
        : socket_(io_context), index_(index)
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
    int handle_recieved_message(const std::string& msg)
    {

        nlohmann::json json_msg = nlohmann::json::parse(msg);
        int command_executed = Console::command_handler_json(json_msg);
        return command_executed;
    }
    asio::ip::tcp::socket socket_;
    std::string message_;
};
#endif