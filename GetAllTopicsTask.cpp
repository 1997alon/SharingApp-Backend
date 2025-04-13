#include "GetAllTopicsTask.h"
#include <iostream>
#include "User.h"
#include "Globals.h"
#include "Crypto.h"
#include <boost/asio.hpp>
#include <json/json.h>


using namespace std;

GetAllTopicsTask::GetAllTopicsTask(const std::string& username, std::shared_ptr<boost::asio::ip::tcp::socket> socket)
    : Task(socket), username(username) {}

void GetAllTopicsTask::run() {
    Globals& globals = Globals::getInstance();
    Crypto& crypto = Crypto::getInstance();
    Json::Value responseJson = globals.getAllTopics(username);
    Json::StreamWriterBuilder writer;
    std::string response = Json::writeString(writer, responseJson);
    // Encode the response using Crypto::encode()
    std::string encodedResponse = crypto.encode(response);
    encodedResponse.push_back('\0');
    if (socket && socket->is_open()) {
        boost::asio::write(*socket, boost::asio::buffer(encodedResponse));
        std::cout << "im in if\n";
    }
    else {
        std::cerr << "Socket is invalid or closed!" << std::endl;
    }
}
