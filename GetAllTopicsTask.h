#pragma once


#ifndef GETALLTOPICSTASK_H
#define GETALLTOPICSTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class GetAllTopicsTask : public Task {
public:
    GetAllTopicsTask(const std::string& username, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
};

#endif 
