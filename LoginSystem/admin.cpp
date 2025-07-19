#include "admin.h"

Admin::Admin(const std::string& username, const std::string& passwordHash, const std::string& backupCode)
    : username(username), passwordHash(passwordHash), backupCode(backupCode) {
}

std::string Admin::getUsername() const {
    return username;
}

std::string Admin::getPasswordHash() const {
    return passwordHash;
}

std::string Admin::getBackupCode() const {
    return backupCode;
} 