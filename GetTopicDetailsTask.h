#pragma once

#ifndef GETTOPICDETAILSTASK_H
#define GETTOPICDETAILSTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class GetTopicDetailsTask : public Task {
public:
    GetTopicDetailsTask(const std::string& username, const std::string& topicName, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
    std::string topicName;
};

#endif 
