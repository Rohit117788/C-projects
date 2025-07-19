#ifndef ADMIN_H
#define ADMIN_H

#include <string>

class Admin {
private:
    std::string username;
    std::string passwordHash;
    std::string backupCode;

public:
    Admin(const std::string& username, const std::string& passwordHash, const std::string& backupCode);
    
    std::string getUsername() const;
    std::string getPasswordHash() const;
    std::string getBackupCode() const;
};

#endif
