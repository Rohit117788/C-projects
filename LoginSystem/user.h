#ifndef USER_H
#define USER_H

#include <string>

class User {
private:
    std::string username;
    std::string email;
    std::string mobile;
    std::string passwordHash;
    std::string backupCode;

public:
    User(const std::string& username, const std::string& email, const std::string& mobile, 
         const std::string& passwordHash, const std::string& backupCode);
    
    std::string getUsername() const;
    std::string getEmail() const;
    std::string getMobile() const;
    std::string getPasswordHash() const;
    std::string getBackupCode() const;
    
    void setPasswordHash(const std::string& hash);
};

#endif
