#pragma once
#ifndef GETMESSAGESTASK_H
#define GETMESSAGESTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class GetMessagesTask : public Task {
public:
    GetMessagesTask(const std::string& username, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
};

#endif // GETMESSAGESTASK_H
