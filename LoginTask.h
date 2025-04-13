#pragma once

#ifndef LOGINTASK_H
#define LOGINTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class LoginTask : public Task {
public:
    LoginTask(const std::string& username, const std::string& password, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
    std::string password;
};

#endif
