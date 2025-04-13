#include "AddTopicTask.h"
#include <iostream>
#include "User.h"
#include "Globals.h"
#include "Crypto.h"
#include <boost/asio.hpp>
#include <json/json.h>

using namespace std;

AddTopicTask::AddTopicTask(const std::string& username, const std::string& topicName, const std::string& author, const std::string& content, std::shared_ptr<boost::asio::ip::tcp::socket> socket)
    : Task(socket), username(username), topicName(topicName), author(author), content(content) {}

void AddTopicTask::run() {
    Globals& globals = Globals::getInstance();
    Crypto& crypto = Crypto::getInstance();
    Json::Value responseJson;

    responseJson["action"] = "addTopic";
    responseJson["success"] = false;
    responseJson["arguments"] = "topic already exist";

    if (globals.addTopic(username, topicName, author, content)) {
        //add authorization for topic as the creator.
        globals.addAuthorization(username, topicName); 

        responseJson["success"] = true;
        responseJson["arguments"] = "its done!";
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
