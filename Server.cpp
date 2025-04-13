#include "Server.h"
#include <iostream>
#include <thread>
#include <memory>
#include <json/json.h>
#include <boost/asio.hpp>
#include "SignUpTask.h"
#include "LoginTask.h"
#include "AddTopicTask.h"
#include "SearchTopicTask.h"
#include "GetTopicDetailsTask.h"
#include "GetAllTopicsTask.h"
#include "GetUserTopicsTask.h"
#include "CheckAuthorizationTopicTask.h"
#include "GetAuthorizationTopicsTask.h"
#include "AskForAuthorizationTopicTask.h"
#include "GetMessagesTask.h"
#include "OpenAMessageTask.h"
#include "ApproveAuthorizationTopicTask.h"
#include "UpdateTopicTask.h"
#include "MenuTask.h"

using boost::asio::ip::tcp;

Server::Server(int port, int numThreads)
    : port(port),
    acceptor(io_context, tcp::endpoint(tcp::v4(), port)),
    pool(numThreads) {}

void Server::start_accept() {
    std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(io_context);

    acceptor.async_accept(*socket, [this, socket](const boost::system::error_code& error) {
        if (!error) {
            std::cout << "New client connected\n";
            handle_read(socket);  // Start reading data from this socket
        }
        else {
            std::cerr << "Error accepting connection: " << error.message() << std::endl;
        }

        // Keep accepting new connections even if the current one is being processed
        start_accept();
        });
}

void Server::handle_read(std::shared_ptr<tcp::socket> socket) {
    std::shared_ptr<boost::asio::streambuf> buf = std::make_shared<boost::asio::streambuf>();

    boost::asio::async_read(*socket, *buf, boost::asio::transfer_at_least(1),
        [this, socket, buf](const boost::system::error_code& error, std::size_t bytes_transferred) {
            if (!error) {
                std::istream is(buf.get());  // Use the shared buffer
                std::string encrypted_message;

                if (std::getline(is, encrypted_message, '\0')) {
                    std::cout << "Received message: " << encrypted_message << std::endl;

                    // Decode the encrypted message using Crypto::decode
                    Crypto& crypto = Crypto::getInstance();
                    std::string decrypted_message = crypto.decode(encrypted_message);
                    std::cout << "Decrypted message: " << decrypted_message << std::endl;

                    // Parse the JSON message
                    Json::Value root;
                    Json::Reader reader;
                    if (reader.parse(decrypted_message, root)) {
                        std::string action = root["action"].asString();
                        std::cout << "Action received: " << action << std::endl;

                        if (action == "signUp") {
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::string password = arguments["password"].asString();
                            std::string email = arguments["email"].asString();
                            std::shared_ptr<Task> task = std::make_shared<SignUpTask>(username, password, email, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "login") {
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::string password = arguments["password"].asString();
                            std::shared_ptr<Task> task = std::make_shared<LoginTask>(username, password, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "addTopic") {
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::string topicName = arguments["topicName"].asString();
                            std::string author = arguments["author"].asString();
                            std::string content = arguments["content"].asString();
                            std::shared_ptr<Task> task = std::make_shared<AddTopicTask>(username, topicName, author, content, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "searchTopic") {
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::string topicName = arguments["topicName"].asString();
                            std::shared_ptr<Task> task = std::make_shared<SearchTopicTask>(username, topicName, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "getTopicDetails") {
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::string topicName = arguments["topicName"].asString();
                            std::shared_ptr<Task> task = std::make_shared<GetTopicDetailsTask>(username, topicName, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "getAllTopics") {
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::shared_ptr<Task> task = std::make_shared<GetAllTopicsTask>(username, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "getUserTopics") { //only of you..
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::shared_ptr<Task> task = std::make_shared<GetUserTopicsTask>(username, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "checkAuthorizationTopic") {
                            // user ask if he can edit a specific topic
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::string topicName = arguments["topicName"].asString();
                            std::shared_ptr<Task> task = std::make_shared<CheckAuthorizationTopicTask>(username, topicName, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "getAuthorizationTopics") { //only for you
                            // user get all the authorizations he has (include his topics)
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::shared_ptr<Task> task = std::make_shared<GetAuthorizationTopicsTask>(username, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "getMessages") { // only for you
                            // get messages change the enum kind to deliver
                            // will send back sender, kind of message, time and deliver/read for each message
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::shared_ptr<Task> task = std::make_shared<GetMessagesTask>(username, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "openAMessage") {
                            //change the enum kind to read
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            int sender = arguments["sender"].asInt();
                            int messageID = arguments["messageID"].asInt();
                            std::shared_ptr<Task> task = std::make_shared<OpenAMessageTask>(username, sender, messageID, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "updateTopic") { // only for Authorizations.. need to check authorizations
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::string topicName = arguments["topicName"].asString();
                            std::string content = arguments["content"].asString();
                            std::shared_ptr<Task> task = std::make_shared<UpdateTopicTask>(username, topicName, content, socket);
                            pool.enqueue(task);
                        }

                        else if (action == "approveAuthorizationTopic") {
                            //send to the sender a regular message that it approve or not
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            int sender = arguments["sender"].asInt();
                            int messageID = arguments["messageID"].asInt();
                            bool approved = arguments["approved"].asBool();
                            std::shared_ptr<Task> task = std::make_shared<ApproveAuthorizationTopicTask>(username, sender,messageID, approved, socket);
                            pool.enqueue(task);

                        }
                        else if (action == "askForAuthorizationTopic") { 
                            // i will find the userID from topicName and update his messages box in the data
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::string topicName = arguments["topicName"].asString();
                            std::shared_ptr<Task> task = std::make_shared<AskForAuthorizationTopicTask>(username, topicName, socket);
                            pool.enqueue(task);
                        }
                        else if (action == "menu") {
                            // send him the messages he didnt read yet with a number too
                            Json::Value arguments = root["arguments"];
                            std::string username = arguments["username"].asString();
                            std::shared_ptr<Task> task = std::make_shared<MenuTask>(username, socket);
                            pool.enqueue(task);
                        }
                    }
                }
                else {
                    std::cerr << "Invalid message format, missing null terminator or malformed data." << std::endl;
                }

                // Continue reading data from the socket if still open
                handle_read(socket);
            }
            else {
                if (error == boost::asio::error::eof) {
                    // Graceful connection closure, no need to throw an error
                    std::cout << "Connection closed gracefully.\n";
                }
                else {
                    std::cerr << "Error reading data: " << error.message() << std::endl;
                }
            }
        });
}


void Server::run() {
    std::cout << "Server listening on port " << port << "...\n";
    start_accept();  // Start accepting connections asynchronously

    // Run the io_context in a separate thread
    std::thread io_thread([this]() {
        io_context.run();  // Run the asynchronous event loop
        });

    io_thread.join();  // Wait for the IO thread to finish
}

int main() {
    Server server(12345, 4);  // Example port and number of threads
    server.run();
    return 0;
}
