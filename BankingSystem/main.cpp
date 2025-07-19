
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include "Account.h"
#include "Transaction.h"
#include "DataManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "picosha2.h"

// --- Admin backup code and password reset integration ---
#include <random>

// Helper to generate a random backup code
std::string generateBackupCode(int length = 10) {
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
    std::string code;
    for (int i = 0; i < length; ++i) code += charset[dis(gen)];
    return code;
}

#define RESET   "\033[0m"
#define BLUE    "\033[1;34m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define RED     "\033[1;31m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"

void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

void printLogo() {
    clearScreen();
    std::cout << BLUE;
    std::cout << std::string(2, '\n');
    std::cout << "==============================\n";
    std::cout << "         YOHO BANK             \n";
    std::cout << "==============================\n";
    std::cout << RESET;
}

void pause() {
    std::cout << WHITE << "\nPress Enter to continue..." << RESET;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string selectBranch() {
    int choice;
    std::cout << CYAN << "\nSelect Branch:" << RESET << std::endl;
    std::cout << "  1. North\n  2. South\n  3. East\n  4. West\n";
    std::cout << YELLOW << "Enter choice: " << RESET;
    std::cin >> choice;
    std::cin.ignore();
    switch (choice) {
        case 1: return "North";
        case 2: return "South";
        case 3: return "East";
        case 4: return "West";
        default: return "North";
    }
}

void coloredInput(const std::string& prompt, std::string& input, const std::string& color = YELLOW) {
    std::cout << color << prompt << RESET;
    getline(std::cin, input);
}

void coloredInput(const std::string& prompt, int& input, const std::string& color = YELLOW) {
    std::cout << color << prompt << RESET;
    std::cin >> input;
    std::cin.ignore();
}

void coloredInput(const std::string& prompt, double& input, const std::string& color = YELLOW) {
    std::cout << color << prompt << RESET;
    std::cin >> input;
    std::cin.ignore();
}

void pinInput(std::string& pin) {
    std::cout << YELLOW << "Enter 4-digit PIN: " << RESET;
    getline(std::cin, pin);
}

bool isValidUserPin(const std::string& pin) {
    if (pin.length() != 4) return false;
    for (char c : pin) if (!isdigit(c)) return false;
    return true;
}

bool isValidPassword(const std::string& pass) {
    if (pass.length() < 7) return false;
    for (char c : pass) {
        if (!isalnum(c)) return true;
    }
    return false;
}

void displayAccounts(const std::vector<Account>& accounts, const std::string& branch = "") {
    std::cout << CYAN << "\n--- Account List ---" << RESET << std::endl;
    for (const auto& acc : accounts) {
        if (!branch.empty() && acc.getBranch() != branch) continue;
        acc.display();
        std::cout << "-----------------------------" << std::endl;
    }
}

void displayTransactions(const std::vector<Transaction>& transactions) {
    std::cout << CYAN << "\n--- Transaction List ---" << RESET << std::endl;
    for (const auto& t : transactions) {
        t.display();
        std::cout << "-----------------------------" << std::endl;
    }
}

bool adminExists() {
    std::ifstream file("admins.txt");
    std::string line;
    return static_cast<bool>(getline(file, line));
}

// --- Admin forgot password flow ---
bool adminLogin() {
    std::string user, pass, line;
    coloredInput("Enter admin username: ", user);
    coloredInput("Enter admin password: ", pass);
    std::ifstream file("admins.txt");
    std::string u, p, b;
    if (getline(file, line)) {
        std::stringstream ss(line);
        getline(ss, u, ',');
        getline(ss, p, ',');
        getline(ss, b, ',');
        if (u != user) {
            std::cout << RED << "Wrong username!" << RESET << std::endl;
            return false;
        }
        if (p != picosha2::hash256_hex_string(pass)) {
            std::cout << RED << "Wrong password!" << RESET << std::endl;
            std::string choice;
            std::cout << YELLOW << "Forgot Password? (y/n): " << RESET;
            getline(std::cin, choice);
            if (choice == "y" || choice == "Y") {
                std::string uname, backup;
                coloredInput("Enter admin username: ", uname);
                coloredInput("Enter backup code: ", backup);
                if (u == uname && b == picosha2::hash256_hex_string(backup)) {
                    std::string newPass;
                    while (true) {
                        coloredInput("Enter new admin password (min 7 chars, 1 symbol): ", newPass);
                        if (!isValidPassword(newPass)) {
                            std::cout << RED << "Password must be at least 7 characters and include at least one symbol!" << RESET << std::endl;
                            continue;
                        }
                        break;
                    }
                    std::ofstream fileOut("admins.txt");
                    fileOut << u << "," << picosha2::hash256_hex_string(newPass) << "," << b << "\n";
                    std::cout << GREEN << "Admin password reset successfully!" << RESET << std::endl;
                } else {
                    std::cout << RED << "Backup code or username incorrect!" << RESET << std::endl;
                }
                pause();
            }
            return false;
        }
        return true;
    }
    std::cout << RED << "No admin registered!" << RESET << std::endl;
    return false;
}

void registerAdmin() {
    std::string user, pass;
    while (true) {
        coloredInput("Set admin username: ", user);
        if (user.empty()) {
            std::cout << RED << "Username cannot be empty!" << RESET << std::endl;
            continue;
        }
        break;
    }
    while (true) {
        coloredInput("Set admin password (min 7 chars, 1 symbol): ", pass);
        if (!isValidPassword(pass)) {
            std::cout << RED << "Password must be at least 7 characters and include at least one symbol!" << RESET << std::endl;
            continue;
        }
        break;
    }
    std::ofstream file("admins.txt");
    std::string pass_hash = picosha2::hash256_hex_string(pass); // Hash password
    std::string backupCode = generateBackupCode();
    std::cout << YELLOW << "Your admin backup code (save this!): " << backupCode << RESET << std::endl;
    std::string backup_hash = picosha2::hash256_hex_string(backupCode);
    file << user << "," << pass_hash << "," << backup_hash << "\n";
    std::cout << GREEN << "Admin registered successfully!" << RESET << std::endl;
    pause();
}

void appointNewAdmin() {
    std::string user, pass;
    while (true) {
        coloredInput("Enter new admin username: ", user);
        if (user.empty()) {
            std::cout << RED << "Username cannot be empty!" << RESET << std::endl;
            continue;
        }
        break;
    }
    while (true) {
        coloredInput("Enter new admin password (min 7 chars, 1 symbol): ", pass);
        if (!isValidPassword(pass)) {
            std::cout << RED << "Password must be at least 7 characters and include at least one symbol!" << RESET << std::endl;
            continue;
        }
        break;
    }
    std::ofstream file("admins.txt");
    std::string pass_hash = picosha2::hash256_hex_string(pass); // Hash password
    std::string backupCode = generateBackupCode();
    std::cout << YELLOW << "Your admin backup code (save this!): " << backupCode << RESET << std::endl;
    std::string backup_hash = picosha2::hash256_hex_string(backupCode);
    file << user << "," << pass_hash << "," << backup_hash << "\n";
    std::cout << GREEN << "New admin appointed successfully!" << RESET << std::endl;
    pause();
}

struct ResetRequest {
    std::string id, name, dob, type; // type: "PIN" or "ACCNUM"
};
static std::vector<ResetRequest> resetRequests;

void mainMenu() {
    std::vector<Account> accounts = DataManager::loadAccounts("accounts.txt");
    std::vector<Transaction> transactions = DataManager::loadTransactions("transactions_ACC123.txt");
    int choice;
    while (true) {
        clearScreen();
        printLogo();
        if (!adminExists()) {
            std::cout << GREEN << "1. User Login\n2. Create Account\n3. Register Admin\n4. Exit\n" << RESET;
        } else {
            std::cout << GREEN << "1. User Login\n2. Create Account\n3. Admin Panel\n4. Exit\n5. Forgot PIN/Account Number\n" << RESET;
        }
        coloredInput("Select option: ", choice);
        if (choice == 1) {
            // User login
            std::string accNum, pin;
            coloredInput("Enter Account Number: ", accNum);
            pinInput(pin);
            if (!isValidUserPin(pin)) {
                std::cout << RED << "PIN must be exactly 4 digits!" << RESET << std::endl;
                pause();
                continue;
            }
            Account* acc = DataManager::findAccount(accounts, accNum);
            if (!acc) {
                std::cout << RED << "Wrong account number!" << RESET << std::endl;
                pause();
                continue;
            }
            if (acc->getPin() != picosha2::hash256_hex_string(pin)) {
                std::cout << RED << "Wrong PIN!" << RESET << std::endl;
                std::string choice;
                std::cout << YELLOW << "Forgot PIN? (y/n): " << RESET;
                getline(std::cin, choice);
                if (choice == "y" || choice == "Y") {
                    std::string aadhar, name, dob;
                    coloredInput("Enter your Aadhaar: ", aadhar);
                    coloredInput("Enter your name: ", name);
                    coloredInput("Enter your date of birth (DDMMYYYY): ", dob);
                    Account* found = nullptr;
                    for (auto& a : accounts) {
                        if (a.getAadhar() == aadhar && a.getName() == name /* && a.getDOB() == dob */) {
                            found = &a;
                            break;
                        }
                    }
                    if (found) {
                        std::string newPin;
                        while (true) {
                            pinInput(newPin);
                            if (!isValidUserPin(newPin)) {
                                std::cout << RED << "PIN must be exactly 4 digits!" << RESET << std::endl;
                                continue;
                            }
                            break;
                        }
                        found->setPin(picosha2::hash256_hex_string(newPin));
                        DataManager::saveAccounts(accounts, "accounts.txt");
                        std::cout << GREEN << "PIN reset successfully!" << RESET << std::endl;
                    } else {
                        std::cout << RED << "No matching account found!" << RESET << std::endl;
                    }
                    pause();
                    continue;
                }
                pause();
                continue;
            }
            if (!acc->getIsActive()) {
                std::cout << RED << "Account closed!" << RESET << std::endl;
                pause();
                continue;
            }
            // User menu
            int userChoice;
            while (true) {
                clearScreen();
                printLogo();
                std::cout << GREEN << "Welcome, " << acc->getName() << "!\n" << RESET;
                std::cout << CYAN << "1. Deposit\n2. Withdraw\n3. Check Balance\n4. Transaction History\n5. Money Transfer\n6. Edit Info\n7. Close Account\n8. Logout\n" << RESET;
                coloredInput("Select option: ", userChoice);
                if (userChoice == 1) {
                    double amt;
                    coloredInput("Enter amount to deposit: ", amt);
                    if (acc->deposit(amt)) {
                        std::string tid = "TXN" + std::to_string(rand() % 1000000);
                        transactions.emplace_back(tid, "Deposit", amt, DataManager::getCurrentDateTime(), acc->getAccountNumber(), "");
                        std::cout << GREEN << "Deposit successful!" << RESET << std::endl;
                    } else {
                        std::cout << RED << "Invalid amount!" << RESET << std::endl;
                    }
                } else if (userChoice == 2) {
                    double amt;
                    coloredInput("Enter amount to withdraw: ", amt);
                    if (acc->withdraw(amt)) {
                        std::string tid = "TXN" + std::to_string(rand() % 1000000);
                        transactions.emplace_back(tid, "Withdraw", amt, DataManager::getCurrentDateTime(), acc->getAccountNumber(), "");
                        std::cout << GREEN << "Withdrawal successful!" << RESET << std::endl;
                    } else {
                        std::cout << RED << "Insufficient balance or invalid amount!" << RESET << std::endl;
                    }
                } else if (userChoice == 3) {
                    std::cout << YELLOW << "Current Balance: " << acc->getBalance() << RESET << std::endl;
                } else if (userChoice == 4) {
                    std::vector<Transaction> userTxns;
                    for (const auto& t : transactions) {
                        if (t.getSrcAccount() == acc->getAccountNumber() || t.getDestAccount() == acc->getAccountNumber())
                            userTxns.push_back(t);
                    }
                    displayTransactions(userTxns);
                } else if (userChoice == 5) {
                    std::string destAccNum;
                    double amt;
                    coloredInput("Enter destination account number: ", destAccNum);
                    coloredInput("Enter amount to transfer: ", amt);
                    Account* destAcc = DataManager::findAccount(accounts, destAccNum);
                    if (destAcc && destAcc->getIsActive() && acc->withdraw(amt)) {
                        destAcc->deposit(amt);
                        std::string tid = "TXN" + std::to_string(rand() % 1000000);
                        transactions.emplace_back(tid, "Transfer", amt, DataManager::getCurrentDateTime(), acc->getAccountNumber(), destAccNum);
                        std::cout << GREEN << "Transfer successful!" << RESET << std::endl;
                    } else {
                        std::cout << RED << "Transfer failed!" << RESET << std::endl;
                    }
                } else if (userChoice == 6) {
                    std::string newPhone, newAddress;
                    coloredInput("Enter new phone: ", newPhone);
                    coloredInput("Enter new address: ", newAddress);
                    acc->setPhone(newPhone);
                    acc->setAddress(newAddress);
                    std::cout << GREEN << "Info updated!" << RESET << std::endl;
                } else if (userChoice == 7) {
                    acc->setIsActive(false);
                    std::cout << YELLOW << "Account closed." << RESET << std::endl;
                    break;
                } else if (userChoice == 8) {
                    break;
                }
                DataManager::saveAccounts(accounts, "accounts.txt");
                DataManager::saveTransactions(transactions, "transactions_ACC123.txt");
                pause();
            }
        } else if (choice == 2) {
            // Create account
            std::string name, gender, phone, address, aadhar, accType, pin, branch, dob;
            while (true) {
                coloredInput("Enter name: ", name);
                if (name.empty()) {
                    std::cout << RED << "Name cannot be empty!" << RESET << std::endl;
                    continue;
                }
                break;
            }
            while (true) {
                coloredInput("Enter date of birth (DDMMYYYY): ", dob);
                if (dob.length() != 8) {
                    std::cout << RED << "DOB must be 8 digits in DDMMYYYY format!" << RESET << std::endl;
                    continue;
                }
                bool valid = true;
                for (char c : dob) if (!isdigit(c)) valid = false;
                if (!valid) {
                    std::cout << RED << "DOB must be numeric!" << RESET << std::endl;
                    continue;
                }
                break;
            }
            coloredInput("Enter gender: ", gender);
            coloredInput("Enter phone: ", phone);
            coloredInput("Enter address: ", address);
            while (true) {
                coloredInput("Enter Aadhaar (15-digit numeric): ", aadhar);
                bool valid = (aadhar.length() == 15);
                for (char c : aadhar) if (!isdigit(c)) valid = false;
                if (!valid) {
                    std::cout << RED << "Aadhaar must be exactly 15 numeric digits!" << RESET << std::endl;
                    aadhar.clear();
                    continue;
                }
                int count = 0;
                for (const auto& acc : accounts) {
                    if (acc.getAadhar() == aadhar) count++;
                }
                if (count >= 2) {
                    std::cout << RED << "A maximum of 2 accounts can be opened with the same Aadhaar!" << RESET << std::endl;
                    aadhar.clear();
                    continue;
                }
                break;
            }
            coloredInput("Enter account type (Saving/Current): ", accType);
            while (true) {
                pinInput(pin);
                if (!isValidUserPin(pin)) {
                    std::cout << RED << "PIN must be exactly 4 digits!" << RESET << std::endl;
                    continue;
                }
                break;
            }
            branch = selectBranch();
            std::string accNum = DataManager::generateAccountNumber();
            std::string custID = DataManager::generateCustomerID();
            std::string pin_hash = picosha2::hash256_hex_string(pin);
            Account newAcc(accNum, custID, name, 0, gender, phone, address, aadhar, accType, 0.0, pin_hash, branch);
            accounts.push_back(newAcc);
            DataManager::saveAccounts(accounts, "accounts.txt");
            std::cout << GREEN << "Account created! Your Account Number: " << accNum << RESET << std::endl;
            pause();
        } else if (choice == 3) {
            if (!adminExists()) {
                registerAdmin();
                continue;
            }
            if (!adminLogin()) { pause(); continue; }
            while (true) {
                clearScreen();
                printLogo();
                std::cout << CYAN
                    << "Admin Panel\n"
                    << "1. View All Accounts\n"
                    << "2. View Accounts by Branch\n"
                    << "3. View Total Bank Balance\n"
                    << "4. View All Transactions\n"
                    << "5. Export Transactions\n"
                    << "6. Appoint New Admin\n"
                    << "7. Logout\n"
                    << "8. Process Reset Requests\n"
                    << "9. Freeze/Unfreeze User Account\n"
                    << "10. Transfer Money to User\n"
                    << "11. View All User Balances\n"
                    << "12. View Admin Transactions\n"
                    << "13. Delete User Account\n"
                    << "14. Reset Admin Password\n"
                    << RESET;
                int adminChoice;
                coloredInput("Select option: ", adminChoice);
                if (adminChoice == 1) {
                    displayAccounts(accounts);
                    pause();
                } else if (adminChoice == 2) {
                    std::string branch = selectBranch();
                    displayAccounts(accounts, branch);
                    pause();
                } else if (adminChoice == 3) {
                    double total = 0;
                    for (const auto& acc : accounts)
                        if (acc.getIsActive()) total += acc.getBalance();
                    std::cout << YELLOW << "Total Bank Balance: " << total << RESET << std::endl;
                    pause();
                } else if (adminChoice == 4) {
                    displayTransactions(transactions);
                    pause();
                } else if (adminChoice == 5) {
                    std::ofstream out("exported_transactions.txt");
                    for (const auto& t : transactions) {
                        out << t.getTransactionID() << ',' << t.getType() << ',' << t.getAmount() << ','
                            << t.getDateTime() << ',' << t.getSrcAccount() << ',' << t.getDestAccount() << '\n';
                    }
                    std::cout << GREEN << "Transactions exported to exported_transactions.txt!" << RESET << std::endl;
                    pause();
                } else if (adminChoice == 6) {
                    appointNewAdmin();
                } else if (adminChoice == 7) {
                    break;
                } else if (adminChoice == 8) {
                    if (resetRequests.empty()) {
                        std::cout << YELLOW << "No reset requests." << RESET << std::endl;
                        pause();
                        continue;
                    }
                    for (size_t i = 0; i < resetRequests.size(); ++i) {
                        const auto& req = resetRequests[i];
                        std::cout << CYAN << "\nRequest " << (i+1) << ": " << RESET << std::endl;
                        std::cout << "ID: " << req.id << ", Name: " << req.name << ", DOB: " << req.dob << ", Type: " << req.type << std::endl;
                        Account* found = nullptr;
                        for (auto& acc : accounts) {
                            if (acc.getAadhar() == req.id && acc.getName() == req.name /* && acc.getDOB() == req.dob */) {
                                found = &acc;
                                break;
                            }
                        }
                        if (!found) {
                            std::cout << RED << "No matching account found for this request." << RESET << std::endl;
                            continue;
                        }
                        if (req.type == "PIN") {
                            std::string newPin;
                            while (true) {
                                coloredInput("Enter new 4-digit PIN for user: ", newPin);
                                if (!isValidUserPin(newPin)) {
                                    std::cout << RED << "PIN must be exactly 4 digits!" << RESET << std::endl;
                                    continue;
                                }
                                break;
                            }
                            found->setPin(picosha2::hash256_hex_string(newPin));
                            std::cout << GREEN << "PIN reset successfully!" << RESET << std::endl;
                        } else if (req.type == "ACCNUM") {
                            std::cout << GREEN << "User's Account Number: " << found->getAccountNumber() << RESET << std::endl;
                        } else {
                            std::cout << RED << "Unknown request type." << RESET << std::endl;
                        }
                    }
                    resetRequests.clear();
                    DataManager::saveAccounts(accounts, "accounts.txt");
                    pause();
                } else if (adminChoice == 9) {
                    std::string accNum;
                    coloredInput("Enter account number to freeze/unfreeze: ", accNum);
                    Account* acc = DataManager::findAccount(accounts, accNum);
                    if (!acc) {
                        std::cout << RED << "Account not found!" << RESET << std::endl;
                    } else {
                        acc->setIsActive(!acc->getIsActive());
                        std::cout << GREEN << (acc->getIsActive() ? "Account unfrozen!" : "Account frozen!") << RESET << std::endl;
                        DataManager::saveAccounts(accounts, "accounts.txt");
                    }
                    pause();
                } else if (adminChoice == 10) {
                    std::string accNum;
                    double amt;
                    coloredInput("Enter account number to credit: ", accNum);
                    coloredInput("Enter amount to credit: ", amt);
                    Account* acc = DataManager::findAccount(accounts, accNum);
                    if (!acc) {
                        std::cout << RED << "Account not found!" << RESET << std::endl;
                    } else {
                        acc->deposit(amt);
                        std::string tid = "ADMIN_TXN" + std::to_string(rand() % 1000000);
                        transactions.emplace_back(tid, "AdminCredit", amt, DataManager::getCurrentDateTime(), "ADMIN", accNum);
                        std::cout << GREEN << "Amount credited successfully!" << RESET << std::endl;
                        DataManager::saveAccounts(accounts, "accounts.txt");
                        DataManager::saveTransactions(transactions, "transactions_ACC123.txt");
                    }
                    pause();
                } else if (adminChoice == 11) {
                    std::cout << CYAN << "\n--- All User Balances ---" << RESET << std::endl;
                    for (const auto& acc : accounts) {
                        std::cout << "Account: " << acc.getAccountNumber() << ", Name: " << acc.getName() << ", Balance: " << acc.getBalance() << std::endl;
                    }
                    pause();
                } else if (adminChoice == 12) {
                    std::vector<Transaction> adminTxns;
                    for (const auto& t : transactions) {
                        if (t.getSrcAccount() == "ADMIN" || t.getDestAccount() == "ADMIN")
                            adminTxns.push_back(t);
                    }
                    displayTransactions(adminTxns);
                    pause();
                } else if (adminChoice == 13) {
                    std::string accNum;
                    coloredInput("Enter account number to delete: ", accNum);
                    auto it = std::remove_if(accounts.begin(), accounts.end(), [&](const Account& acc) {
                        return acc.getAccountNumber() == accNum;
                    });
                    if (it != accounts.end()) {
                        accounts.erase(it, accounts.end());
                        DataManager::saveAccounts(accounts, "accounts.txt");
                        std::cout << GREEN << "Account deleted successfully!" << RESET << std::endl;
                    } else {
                        std::cout << RED << "Account not found!" << RESET << std::endl;
                    }
                    pause();
                } else if (adminChoice == 14) {
                    std::string currentPass, newPass;
                    std::ifstream fileIn("admins.txt");
                    std::string line, u, p;
                    if (getline(fileIn, line)) {
                        std::stringstream ss(line);
                        getline(ss, u, ',');
                        getline(ss, p, ',');
                    }
                    coloredInput("Enter current admin password: ", currentPass);
                    if (p != picosha2::hash256_hex_string(currentPass)) {
                        std::cout << RED << "Wrong current password!" << RESET << std::endl;
                        pause();
                    } else {
                        while (true) {
                            coloredInput("Enter new admin password (min 7 chars, 1 symbol): ", newPass);
                            if (!isValidPassword(newPass)) {
                                std::cout << RED << "Password must be at least 7 characters and include at least one symbol!" << RESET << std::endl;
                                continue;
                            }
                            break;
                        }
                        std::ofstream fileOut("admins.txt");
                        fileOut << u << "," << picosha2::hash256_hex_string(newPass) << "\n";
                        std::cout << GREEN << "Admin password reset successfully!" << RESET << std::endl;
                        pause();
                    }
                }
                pause();
            }
        } else if (choice == 4) {
            std::cout << GREEN << "Thank you for using YOHO Bank!" << RESET << std::endl;
            break;
        } else if (choice == 5) {
            std::string id, name, dob, type;
            std::cout << CYAN << "\n--- Forgot PIN/Account Number Request ---" << RESET << std::endl;
            coloredInput("Enter your ID number (Aadhaar): ", id);
            coloredInput("Enter your name: ", name);
            coloredInput("Enter your date of birth (DDMMYYYY): ", dob);
            coloredInput("Type 'PIN' to reset PIN or 'ACCNUM' to get account number: ", type);
            resetRequests.push_back({id, name, dob, type});
            std::cout << GREEN << "Request submitted to admin. Please wait for admin to process your request." << RESET << std::endl;
            pause();
        }
    }
}

int main() {
    mainMenu();
    return 0;
}
