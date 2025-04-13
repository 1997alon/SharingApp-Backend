#pragma once
#ifndef GETAUTHORIZATIONTOPICSTASK_H
#define GETAUTHORIZATIONTOPICSTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class GetAuthorizationTopicsTask : public Task {
public:
    GetAuthorizationTopicsTask(const std::string& username, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
};

#endif 