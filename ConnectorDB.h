#pragma once

#ifndef CONNECTOR_DB_H
#define CONNECTOR_DB_H
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <json/json.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <vector>
#include <string>
#include "User.h" // Assuming this is your User class

class ConnectorDB {
private:
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> conn;

    // Singleton instance
    static ConnectorDB* instance;

    // Private constructor
    ConnectorDB();

public:
    // Delete copy/move constructors & assignment
    ConnectorDB(const ConnectorDB&) = delete;
    ConnectorDB& operator=(const ConnectorDB&) = delete;
    ConnectorDB(ConnectorDB&&) = delete;
    ConnectorDB& operator=(ConnectorDB&&) = delete;

    // Destructor
    ~ConnectorDB();

    // Singleton access
    static ConnectorDB* getInstance();

    // Methods
    User* getUser(const std::string& username, size_t passwordHash);
    Json::Value findUser(const std::string& username, size_t passwordHash);
    bool addUser(const std::string& username, size_t passwordHash, const std::string& email);
    bool findTopic(const std::string& username, const std::string& topicName);
    Json::Value getTopicDetails(const std::string& username, std::string& topicName);
    Json::Value getUserTopics(const std::string& username);
    Json::Value checkAuthorizationTopic(const std::string& username, const std::string& topicName);
    Json::Value getAuthorizationTopics(const std::string& username);
    Json::Value askForAuthorizationTopic(const std::string& username, const std::string& topicName);
    Json::Value getAllTopics(const std::string& username);
    Json::Value getMessages(std::string& username);
    Json::Value openAMessage(std::string& username, int sender, int messageID);
    Json::Value approveAuthorizationTopic(std::string& username, int sender, int messageID, bool approved);
    Json::Value updateTopic(std::string& username, std::string& topicName, std::string& content);
    Json::Value menu(std::string& username);
    bool addTopic(const std::string& username, const std::string& topicName, const std::string& author, const std::string& content);
    void addAuthorization(const std::string& username, const std::string& topicName);
    std::string size_tToString(size_t value);
};

#endif // CONNECTOR_DB_H
