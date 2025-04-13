#include "Crypto.h"

// Define the static variables
Crypto* Crypto::instance = nullptr;
std::mutex Crypto::mutex;

// Singleton instance getter
Crypto& Crypto::getInstance() {
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr) {
        instance = new Crypto();
    }
    return *instance;
}

std::size_t Crypto::toHashcode(const std::string& username) const {
    // Hash the username string and return the hash code
    std::hash<std::string> hasher;
    return hasher(username);
}

std::string Crypto::encode(const std::string& message) const {
    std::string encodedMessage;

    // Convert each character to its ASCII value and store as a string
    for (char c : message) {
        encodedMessage += std::to_string(static_cast<int>(c)) + " ";  // Convert char to ASCII and add space separator
    }

    // Append a null character to signify the end of the message
    encodedMessage += std::to_string(static_cast<int>(messageDelimiter));

    return encodedMessage;
}

std::string Crypto::decode(const std::string& message) const {
    std::string decodedMessage;
    size_t pos = 0;
    size_t spacePos;

    // Decode the message by converting ASCII values back to characters
    while ((spacePos = message.find(' ', pos)) != std::string::npos) {
        std::string asciiValueStr = message.substr(pos, spacePos - pos);
        int asciiValue = std::stoi(asciiValueStr);
        decodedMessage += static_cast<char>(asciiValue);  // Convert ASCII value back to character
        pos = spacePos + 1;
    }

    // Remove the null character at the end if present
    if (decodedMessage.back() == messageDelimiter) {
        decodedMessage.pop_back();
    }

    return decodedMessage;
}
