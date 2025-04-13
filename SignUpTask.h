#pragma once

#ifndef SIGNUPTASK_H
#define SIGNUPTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class SignUpTask : public Task {
public:
    SignUpTask(const std::string& username, const std::string& password, const std::string& email, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
    std::string password;
    std::string email;
};

#endif // ADDUSERTASK_H
