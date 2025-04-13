#include "SearchTopicTask.h"
#include <iostream>
#include "User.h"
#include "Globals.h"
#include "Crypto.h"
#include <boost/asio.hpp>
#include <json/json.h>

using namespace std;

SearchTopicTask::SearchTopicTask(const std::string& username, const std::string& topicName, std::shared_ptr<boost::asio::ip::tcp::socket> socket)
    : Task(socket), username(username), topicName(topicName) {}

void SearchTopicTask::run() {
    Globals& globals = Globals::getInstance();
    Crypto& crypto = Crypto::getInstance();
    
    Json::Value responseJson;

    responseJson["action"] = "searchTopic";
    responseJson["success"] = false;
    responseJson["arguments"] = "topic not exist";

    if (globals.findTopic(username, topicName)) {
        responseJson["success"] = true;
        responseJson["arguments"] = "topic exists";
    }

    Json::StreamWriterBuilder writer;
    std::string response = Json::writeString(writer, responseJson);
    // Encode the response using Crypto::encode()
    std::string encodedResponse = crypto.encode(response);
    encodedResponse.push_back('\0');
    if (socket && socket->is_open()) {
        boost::asio::write(*socket, boost::asio::buffer(encodedResponse));
    }
    else {
        std::cerr << "Socket is invalid or closed!" << std::endl;
    }
}
