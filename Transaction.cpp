
#include "Transaction.h"
#include <iostream>

Transaction::Transaction()
    : transactionID(""), type(""), amount(0.0), dateTime(""), srcAccount(""), destAccount("") {}

Transaction::Transaction(const std::string& id, const std::string& type, double amount, const std::string& dateTime,
                         const std::string& srcAccount, const std::string& destAccount)
    : transactionID(id), type(type), amount(amount), dateTime(dateTime), srcAccount(srcAccount), destAccount(destAccount) {}

void Transaction::display() const {
    std::cout << "Transaction ID: " << transactionID << "\n"
              << "From: " << srcAccount << "\n"
              << "To: " << destAccount << "\n"
              << "Amount: " << amount << "\n"
              << "Type: " << type << "\n"
              << "Date: " << dateTime << std::endl;
}

std::string Transaction::getDestAccount() const { return destAccount; }
std::string Transaction::getSrcAccount() const { return srcAccount; }
std::string Transaction::getDateTime() const { return dateTime; }
double Transaction::getAmount() const { return amount; }
std::string Transaction::getType() const { return type; }
std::string Transaction::getTransactionID() const { return transactionID; }
