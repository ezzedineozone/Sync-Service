#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "dependencies/asio/asio.hpp"
#include "tcp_connection.h"
#include "sync_module.h"
class tcp_server {
public:
    tcp_server(asio::io_context& io_context)
        : io_context_(io_context),
        acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 13))
    {
        start_accept();
    }
    void notify_add(const SyncModule& module)
    {
        for (const auto& connection : connections_)
        {
            connection->notify_add(module);
        }
    }

    void notify_removal(const std::string& name)
    {
        for (const auto& connection : connections_)
        {
            connection->notify_removal(name);
        }
    }
private:
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::vector<tcp_connection::pointer> connections_;
    ;
    void start_accept()
    {
        tcp_connection::pointer connection =
            tcp_connection::create(io_context_);
        acceptor_.async_accept(connection->socket(),
            std::bind(&tcp_server::handle_accept, this, connection,
                asio::placeholders::error));
    }
    void handle_accept(tcp_connection::pointer new_connection,
        const std::error_code& error)
    {
        if (!error)
        {
            this->connections_.push_back(new_connection);
            new_connection->start();
        }

        start_accept();
    }
};
#endif
