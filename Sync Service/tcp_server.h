#pragma once
#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "dependencies/asio/asio.hpp"
#include "tcp_connection.h"
class tcp_server {
public:
    tcp_server(asio::io_context& io_context)
        : io_context_(io_context),
        acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 25565))
    {
        start_accept();
    }
private:
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    void start_accept()
    {
        tcp_connection::pointer new_connection =
            tcp_connection::create(io_context_);

        acceptor_.async_accept(new_connection->socket(),
            std::bind(&tcp_server::handle_accept, this, new_connection,
                asio::placeholders::error));
    }
    void handle_accept(tcp_connection::pointer new_connection,
        const std::error_code& error)
    {
        if (!error)
        {
            new_connection->start();
        }

        start_accept();
    }
};
#endif
