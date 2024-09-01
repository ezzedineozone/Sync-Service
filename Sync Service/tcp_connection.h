#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

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
        std::string message_2 = std::string("hi2");
        asio::async_write(socket_, asio::buffer(message_),
            std::bind(&tcp_connection::handle_write, shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
        asio::async_write(socket_, asio::buffer(message_2),
            std::bind(&tcp_connection::handle_write, shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }
private:
    tcp_connection(asio::io_context& io_context)
        : socket_(io_context)
    {

    }

    void handle_write(const std::error_code&,
        size_t)
    {
        std::cout << "\nwritten to console";
    }

    asio::ip::tcp::socket socket_;
    std::string message_;
};
#endif