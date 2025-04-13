#pragma once
#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>
#include <functional>  // for std::hash
#include <mutex>

class Crypto {
private:
    // Private constructor (Singleton)
    Crypto() = default;

    // Deleted copy/move constructors
    Crypto(const Crypto&) = delete;
    Crypto& operator=(const Crypto&) = delete;
    Crypto(Crypto&&) = delete;
    Crypto& operator=(Crypto&&) = delete;

    // For thread-safe singleton
    static std::mutex mutex;
    static Crypto* instance;

    const char messageDelimiter = '\0';         // Null character as a message delimiter

public:
    // Get singleton instance
    static Crypto& getInstance();

    // Hash a username into a consistent hash code
    std::size_t toHashcode(const std::string& username) const;

    // Encode/decode message using ASCII conversion
    std::string encode(const std::string& message) const;
    std::string decode(const std::string& message) const;
};

#endif // CRYPTO_H
