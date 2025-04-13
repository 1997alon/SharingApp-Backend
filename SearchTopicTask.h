#pragma once

#ifndef SEARCGTOPICTASK_H
#define SEARCGTOPICTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class SearchTopicTask : public Task {
public:
    SearchTopicTask(const std::string& username, const std::string& topicName, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
    std::string topicName;
};

#endif 
