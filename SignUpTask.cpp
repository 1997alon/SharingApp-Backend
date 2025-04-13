#include "SignUpTask.h"
#include <iostream>
#include "User.h"
#include "Globals.h"
#include "Crypto.h"
#include <boost/asio.hpp>
#include <json/json.h>


using namespace std;

SignUpTask::SignUpTask(const std::string& username, const std::string& password, const std::string& email, std::shared_ptr<boost::asio::ip::tcp::socket> socket)
    : Task(socket), username(username), password(password), email(email) {}

void SignUpTask::run() {
    Globals& globals = Globals::getInstance();
    Crypto& crypto = Crypto::getInstance();
    std::size_t hash = crypto.toHashcode(password);
    std::cout << hash << std::endl;
    Json::Value responseJson;

    if (!globals.signUp(username, hash, email, socket)) { 
        responseJson["action"] = "signup";
        responseJson["success"] = false;
        responseJson["arguments"] = "user already exist";

        // Convert the JSON object to string
        Json::StreamWriterBuilder writer;
        std::string response = Json::writeString(writer, responseJson);

        // Encode the response using Crypto::encode()
        std::string encodedResponse = crypto.encode(response);

        if (socket && socket->is_open()) {
            boost::asio::write(*socket, boost::asio::buffer(encodedResponse));
        }
        else {
            std::cerr << "Socket is invalid or closed!" << std::endl;
        }

        std::cout << "User already exists!" << std::endl;
        return;  
    }

    globals.printSockets();

    responseJson["action"] = "signup";
    responseJson["success"] = true;
    responseJson["arguments"] = "welcome!";

    // Convert the JSON object to string
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
