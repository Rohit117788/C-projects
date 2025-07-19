#include "user.h"

User::User(const std::string& username, const std::string& email, const std::string& mobile, 
           const std::string& passwordHash, const std::string& backupCode)
    : username(username), email(email), mobile(mobile), passwordHash(passwordHash), backupCode(backupCode) {
}

std::string User::getUsername() const {
    return username;
}

std::string User::getEmail() const {
    return email;
}

std::string User::getMobile() const {
    return mobile;
}

std::string User::getPasswordHash() const {
    return passwordHash;
}

std::string User::getBackupCode() const {
    return backupCode;
}

void User::setPasswordHash(const std::string& hash) {
    passwordHash = hash;
}
