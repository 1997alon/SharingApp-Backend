#include "ConnectorDB.h"
#include <stdexcept>
#include <iostream>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <json/json.h>

ConnectorDB* ConnectorDB::instance = nullptr;

ConnectorDB::ConnectorDB() {
    try {
        driver = sql::mysql::get_driver_instance();
        conn.reset(driver->connect("tcp://127.0.0.1:3306", "root", "1997Alon")); 
        conn->setSchema("wiki"); 
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error while connecting to the database: " << e.what() << std::endl;
        throw std::runtime_error("Database connection failed");
    }
}

ConnectorDB::~ConnectorDB() {
    if (conn) {
        conn->close();
    }
}

ConnectorDB* ConnectorDB::getInstance() {
    if (instance == nullptr) {
        instance = new ConnectorDB();
    }
    return instance;
}
User* ConnectorDB::getUser(const std::string& username, size_t passwordHash) {
    try {
        std::string passwordHashStr = size_tToString(passwordHash);

        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("SELECT username, password, email FROM users WHERE username = ? AND password = ?")
        );
        pstmt->setString(1, username);
        pstmt->setString(2, passwordHashStr);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            std::string fetchedUsername = res->getString("username");
            std::string fetchedPasswordStr = res->getString("password");
            std::string email = res->getString("email");

            // Convert password string back to size_t (optional, since we already have it)
            size_t fetchedPasswordHash = std::stoull(fetchedPasswordStr);

            return new User(fetchedUsername, fetchedPasswordHash, email);
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error while getting user: " << e.what() << std::endl;
    }

    return nullptr; // User not found or error
}


Json::Value ConnectorDB::findUser(const std::string& username, size_t passwordHash) {
    Json::Value result;
    result["action"] = "login";
    result["success"] = false;
    try {
        std::string passwordHashStr = size_tToString(passwordHash);
        // Check if username exists
        std::unique_ptr<sql::PreparedStatement> checkUserStmt(
            conn->prepareStatement("SELECT username, password FROM users WHERE username = ? LIMIT 1")
        );
        checkUserStmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> checkUserRes(checkUserStmt->executeQuery());

        if (!checkUserRes->next()) {
            result["success"] = false;
            result["reason"] = "Username does not exist.";
            return result;
        }
        // Now username exists, compare password
        std::string dbPassword = checkUserRes->getString("password");
        std::string inputPassword = passwordHashStr;

        if (dbPassword == inputPassword) {
            result["success"] = true;
            result["reason"] = "Login successful...";
        }
        else {
            result["success"] = false;
            result["reason"] = "Username is correct, but password is incorrect.";
        }
        return result;
    }
    catch (sql::SQLException& e) {
        result["success"] = false;
        result["reason"] = std::string("SQL Exception: ") + e.what();
        return result;
    }
}

// Method to add a user to the database
bool ConnectorDB::addUser(const std::string& username, size_t passwordHash, const std::string& email) {
    try {
        // Check if username already exists
        std::unique_ptr<sql::PreparedStatement> checkStmt(
            conn->prepareStatement("SELECT username FROM users WHERE username = ? LIMIT 1")
        );
        checkStmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> checkRes(checkStmt->executeQuery());
        std::cout << "after checking user exists\n";
        if (checkRes->next()) {
            // Username already exists
            return false;
        }
        std::cout << passwordHash << std::endl;
        // Username is available, proceed to insert
        std::string passwordHashStr = size_tToString(passwordHash);
        std::cout << "after size_tToString\n";
        std::cout << passwordHashStr<<std::endl;
        std::unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("INSERT INTO users (username, password, email) VALUES (?, ?, ?)")
        );
        pstmt->setString(1, username);
        pstmt->setString(2, passwordHashStr);
        pstmt->setString(3, email);

        pstmt->executeUpdate();
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Error while adding user: " << e.what() << std::endl;
        throw std::runtime_error("Failed to add user to database");
    }
}

bool ConnectorDB::findTopic(const std::string& username, const std::string& topicName) {
    //check that topic exists for search and check if topic does not exist for adding.
    try {
        // Step 1: Verify the user exists using the password hash
        std::unique_ptr<sql::PreparedStatement> userStmt(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userStmt->setString(1, username);

        std::unique_ptr<sql::ResultSet> userRes(userStmt->executeQuery());

        if (!userRes->next()) {
            std::cout << "User not found\n";
            return false; 
        }

        // Step 2: Check if a topic with the given topicName already exists (topicName is UNIQUE)
        std::unique_ptr<sql::PreparedStatement> topicStmt(
            conn->prepareStatement("SELECT 1 FROM topics WHERE topicName = ? LIMIT 1")
        );
        topicStmt->setString(1, topicName);

        std::unique_ptr<sql::ResultSet> topicRes(topicStmt->executeQuery());

        return topicRes->next();  // True if topic exists, false otherwise
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in findTopic: " << e.what() << std::endl;
        return false;
    }
}


bool ConnectorDB::addTopic(const std::string& username, const std::string& topicName, const std::string& author, const std::string& content) {
    try {
        // Step 1: Verify the user exists using the password hash
        std::unique_ptr<sql::PreparedStatement> userStmt(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userStmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userStmt->executeQuery());

        if (!userRes->next()) {
            std::cout << "User not found\n";
            return false;  // User doesn't exist
        }

        int userID = userRes->getInt("userID");

        // Step 2: Check if a topic with the given topicName already exists (topicName is UNIQUE)
        std::unique_ptr<sql::PreparedStatement> topicStmt(
            conn->prepareStatement("SELECT 1 FROM topics WHERE topicName = ? LIMIT 1")
        );
        topicStmt->setString(1, topicName);

        std::unique_ptr<sql::ResultSet> topicRes(topicStmt->executeQuery());

        if (topicRes->next()) {
            std::cout << "Topic already exists\n";
            return false;  // Topic already exists
        }

        // Step 3: Insert the new topic into the database
        std::unique_ptr<sql::PreparedStatement> insertStmt(
            conn->prepareStatement("INSERT INTO topics (topicName, author, userID, content) VALUES (?, ?, ?, ?)")
        );
        insertStmt->setString(1, topicName);
        insertStmt->setString(2, author);
        insertStmt->setInt(3, userID);
        insertStmt->setString(4, content);

        insertStmt->executeUpdate();  // Execute the insert statement

        std::cout << "Topic added successfully\n";
        return true;  // Topic added successfully
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in addTopic: " << e.what() << std::endl;
        return false;
    }
}

void ConnectorDB::addAuthorization(const std::string& username, const std::string& topicName) {
    try {
        // Step 1: Verify the user exists using the password hash
        std::unique_ptr<sql::PreparedStatement> userStmt(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userStmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userStmt->executeQuery());

        if (!userRes->next()) {
            std::cout << "User not found\n";
        }

        int userID = userRes->getInt("userID");

        // Step 2: Check if a topic with the given topicName already exists (topicName is UNIQUE)
        std::unique_ptr<sql::PreparedStatement> authStmt(
            conn->prepareStatement("SELECT topicID FROM topics WHERE topicName = ?")
        );
        authStmt->setString(1, topicName);

        std::unique_ptr<sql::ResultSet> topicRes(authStmt->executeQuery());

        if (topicRes->next()) {
            std::cout << "Topic already exists\n";
        }

        int topicID = topicRes->getInt("topicID");

        std::unique_ptr<sql::PreparedStatement> insertStmt(
            conn->prepareStatement("INSERT INTO authorization (userID, topicID, topicName) VALUES (?, ?, ?)")
        );
        insertStmt->setInt(1, userID);
        insertStmt->setInt(2, topicID);
        insertStmt->setString(3, topicName);

        insertStmt->executeUpdate();
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in addTopic: " << e.what() << std::endl;
    }
    std::cout << "authorization done!\n";
}

Json::Value ConnectorDB::getTopicDetails(const std::string& username, std::string& topicName) {
    Json::Value result;
    result["success"] = false;
    try {
        // Step 1: Verify the user exists using the password hash
        std::unique_ptr<sql::PreparedStatement> userStmt(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userStmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userStmt->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User not found or incorrect password";
            return result;
        }

        int userID = userRes->getInt("userID");

        // Step 2: Get the topic details if it exists for the user
        std::unique_ptr<sql::PreparedStatement> topicStmt(
            conn->prepareStatement("SELECT topicID, author, content, createdAt FROM topics WHERE topicName = ? LIMIT 1")
        );
        topicStmt->setString(1, topicName);
        std::unique_ptr<sql::ResultSet> topicRes(topicStmt->executeQuery());

        if (topicRes->next()) {
            // Topic exists, return the details in JSON
            result["action"] = "getTopicDetails";
            result["success"] = true;
            result["content"] = topicRes->getString("content").asStdString();
            result["author"] = topicRes->getString("author").asStdString();
            result["time"] = topicRes->getString("createdAt").asStdString();
        }
        else {
            result["error"] = "Topic not found for this user";
        }

    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in getTopicDetails: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
    }

    return result;
}


Json::Value ConnectorDB::getAllTopics(const std::string& username) {
    Json::Value result;
    std::cout << "in connector\n";
    result["success"] = false;

    try {
        // Step 1: Verify the user exists using the username
        std::unique_ptr<sql::PreparedStatement> userStmt(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userStmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userStmt->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User not found or incorrect password";
            return result;
        }
        std::cout << "in step1\n";
        // Step 2: Retrieve all topics from the topics table
        std::unique_ptr<sql::PreparedStatement> topicsStmt(
            conn->prepareStatement("SELECT topicName, author, createdAt AS t FROM topics")

        );
        std::unique_ptr<sql::ResultSet> topicsRes(topicsStmt->executeQuery());
        std::cout << "in step2\n";
        // Step 3: Create the topics array
        Json::Value topicsJson(Json::arrayValue);
        int i = 0;
        while (topicsRes->next()){
            i++;
            std::string topicName = topicsRes->getString("topicName");
            std::string author = topicsRes->getString("author");
            std::string t = topicsRes->getString("t");
            
            // Check if topicName, author, and time are not NULL or empty
            if (!topicName.empty() && !author.empty() && !t.empty()) {
                std::cout << i << std::endl;
                Json::Value topicJson;
                topicJson["topic"] = topicName;
                topicJson["author"] = author;
                topicJson["time"] = t;
                topicsJson.append(topicJson);
            }
        }
        std::cout << "in step3\n";
        // Step 4: Populate the result JSON
        result["success"] = true;
        result["action"] = "getAllTopics";
        result["topics"] = topicsJson;

    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in getAllTopics: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
    }

    return result;
}


Json::Value ConnectorDB::getUserTopics(const std::string& username) {
    Json::Value result;
    result["action"] = "getUserTopics";
    result["success"] = false;
    Json::Value topicsJson(Json::arrayValue);

    try {
        // Step 1: Verify the user exists using the username
        std::unique_ptr<sql::PreparedStatement> userST(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userST->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userST->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User not found";
            return result;
        }

        int userID = userRes->getInt("userID");

        // Step 2: Retrieve all topics for the user from the topics table
        std::unique_ptr<sql::PreparedStatement> topicsStmt(
            conn->prepareStatement("SELECT topicName, author, createdAt FROM topics WHERE userID = ?")
        );
        topicsStmt->setInt(1, userID);
        std::unique_ptr<sql::ResultSet> topicsRes(topicsStmt->executeQuery());

        while (topicsRes->next()) {
            std::string topicName = topicsRes->getString("topicName");
            std::string author = topicsRes->getString("author");
            std::string time = topicsRes->getString("createdAt");

            // Check if topicName, author, and time are not NULL or empty
            if (!topicName.empty() && !author.empty() && !time.empty()) {
                Json::Value topicJson;
                topicJson["topic"] = topicName;
                topicJson["author"] = author;
                topicJson["time"] = time;

                topicsJson.append(topicJson);
            }
        }

        result["success"] = true;
        result["topics"] = topicsJson;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in getUserTopics: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
    }

    return result;
}

Json::Value ConnectorDB::checkAuthorizationTopic(const std::string& username, const std::string& topicName) {
    Json::Value result;
    result["action"] = "checkAuthorizationTopic";
    result["success"] = false;
    try {
        std::unique_ptr<sql::PreparedStatement> userST(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userST->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userST->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User not found";
            return result;
        }

        int userID = userRes->getInt("userID");
        std::unique_ptr<sql::PreparedStatement> authorST(
            conn->prepareStatement("SELECT topicName FROM authorization WHERE userID = ? AND topicName = ?")
        );
        authorST->setInt(1, userID);
        authorST->setString(2, topicName);
        std::unique_ptr<sql::ResultSet> AuthorRes(authorST->executeQuery());

        if (AuthorRes->next()) {
            result["success"] = true;
        }
        else {
            result["error"] = "no Authorization";
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in checkAuthorizationTopic: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
    }
    return result;
}

Json::Value ConnectorDB::getAuthorizationTopics(const std::string& username) {
    Json::Value result;
    result["action"] = "getAuthorizationTopics";
    result["success"] = false;
    Json::Value topicsJson(Json::arrayValue);

    try {
        std::unique_ptr<sql::PreparedStatement> userST(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userST->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userST->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User not found";
            return result;
        }
        int userID = userRes->getInt("userID");
        std::unique_ptr<sql::PreparedStatement> topicsStmt(
            conn->prepareStatement("SELECT topicName FROM authorization WHERE userID = ?")
        );
        topicsStmt->setInt(1, userID);
        std::unique_ptr<sql::ResultSet> topicsRes(topicsStmt->executeQuery());

        while (topicsRes->next()) {
            std::string topicName = topicsRes->getString("topicName");
            if (!topicName.empty()) {
                Json::Value topicJson;
                topicJson["topic"] = topicName;
                topicsJson.append(topicJson);
            }
        }
        result["success"] = true;
        result["topics"] = topicsJson;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in getAuthorizationTopics: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
    }
    return result;
}

Json::Value ConnectorDB::askForAuthorizationTopic(const std::string& username, const std::string& topicName) {
    Json::Value result;
    result["action"] = "askForAuthorizationTopic";
    result["success"] = false;

    try {
        // Step 1: Get userID of the requester (sender)
        std::unique_ptr<sql::PreparedStatement> userST(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userST->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userST->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User not found";
            return result;
        }
        int userID = userRes->getInt("userID");
        // Step 2: Get userID and topicID of the topic owner (receiver)
        std::unique_ptr<sql::PreparedStatement> topicST(
            conn->prepareStatement("SELECT topicID, userID FROM topics WHERE topicName = ?")
        );
        topicST->setString(1, topicName);
        std::unique_ptr<sql::ResultSet> topicRes(topicST->executeQuery());

        if (!topicRes->next()) {
            result["error"] = "Topic not found";
            return result;
        }
        int topicID = topicRes->getInt("topicID");
        int userID2 = topicRes->getInt("userID");

        if (userID == userID2) {
            result["error"] = "You already own this topic";
            return result;
        }
        // Step 3: Check if authorization already exists
        std::unique_ptr<sql::PreparedStatement> checkAuthST(
            conn->prepareStatement("SELECT * FROM authorization WHERE userID = ? AND topicID = ?")
        );
        checkAuthST->setInt(1, userID);
        checkAuthST->setInt(2, topicID);
        std::unique_ptr<sql::ResultSet> authRes(checkAuthST->executeQuery());

        if (authRes->next()) {
            result["error"] = "You already have authorization for this topic";
            return result;
        }
        // Step 4: Check if a similar pending authorization message already exists
        std::unique_ptr<sql::PreparedStatement> checkMsgST(
            conn->prepareStatement(
                "SELECT * FROM messages "
                "WHERE sender = ? AND receiver = ? AND kind = 'authorization' "
                "AND topicName = ? AND status IN ('sent', 'delivered') AND isDeleted = FALSE"
            )
        );
        checkMsgST->setInt(1, userID);
        checkMsgST->setInt(2, userID2);
        checkMsgST->setString(3, topicName);
        std::unique_ptr<sql::ResultSet> msgRes(checkMsgST->executeQuery());

        if (msgRes->next()) {
            result["error"] = "you are waiting to approve already for this topic";  // matching unread message already exists
            return result;
        }
        // Step 5: Insert the new authorization request message
        std::string content = "Can I get authorization to edit your topic '" + topicName + "'?";

        std::unique_ptr<sql::PreparedStatement> insertMsg(
            conn->prepareStatement(
                "INSERT INTO messages (sender, receiver, kind, messageContent, topicName) "
                "VALUES (?, ?, 'authorization', ?, ?)"
            )
        );
        insertMsg->setInt(1, userID);
        insertMsg->setInt(2, userID2);
        insertMsg->setString(3, content);
        insertMsg->setString(4, topicName);
        insertMsg->execute();

        result["success"] = true;
        result["message"] = "Authorization request sent";
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in askForAuthorizationTopic: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
    }
    return result;
}

Json::Value ConnectorDB::getMessages(std::string& username) {
    Json::Value result;
    result["action"] = "getMessages";
    result["success"] = false;
    Json::Value messages(Json::arrayValue);

    try {
        // Step 1: Get userID of the receiver
        std::unique_ptr<sql::PreparedStatement> userST(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userST->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userST->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User not found";
            return result;
        }
        int userID = userRes->getInt("userID");

        // Step 2: Get all non-deleted messages sent to this user
        std::unique_ptr<sql::PreparedStatement> msgST(
            conn->prepareStatement(
                "SELECT m.messageID, m.sender, m.kind, m.time, m.status, m.messageContent, m.topicName "
                "FROM messages m "
                "JOIN users u ON m.sender = u.userID "
                "WHERE m.receiver = ? AND m.isDeleted = FALSE "
                "ORDER BY m.time DESC"
            )
        );
        msgST->setInt(1, userID);
        std::unique_ptr<sql::ResultSet> msgRes(msgST->executeQuery());
        while (msgRes->next()) {
            Json::Value msg;
            msg["messageID"] = msgRes->getInt("messageID");
            msg["sender"] = msgRes->getInt("sender");
            msg["kind"] = static_cast<std::string>(msgRes->getString("kind"));
            msg["time"] = static_cast<std::string>(msgRes->getString("time"));
            msg["status"] = static_cast<std::string>(msgRes->getString("status"));
            msg["topic"] = static_cast<std::string>(msgRes->getString("topicName"));

            messages.append(msg);
        }

        result["success"] = true;
        result["messages"] = messages;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in getMessages: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
    }

    return result;
}

Json::Value ConnectorDB::openAMessage(std::string& username, int sender, int messageID) {
    Json::Value result;
    result["action"] = "openAMessage";
    result["success"] = false;

    try {
        // Step 1: Get userID of the receiver
        std::unique_ptr<sql::PreparedStatement> userST(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userST->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userST->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User not found";
            return result;
        }
        int receiverID = userRes->getInt("userID");

        // Step 2: Check if the message exists
        std::unique_ptr<sql::PreparedStatement> msgST(
            conn->prepareStatement(
                "SELECT messageContent, time, status, answered "
                "FROM messages "
                "WHERE messageID = ? AND sender = ? AND receiver = ? AND isDeleted = FALSE"
            )
        );
        msgST->setInt(1, messageID);
        msgST->setInt(2, sender);        // sender is already userID
        msgST->setInt(3, receiverID);    // receiver is the logged-in user

        std::unique_ptr<sql::ResultSet> msgRes(msgST->executeQuery());

        if (!msgRes->next()) {
            result["error"] = "Message not found or incorrect sender/receiver";
            return result;
        }

        // Step 3: Retrieve the content and time of the message
        std::string messageContent = msgRes->getString("messageContent");
        std::string time = msgRes->getString("time");
        std::string status = msgRes->getString("status");
        std::string answered = msgRes->getString("answered");
        // Step 4: Update the message status to 'read'
        std::unique_ptr<sql::PreparedStatement> updateST(
            conn->prepareStatement("UPDATE messages SET status = 'read' WHERE messageID = ?")
        );
        updateST->setInt(1, messageID);
        updateST->executeUpdate();

        // Step 5: Add the data to the result
        result["success"] = true;
        result["messageContent"] = messageContent;
        result["time"] = time;
        result["answered"] = answered;
        result["status"] = "read"; // Status is now updated to read
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in openAMessage: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
    }

    return result;
}

Json::Value ConnectorDB::approveAuthorizationTopic(std::string& username, int sender, int messageID, bool approved) {
    Json::Value result;
    result["action"] = "approveAuthorizationTopic";
    result["success"] = false;
    try {
        // Step 1: Get receiverID from username
        std::unique_ptr<sql::PreparedStatement> userST(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userST->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userST->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User not found";
            return result;
        }
        int receiverID = userRes->getInt("userID");

        // Step 2: Get message details from messages table
        std::unique_ptr<sql::PreparedStatement> msgStmt(
            conn->prepareStatement("SELECT topicName FROM messages WHERE messageID = ? AND sender = ? AND receiver = ? AND kind = 'authorization'")
        );
        msgStmt->setInt(1, messageID);
        msgStmt->setInt(2, sender);
        msgStmt->setInt(3, receiverID);
        std::unique_ptr<sql::ResultSet> msgRes(msgStmt->executeQuery());

        if (!msgRes->next()) {
            result["error"] = "Authorization message not found or invalid";
            return result;
        }
        std::string topicName = msgRes->getString("topicName");

        // Step 3: Get topicID by topicName and receiver(who created the topic)
        std::unique_ptr<sql::PreparedStatement> topicStmt(
            conn->prepareStatement("SELECT topicID FROM topics WHERE topicName = ? AND userID = ?")
        );
        topicStmt->setString(1, topicName);
        topicStmt->setInt(2, receiverID);
        std::unique_ptr<sql::ResultSet> topicRes(topicStmt->executeQuery());

        if (!topicRes->next()) {
            result["error"] = "Topic not found for this user";
            return result;
        }
        int topicID = topicRes->getInt("topicID");

        // Step 4: Check if authorization already exists
        std::unique_ptr<sql::PreparedStatement> authCheckStmt(
            conn->prepareStatement("SELECT * FROM wiki.authorization WHERE userID = ? AND topicID = ?")
        );
        authCheckStmt->setInt(1, sender);
        authCheckStmt->setInt(2, topicID);
        std::unique_ptr<sql::ResultSet> authRes(authCheckStmt->executeQuery());

        if (authRes->next()) {
            result["error"] = "Authorization already exists";
            return result;
        }

        if (approved) {
            // Step 5a: Insert into wiki.authorization
            std::unique_ptr<sql::PreparedStatement> insertAuth(
                conn->prepareStatement("INSERT INTO wiki.authorization (userID, topicID, topicName) VALUES (?, ?, ?)")
            );
            insertAuth->setInt(1, sender);
            insertAuth->setInt(2, topicID);
            insertAuth->setString(3, topicName);
            insertAuth->executeUpdate();

            // Step 5b: Send "approved" message to sender
            std::unique_ptr<sql::PreparedStatement> msgInsert(
                conn->prepareStatement("INSERT INTO messages (sender, receiver, kind, messageContent, topicName, status) VALUES (?, ?, 'regular', ?, ?, 'sent')")
            );
            msgInsert->setInt(1, receiverID);
            msgInsert->setInt(2, sender);
            msgInsert->setString(3, "Authorization approved for topic: " + topicName);
            msgInsert->setString(4, topicName);
            msgInsert->executeUpdate();

            // Step 5c: Update answered field to 'approved' in messages table
            std::unique_ptr<sql::PreparedStatement> updateAnsweredStmt(
                conn->prepareStatement("UPDATE messages SET answered = 'approved' WHERE messageID = ?")
            );
            updateAnsweredStmt->setInt(1, messageID);
            updateAnsweredStmt->executeUpdate();
        }
        else {
            // Step 5: Send "denied" message to sender
            std::unique_ptr<sql::PreparedStatement> denyMsgInsert(
                conn->prepareStatement("INSERT INTO messages (sender, receiver, kind, messageContent, topicName, status) VALUES (?, ?, 'regular', ?, ?, 'sent')")
            );
            denyMsgInsert->setInt(1, receiverID);
            denyMsgInsert->setInt(2, sender);
            denyMsgInsert->setString(3, "Authorization request denied for topic: " + topicName);
            denyMsgInsert->setString(4, topicName);
            denyMsgInsert->executeUpdate();

            // Step 5b: Update answered field to 'denied' in messages table
            std::unique_ptr<sql::PreparedStatement> updateAnsweredStmt(
                conn->prepareStatement("UPDATE messages SET answered = 'denied' WHERE messageID = ?")
            );
            updateAnsweredStmt->setInt(1, messageID);
            updateAnsweredStmt->executeUpdate();
        }

        result["success"] = true;
        return result;
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in approveAuthorizationTopic: " << e.what()
            << " (MySQL error code: " << e.getErrorCode()
            << ", SQLState: " << e.getSQLState() << ")" << std::endl;
        result["error"] = "Database error occurred";
    }
    return result;
}


Json::Value ConnectorDB::updateTopic(std::string& username, std::string& topicName, std::string& content) {
    Json::Value result;
    result["action"] = "updateTopic";
    result["success"] = false;
    try {
        // Step 1: Check if user exists
        std::unique_ptr<sql::PreparedStatement> userST(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userST->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userST->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User does not exist";
            return result;
        }
        int userID = userRes->getInt("userID");

        // Step 2: Check if topic exists
        std::unique_ptr<sql::PreparedStatement> topicST(
            conn->prepareStatement("SELECT topicID FROM topics WHERE topicName = ?")
        );
        topicST->setString(1, topicName);
        std::unique_ptr<sql::ResultSet> topicRes(topicST->executeQuery());

        if (!topicRes->next()) {
            result["error"] = "Topic does not exist";
            return result;
        }
        int topicID = topicRes->getInt("topicID");

        // Step 3: Check authorization
        std::unique_ptr<sql::PreparedStatement> authST(
            conn->prepareStatement("SELECT 1 FROM authorization WHERE userID = ? AND topicID = ?")
        );
        authST->setInt(1, userID);
        authST->setInt(2, topicID);
        std::unique_ptr<sql::ResultSet> authRes(authST->executeQuery());

        if (!authRes->next()) {
            result["error"] = "User not authorized for this topic";
            return result;
        }

        // Step 4: Update topic content
        std::unique_ptr<sql::PreparedStatement> updateST(
            conn->prepareStatement("UPDATE topics SET content = ? WHERE topicID = ?")
        );
        updateST->setString(1, content);
        updateST->setInt(2, topicID);
        int rowsAffected = updateST->executeUpdate();
        if (rowsAffected > 0) {
            result["success"] = true;
            result["message"] = "Topic content updated successfully";
        }
        else {
            result["error"] = "Content update failed";
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in updateTopic: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
    }
    return result;
}

Json::Value ConnectorDB::menu(std::string& username) {
    Json::Value result;
    result["action"] = "menu";
    result["success"] = false;
    try {
        // Step 1: Check if user exists
        std::unique_ptr<sql::PreparedStatement> userST(
            conn->prepareStatement("SELECT userID FROM users WHERE username = ?")
        );
        userST->setString(1, username);
        std::unique_ptr<sql::ResultSet> userRes(userST->executeQuery());

        if (!userRes->next()) {
            result["error"] = "User does not exist";
            return result;
        }
        int userID = userRes->getInt("userID");

        // Step 2: Check for unread messages
        std::unique_ptr<sql::PreparedStatement> msgST(
            conn->prepareStatement(
                "SELECT COUNT(*) AS unreadCount FROM messages "
                "WHERE receiver = ? AND status != 'read'"
            )
        );
        msgST->setInt(1, userID);
        std::unique_ptr<sql::ResultSet> msgRes(msgST->executeQuery());
        if (msgRes->next()) {
            int count = msgRes->getInt("unreadCount");
            result["newMessages"] = false;
            std::cout << count << std::endl;
            if (count > 0) {
                result["newMessages"] = true;
            }
            result["success"] = true;
        }
        else {
            // Should not happen, but just in case
            result["newMessages"] = false;
            result["error"] = "Failed to fetch messages info";
        }
    }
    catch (sql::SQLException& e) {
        std::cerr << "SQLException in menu: " << e.what() << std::endl;
        result["error"] = "Database error occurred";
        result["newMessages"] = false;
    }
    return result;
}

std::string ConnectorDB::size_tToString(size_t value) {
    return std::to_string(value); 
}
