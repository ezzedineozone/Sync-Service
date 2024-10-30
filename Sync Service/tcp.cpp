#include "tcp.h"
#include "sync_service.h"
#include "console.h"
#include "sync_module.h"
tcp_connection::pointer tcp_connection::create(asio::io_context& io_context, int index, const SyncService& service)
{
    return std::make_shared<tcp_connection>(io_context, index, service);
}

asio::ip::tcp::socket& tcp_connection::socket()
{
    return socket_;
}

void tcp_connection::start()
{
    nlohmann::json j;
    std::string command = "init";
    std::vector<SyncModule*> data = this->service.get_handler()->sync_modules;
    nlohmann::json jsonData = nlohmann::json::array();
    for (const auto& module : data) {
        if (module != nullptr) {
            jsonData.push_back(module->to_json());
        }
    }
    j["command"] = command;
    j["data"] = jsonData;
    message_ = j.dump() + "\r\n";
    asio::async_write(socket_, asio::buffer(message_),
        std::bind(&tcp_connection::handle_write, shared_from_this(),
            std::placeholders::_1,
            std::placeholders::_2));
}

void tcp_connection::notify_removal(std::string name)
{
    nlohmann::json j;
    j["command"] = "remove";
    j["data"] = name;
    message_ = j.dump() + "\r\n";
    asio::async_write(socket_, asio::buffer(message_), std::bind(&tcp_connection::notify_success, shared_from_this(), "remove", std::placeholders::_1, std::placeholders::_2));
}
void tcp_connection::notify_failure(std::string command, std::string message)
{
    nlohmann::json j;
    j["command"] = "-1";
    j["data"] = message;
    message_ = j.dump() + "\r\n";
    asio::async_write(socket_, asio::buffer(message_), std::bind(&tcp_connection::notify_success, shared_from_this(), "remove", std::placeholders::_1, std::placeholders::_2));
}
void tcp_server::notify_failure(std::string command, std::string msg)
{
    for (const auto& connection : connections_)
    {
        connection->notify_failure (command, msg);
    }
}
void tcp_connection::notify_add(const SyncModule& module)
{
    nlohmann::json j;
    j["command"] = "add";
    j["data"] = module.to_json();
    message_ = j.dump() + "\r\n";
    asio::async_write(this->socket_, asio::buffer(this->message_), std::bind(&tcp_connection::notify_success, shared_from_this(), "add", std::placeholders::_1, std::placeholders::_2));
}

void tcp_connection::start_reading() {
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
            throw std::system_error(error);

        msg.append(buf.data(), len);
        handle_recieved_message(msg);
    }
}

void tcp_connection::notify_success(std::string type, const std::error_code& ec, std::size_t bytes_transferred) {
    if (!ec) {
        Console::notify("Command '" + type + "' successfully sent. Bytes transferred: " + std::to_string(bytes_transferred) + "\n");
    }
    else {
        Console::notify("Error sending command '" + type + "': " + ec.message() + "\n");
    }
}

tcp_connection::tcp_connection(asio::io_context& io_context, int index, const SyncService& service)
    : socket_(io_context), index_(index), service(service)
{
}

void tcp_connection::handle_write(const std::error_code& error, std::size_t bytes_transferred)
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

void tcp_connection::handle_disconnect(const std::error_code& ec)
{
    if (ec) {
        Console::notify_concurrent("Connection closed with error: " + ec.message());
    }
    else {
        Console::notify_concurrent("Connection closed gracefully.");
    }
}

int tcp_connection::handle_recieved_message(const std::string& msg)
{
    nlohmann::json json_msg = nlohmann::json::parse(msg);
    int command_executed = Console::command_handler_json(json_msg);
    return command_executed;
}
tcp_server::tcp_server(asio::io_context& io_context, const SyncService& service)
    : io_context_(io_context),
    acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 13)), indexes(0),
    service(service)
{
    start_accept();
}

tcp_server::~tcp_server() {
    for (auto& thread : read_threads) {
        if (thread->joinable()) {
            thread->join();
        }
    }
    connections_.clear();
}

void tcp_server::notify_add(const SyncModule& module)
{
    for (const auto& connection : connections_)
    {
        connection->notify_add(module);
    }
}

void tcp_server::notify_removal(const std::string& name)
{
    for (const auto& connection : connections_)
    {
        connection->notify_removal(name);
    }
}

void tcp_server::start_accept()
{
    tcp_connection::pointer connection =
        tcp_connection::create(io_context_, this->indexes, this->service);
    acceptor_.async_accept(connection->socket(),
        std::bind(&tcp_server::handle_accept, this, connection,
            asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection, const std::error_code& error)
{
    if (!error)
    {
        this->connections_.push_back(new_connection);
        new_connection->start();
        read_threads.push_back(std::make_unique<std::thread>([this, new_connection] {
            Console::notify_concurrent("began reading on connection " + this->indexes);
            this->indexes += 1;
            new_connection->start_reading();
            }));
    }

    start_accept();
}
