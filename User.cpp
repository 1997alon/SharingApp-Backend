#include "User.h"

// Constructor
User::User() : username(""), password(0), email(""), postCount(0) {}

User::User(const std::string& username, const size_t password, const std::string& email, int postCount)
    : username(username), password(password), email(email), postCount(postCount) {}

// Copy Constructor
User::User(const User& other)
    : username(other.username), password(other.password), email(other.email), postCount(other.postCount) {}

// Copy Assignment
User& User::operator=(const User& other) {
    if (this != &other) {
        username = other.username;
        password = other.password;
        email = other.email;
        postCount = other.postCount;
    }
    return *this;
}

// Move Constructor
User::User(User&& other) noexcept
    : username(std::move(other.username)), password(other.password),
    email(std::move(other.email)), postCount(other.postCount) {
    other.password = 0;
    other.postCount = 0;
}

// Move Assignment
User& User::operator=(User&& other) noexcept {
    if (this != &other) {
        username = std::move(other.username);
        password = other.password;
        email = std::move(other.email);
        postCount = other.postCount;

        other.password = 0;
        other.postCount = 0;
    }
    return *this;
}

// Destructor
User::~User() = default;

// Getters
std::string User::getUsername() const {
    return username;
}

size_t User::getPassword() const {
    return password;
}

std::string User::getEmail() const {
    return email;
}

int User::getPostCount() const {
    return postCount;
}

void User::incrementPostCount() {
    ++postCount;
}
