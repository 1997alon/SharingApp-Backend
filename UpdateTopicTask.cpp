#include "UpdateTopicTask.h"
#include <iostream>
#include "User.h"
#include "Globals.h"
#include "Crypto.h"
#include <boost/asio.hpp>
#include <json/json.h>

using namespace std;

UpdateTopicTask::UpdateTopicTask(std::string& username, std::string& topicName, std::string& content, std::shared_ptr<boost::asio::ip::tcp::socket> socket)
    : Task(socket), username(username), topicName(topicName), content(content) {}

void UpdateTopicTask::run() {
    Globals& globals = Globals::getInstance();
    Crypto& crypto = Crypto::getInstance();

    Json::Value responseJson = globals.updateTopic(username, topicName, content);

    Json::StreamWriterBuilder writer;
    std::string response = Json::writeString(writer, responseJson);
    std::string encodedResponse = crypto.encode(response);
    encodedResponse.push_back('\0');
    if (socket && socket->is_open()) {
        boost::asio::write(*socket, boost::asio::buffer(encodedResponse));
    }
    else {
        std::cerr << "Socket is invalid or closed!" << std::endl;
    }
}
