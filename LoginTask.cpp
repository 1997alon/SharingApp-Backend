#include "LoginTask.h"
#include <iostream>
#include "User.h"
#include "Globals.h"
#include "Crypto.h"
#include <boost/asio.hpp>
#include <json/json.h>

using namespace std;

LoginTask::LoginTask(const std::string& username, const std::string& password, std::shared_ptr<boost::asio::ip::tcp::socket> socket)
    : Task(socket), username(username), password(password) {}

void LoginTask::run() {
    Globals& globals = Globals::getInstance();
    Crypto& crypto = Crypto::getInstance();
    size_t hashpass = crypto.toHashcode(password);

    Json::Value responseJson = globals.login(username, hashpass, socket);
    std::cout << responseJson << std::endl;

    // Convert the JSON object to a string
    Json::StreamWriterBuilder writer;
    std::string response = Json::writeString(writer, responseJson);

    // Encode the response
    std::string encodedResponse = crypto.encode(response);
    encodedResponse.push_back('\0');
    // Send encoded response over the socket
    if (socket && socket->is_open()) {
        boost::asio::write(*socket, boost::asio::buffer(encodedResponse));
    }
    else {
        std::cerr << "Socket is invalid or closed!" << std::endl;
    }
}
