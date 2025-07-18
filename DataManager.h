
#ifndef DATAMANAGER_H
#define DATAMANAGER_H
#include <vector>
#include <string>
#include "Account.h"
#include "Transaction.h"

class DataManager {
public:
    static std::vector<Account> loadAccounts(const std::string& filename);
    static void saveAccounts(const std::vector<Account>& accounts, const std::string& filename);
    static std::vector<Transaction> loadTransactions(const std::string& filename);
    static void saveTransactions(const std::vector<Transaction>& transactions, const std::string& filename);
    static Account* findAccount(std::vector<Account>& accounts, const std::string& accountNumber);
    static std::string generateAccountNumber();
    static std::string generateCustomerID();
    static std::string getCurrentDateTime();
};

#endif // DATAMANAGER_H
