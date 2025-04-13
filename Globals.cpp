#include "Globals.h"
#include <iostream>
#include "ConnectorDB.h"

Globals& Globals::getInstance() {
    static Globals instance;
    return instance;
}

bool Globals::signUp(const std::string& username, const size_t& passwordHash, const std::string& email, std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
    User newUser(username, passwordHash, email); 
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->addUser(username, passwordHash, email);
}

Json::Value Globals::login(std::string& username, const size_t& passwordHash, std::shared_ptr<boost::asio::ip::tcp::socket>& socket) {
    Json::Value loginResult = findUser(username, passwordHash);
    if (loginResult["success"].asBool()) {
        User* newUser = getUser(username, passwordHash);
        if (newUser == nullptr) {
            loginResult["success"] = false;
            loginResult["reason"] = "getUser returned null.";
            return loginResult;
        }
        addToHashMaps(passwordHash, *newUser, socket);
        users.push_back(*newUser);
        delete newUser;

        return loginResult;
    }
    return loginResult;
}

User* Globals::getUser(const std::string& username, const size_t passwordHash) {
    ConnectorDB* db = ConnectorDB::getInstance();
    User* newUser = db->getUser(username, passwordHash);
    return newUser;
}

void Globals::addToHashMaps(const size_t& passwordHash, User& user, std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
    passwordToUser[passwordHash] = user;
    passwordToSocket[passwordHash] = socket;
    socketToPassword[socket] = passwordHash;
}

std::shared_ptr<boost::asio::ip::tcp::socket> Globals::getSocketByPasswordHash(const size_t& passwordHash) {
    auto it = passwordToSocket.find(passwordHash);
    if (it != passwordToSocket.end()) {
        return it->second;
    }
    return nullptr;
}

User* Globals::getUserBySocket(std::shared_ptr<boost::asio::ip::tcp::socket> socket) {
    auto it = socketToPassword.find(socket);
    if (it != socketToPassword.end()) {
        size_t passwordHash = it->second;
        auto userIt = passwordToUser.find(passwordHash);
        if (userIt != passwordToUser.end()) {
            return new User(userIt->second);
        }
    }
    return nullptr;
}

const std::vector<User>& Globals::getAllUsers() const {
    return users;
}

Json::Value Globals::findUser(const std::string& username, size_t passwordHash) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->findUser(username, passwordHash);
}




void Globals::printUsers() const {
    for (const auto& user : users) {
        std::cout << "User: " << user.getUsername() << ", Email: " << user.getEmail() << std::endl;
    }
}

void Globals::printSockets() const {
    for (const auto& pair : passwordToSocket) {
        const size_t& passwordHash = pair.first;
        const auto& socket = pair.second;
        auto userIt = passwordToUser.find(passwordHash);
        if (userIt != passwordToUser.end()) {
            const User& user = userIt->second;
            std::cout << "Socket for User: " << user.getUsername() << " (Email: " << user.getEmail() << ") - Socket: " << socket.get() << std::endl;
        }
    }
}

User* Globals::getUserByHash(size_t passwordHash) const {
    auto userIt = passwordToUser.find(passwordHash);
    if (userIt != passwordToUser.end()) {
        return new User(userIt->second);
    }
    return nullptr;
}


bool Globals::findTopic(const std::string& username, const std::string& topicName) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->findTopic(username, topicName);
}

bool Globals::addTopic(const std::string username, const std::string& topicName, const std::string& author, const std::string& content) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->addTopic(username, topicName, author, content);
}

void Globals::addAuthorization(const std::string& username, const std::string& topicName) {
    ConnectorDB* db = ConnectorDB::getInstance();
    db->addAuthorization(username, topicName); // Calling the method from ConnectorDB
}
Json::Value Globals::getTopicDetails(const std::string& username, std::string& topicName) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->getTopicDetails(username, topicName);
}

Json::Value Globals::getAllTopics(const std::string& username) {
    ConnectorDB* db = ConnectorDB::getInstance();
    std::cout << "in global\n";
    return db->getAllTopics(username);
}

Json::Value Globals::getUserTopics(const std::string& username) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->getUserTopics(username);
}

Json::Value Globals::checkAuthorizationTopic(const std::string& username, const std::string& topicName) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->checkAuthorizationTopic(username, topicName);
}
Json::Value Globals::getAuthorizationTopics(const std::string& username) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->getAuthorizationTopics(username);
}

Json::Value Globals::askForAuthorizationTopic(const std::string& username, const std::string& topicName) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->askForAuthorizationTopic(username, topicName);
}

Json::Value Globals::getMessages(std::string& username) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->getMessages(username);
}

Json::Value Globals::openAMessage(std::string& username, int sender, int messageID) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->openAMessage(username, sender, messageID);
}

Json::Value Globals::approveAuthorizationTopic(std::string& username, int sender, int messageID, bool approved) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->approveAuthorizationTopic(username, sender, messageID, approved);
}

Json::Value Globals::updateTopic(std::string& username, std::string& topicName, std::string& content) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->updateTopic(username, topicName, content);
}

Json::Value Globals::menu(std::string& username) {
    ConnectorDB* db = ConnectorDB::getInstance();
    return db->menu(username);
}

