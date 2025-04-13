#pragma once

#ifndef TASK_H
#define TASK_H

#include <boost/asio.hpp> // For the Socket
#include <memory>  // For std::shared_ptr

class Task {
public:
    Task(std::shared_ptr<boost::asio::ip::tcp::socket> socket) : socket(socket) {}
    virtual ~Task() = default;

    // Pure virtual function for the task to implement
    virtual void run() = 0;

    // Provide a getter for the socket
    std::shared_ptr<boost::asio::ip::tcp::socket> getSocket() { return socket; }

protected:
    std::shared_ptr<boost::asio::ip::tcp::socket> socket;  // Socket member variable
};

#endif // TASK_H
