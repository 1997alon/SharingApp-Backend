#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "User.h"
#include "Globals.h"
#include "Crypto.h"
#include "SignUpTask.h"
#include "ThreadPool.h"
#include <boost/asio.hpp>
#include <memory>  // for shared_ptr

class Server {
public:
    Server(int port = 12345, int numThreads = 5);

    void run();

private:
    int port;
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor;

    ThreadPool pool;

    // Method to handle reading data asynchronously from the socket
    void handle_read(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    // Method to start accepting connections asynchronously
    void start_accept();
};

#endif // SERVER_H
