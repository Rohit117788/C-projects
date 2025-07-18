
#ifndef TRANSACTION_H
#define TRANSACTION_H
#include <string>

class Transaction {
private:
    std::string transactionID;
    std::string type; // Deposit, Withdraw, Transfer
    double amount;
    std::string dateTime;
    std::string srcAccount;
    std::string destAccount;
public:
    Transaction();
    Transaction(const std::string& id, const std::string& type, double amount, const std::string& dateTime,
                const std::string& srcAccount, const std::string& destAccount);
    std::string getTransactionID() const;
    std::string getType() const;
    double getAmount() const;
    std::string getDateTime() const;
    std::string getSrcAccount() const;
    std::string getDestAccount() const;
    void display() const;
};

#endif // TRANSACTION_H
