#include "Account.h"
#include <iostream>

Account::Account() : age(0), balance(0.0), isActive(true) {}

Account::Account(const std::string& accNum, const std::string& custID, const std::string& name, int age, const std::string& gender,
                 const std::string& phone, const std::string& address, const std::string& aadhar, const std::string& accType,
                 double balance, const std::string& pin, const std::string& branch)
    : accountNumber(accNum), customerID(custID), name(name), age(age), gender(gender), phone(phone), address(address),
      aadhar(aadhar), accountType(accType), balance(balance), pin(pin), branch(branch), isActive(true) {}

std::string Account::getAccountNumber() const { return accountNumber; }
std::string Account::getCustomerID() const { return customerID; }
std::string Account::getName() const { return name; }
int Account::getAge() const { return age; }
std::string Account::getGender() const { return gender; }
std::string Account::getPhone() const { return phone; }
std::string Account::getAddress() const { return address; }
std::string Account::getAadhar() const { return aadhar; }
std::string Account::getAccountType() const { return accountType; }
double Account::getBalance() const { return balance; }
std::string Account::getPin() const { return pin; }
bool Account::getIsActive() const { return isActive; }
std::string Account::getBranch() const { return branch; }

void Account::setPhone(const std::string& phone_) { phone = phone_; }
void Account::setAddress(const std::string& address_) { address = address_; }
void Account::setPin(const std::string& pin_) { pin = pin_; }
void Account::setIsActive(bool active) { isActive = active; }
void Account::setBranch(const std::string& branch_) { branch = branch_; }

bool Account::deposit(double amount) {
    if (amount > 0) {
        balance += amount;
        return true;
    }
    return false;
}

bool Account::withdraw(double amount) {
    if (amount > 0 && balance - amount >= 0) {
        balance -= amount;
        return true;
    }
    return false;
}

void Account::display() const {
    std::cout << "Account Number: " << accountNumber << "\n"
              << "Customer ID: " << customerID << "\n"
              << "Name: " << name << "\n"
              << "Age: " << age << "\n"
              << "Gender: " << gender << "\n"
              << "Phone: " << phone << "\n"
              << "Address: " << address << "\n"
              << "Aadhar: " << aadhar << "\n"
              << "Account Type: " << accountType << "\n"
              << "Balance: " << balance << "\n"
              << "Status: " << (isActive ? "Active" : "Closed") << std::endl;
}
