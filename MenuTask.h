#pragma once
#ifndef MENUTASK_H
#define MENUTASK_H

#include "Task.h"
#include <string>
#include <boost/asio.hpp> // For the Socket

class MenuTask : public Task {
public:
    MenuTask(const std::string& username, std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    void run() override;

private:
    std::string username;
};

#endif 

