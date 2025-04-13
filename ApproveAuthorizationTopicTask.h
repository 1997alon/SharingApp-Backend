#pragma once

#ifndef APPROVEAUTHORIZATIONTOPICTASK_H
#define APPROVEAUTHORIZATIONTOPICTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class ApproveAuthorizationTopicTask : public Task {
public:
    ApproveAuthorizationTopicTask(std::string& username, int sender, int messageID, bool approved, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
    int sender;
    int messageID;
    bool approved;
};

#endif // GETMESSAGESTASK_H
