#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

#include <unordered_map>
#include <vector>
#include <boost/asio.hpp>
#include "User.h"
#include <memory>
#include <json/json.h>

class Globals {
public:
    static Globals& getInstance();

    bool signUp(const std::string& username, const size_t& passwordHash, const std::string& email, std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    std::shared_ptr<boost::asio::ip::tcp::socket> getSocketByPasswordHash(const size_t& passwordHash);
    User* getUserBySocket(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    const std::vector<User>& getAllUsers() const;
    Json::Value findUser(const std::string& username, size_t passwordHash);
    bool findTopic(const std::string& username, const std::string& topicName);
    bool addTopic(const std::string username, const std::string& topicName, const std::string& author, const std::string& content);
    User* getUser(const std::string& username, const size_t passwordHash);
    void printUsers() const;
    void printSockets() const;
    User* getUserByHash(size_t passwordHash) const;
    Json::Value getUserTopics(const std::string& username);
    void addAuthorization(const std::string& username, const std::string& topicName);
    Json::Value login(std::string& username, const size_t& passwordHash, std::shared_ptr<boost::asio::ip::tcp::socket>& socket);
    Json::Value getTopicDetails(const std::string& username, std::string& topicName);
    Json::Value getAllTopics(const std::string& username);
    Json::Value checkAuthorizationTopic(const std::string& username, const std::string& topicName);
    Json::Value getAuthorizationTopics(const std::string& username);
    Json::Value askForAuthorizationTopic(const std::string& username, const std::string& topicName);
    Json::Value getMessages(std::string& username);
    Json::Value openAMessage(std::string& username, int sender, int messageID);
    Json::Value approveAuthorizationTopic(std::string& username, int sender, int messageID, bool approved);
    Json::Value updateTopic(std::string& username, std::string& topicName, std::string& content);
    Json::Value menu(std::string& username);

private:
    Globals() = default;

    std::unordered_map<size_t, User> passwordToUser;
    std::unordered_map<size_t, std::shared_ptr<boost::asio::ip::tcp::socket>> passwordToSocket;
    std::unordered_map<std::shared_ptr<boost::asio::ip::tcp::socket>, size_t> socketToPassword;
    std::vector<User> users;

    void addToHashMaps(const size_t& passwordHash, User& user, std::shared_ptr<boost::asio::ip::tcp::socket> socket);
};

#endif // GLOBALS_H
