#pragma once

#ifndef ADDTOPICTASK_H
#define ADDTOPICTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class AddTopicTask : public Task {
public:
    AddTopicTask(const std::string& username, const std::string& topicName, const std::string& author, const std::string& content, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
    std::string topicName;
    std::string author;
    std::string content;
};

#endif 
