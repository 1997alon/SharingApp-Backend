#pragma once

#ifndef USER_H
#define USER_H

#include <string>
#include <utility> // for std::move

class User {
private:
    std::string username; 
    size_t password; // Change password to size_t
    std::string email;
    int postCount;

public:
    // Constructor
    User();
    User(const std::string& username, const size_t password, const std::string& email, int postCount = 0);

    // Copy Constructor
    User(const User& other);

    // Copy Assignment
    User& operator=(const User& other);

    // Move Constructor
    User(User&& other) noexcept;

    // Move Assignment
    User& operator=(User&& other) noexcept;

    // Destructor
    ~User();

    // Getters
    std::string getUsername() const;
    size_t getPassword() const;
    std::string getEmail() const;
    int getPostCount() const;

    // Other methods
    void incrementPostCount();
};

#endif // USER_H
