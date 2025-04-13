#pragma once

#ifndef UPDATETOPICTASK_H
#define UPDATETOPICTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class UpdateTopicTask : public Task {
public:
    UpdateTopicTask(std::string& username, std::string& topicName, std::string& content, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
    std::string topicName;
    std::string content;
};

#endif 
