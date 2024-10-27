#ifndef TCP_H
#define TCP_H
#include "dependencies/asio/asio.hpp"
#include "dependencies/json/json.hpp"
#include <thread>
#include <memory>
class SyncService;
class Console;
class SyncModule;

class tcp_connection : public std::enable_shared_from_this<tcp_connection>
{
public:
    typedef std::shared_ptr<tcp_connection> pointer;
    static pointer create(asio::io_context& io_context, int index, const SyncService& service);
    tcp_connection(asio::io_context& io_context, int index, const SyncService& service);
    asio::ip::tcp::socket& socket();
    void start();
    void notify_removal(std::string name);
    void notify_add(const SyncModule& module);
    void start_reading();
    void notify_success(std::string type, const std::error_code& ec, std::size_t bytes_transferred);

private:
    void handle_write(const std::error_code& error, std::size_t bytes_transferred);
    void handle_disconnect(const std::error_code& ec);
    int handle_recieved_message(const std::string& msg);
    asio::ip::tcp::socket socket_;
    std::string message_;
    const SyncService& service;
    int index_;
};

class tcp_server {
public:
    tcp_server(asio::io_context& io_context, const SyncService& service);
    ~tcp_server();
    void notify_add(const SyncModule& module);
    void notify_removal(const std::string& name);

private:
    void start_accept();
    void handle_accept(tcp_connection::pointer new_connection, const std::error_code& error);
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::vector<tcp_connection::pointer> connections_;
    std::vector<std::unique_ptr<std::thread>> read_threads;
    const SyncService& service;
    int indexes;
};
#endif
