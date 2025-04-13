#pragma once
#ifndef OPENMESSAGETASK_H
#define OPENMESSAGETASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class OpenAMessageTask : public Task {
public:
    OpenAMessageTask(std::string& username, int sender, int messageID, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
    int sender; 
    int messageID;
};

#endif // GETMESSAGESTASK_H
