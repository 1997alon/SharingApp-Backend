#pragma once

#ifndef GETUSERTOPICSTASK_H
#define GETUSERTOPICSTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class GetUserTopicsTask : public Task {
public:
    GetUserTopicsTask(const std::string& username, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
    std::string topicName;
};

#endif 
