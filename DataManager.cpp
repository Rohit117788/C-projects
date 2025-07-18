
#include "DataManager.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

std::vector<Account> DataManager::loadAccounts(const std::string& filename) {
    std::vector<Account> accounts;
    std::ifstream file(filename);
    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string accNum, custID, name, gender, phone, address, aadhar, accType, pin, branch, isActiveStr;
        int age;
        double balance;
        bool isActive;
        getline(ss, accNum, ',');
        getline(ss, custID, ',');
        getline(ss, name, ',');
        ss >> age; ss.ignore();
        getline(ss, gender, ',');
        getline(ss, phone, ',');
        getline(ss, address, ',');
        getline(ss, aadhar, ',');
        getline(ss, accType, ',');
        ss >> balance; ss.ignore();
        getline(ss, pin, ',');
        getline(ss, branch, ',');
        getline(ss, isActiveStr, ',');
        isActive = (isActiveStr == "1");
        Account acc(accNum, custID, name, age, gender, phone, address, aadhar, accType, balance, pin, branch);
        acc.setIsActive(isActive);
        accounts.push_back(acc);
    }
    return accounts;
}

void DataManager::saveAccounts(const std::vector<Account>& accounts, const std::string& filename) {
    std::ofstream file(filename);
    for (const auto& acc : accounts) {
        file << acc.getAccountNumber() << ','
             << acc.getCustomerID() << ','
             << acc.getName() << ','
             << acc.getAge() << ','
             << acc.getGender() << ','
             << acc.getPhone() << ','
             << acc.getAddress() << ','
             << acc.getAadhar() << ','
             << acc.getAccountType() << ','
             << acc.getBalance() << ','
             << acc.getPin() << ','
             << acc.getBranch() << ','
             << (acc.getIsActive() ? "1" : "0") << '\n';
    }
}

std::vector<Transaction> DataManager::loadTransactions(const std::string& filename) {
    std::vector<Transaction> transactions;
    std::ifstream file(filename);
    std::string line;
    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string id, type, dateTime, src, dest;
        double amount;
        getline(ss, id, ',');
        getline(ss, type, ',');
        ss >> amount; ss.ignore();
        getline(ss, dateTime, ',');
        getline(ss, src, ',');
        getline(ss, dest, ',');
        transactions.emplace_back(id, type, amount, dateTime, src, dest);
    }
    return transactions;
}

void DataManager::saveTransactions(const std::vector<Transaction>& transactions, const std::string& filename) {
    std::ofstream file(filename);
    for (const auto& t : transactions) {
        file << t.getTransactionID() << ','
             << t.getType() << ','
             << t.getAmount() << ','
             << t.getDateTime() << ','
             << t.getSrcAccount() << ','
             << t.getDestAccount() << '\n';
    }
}

Account* DataManager::findAccount(std::vector<Account>& accounts, const std::string& accountNumber) {
    for (auto& acc : accounts) {
        if (acc.getAccountNumber() == accountNumber) {
            return &acc;
        }
    }
    return nullptr;
}

std::string DataManager::generateAccountNumber() {
    // Scan accounts.txt for the highest ACC number and increment
    std::ifstream file("accounts.txt");
    std::string line, accNum;
    int maxNum = 122; // Start before ACC123
    while (getline(file, line)) {
        std::stringstream ss(line);
        getline(ss, accNum, ',');
        if (accNum.substr(0, 3) == "ACC" && accNum.length() > 3) {
            int num = std::stoi(accNum.substr(3));
            if (num > maxNum) maxNum = num;
        }
    }
    return "ACC" + std::to_string(maxNum + 1);
}

std::string DataManager::generateCustomerID() {
    static int counter = 1000; // For demo, should use persistent storage
    return "CUST" + std::to_string(counter++);
}

std::string DataManager::getCurrentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm* ptm = std::localtime(&now);
    char buffer[32];
    std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
    return std::string(buffer);
}
