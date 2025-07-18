
#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>
#include <vector>

class Account {
private:
    std::string accountNumber;
    std::string customerID;
    std::string name;
    int age;
    std::string gender;
    std::string phone;
    std::string address;
    std::string aadhar;
    std::string accountType; // Saving/Current
    double balance;
    std::string pin;
    bool isActive;
    std::string branch;
public:
    Account();
    Account(const std::string& accNum, const std::string& custID, const std::string& name, int age, const std::string& gender,
            const std::string& phone, const std::string& address, const std::string& aadhar, const std::string& accType,
            double balance, const std::string& pin, const std::string& branch);
    // Getters
    std::string getAccountNumber() const;
    std::string getCustomerID() const;
    std::string getName() const;
    int getAge() const;
    std::string getGender() const;
    std::string getPhone() const;
    std::string getAddress() const;
    std::string getAadhar() const;
    std::string getAccountType() const;
    double getBalance() const;
    std::string getPin() const;
    bool getIsActive() const;
    std::string getBranch() const;
    // Setters
    void setPhone(const std::string& phone);
    void setAddress(const std::string& address);
    void setPin(const std::string& pin);
    void setIsActive(bool active);
    void setBranch(const std::string& branch);
    // Operations
    bool deposit(double amount);
    bool withdraw(double amount);
    void display() const;
};

#endif // ACCOUNT_H
