# Banking System Project

A simple C++ banking system with both admin and user features, designed and implemented by **Rohit Ahirwar**.

---

## Features

- **User Registration & Login**
  - Secure 4-digit PIN for each user
  - Unique account number generation (e.g., ACC123, ACC124, ...)
  - Date of birth and Aadhaar validation
  - Each Aadhaar can have up to 2 accounts

- **User Operations**
  - Deposit, withdraw, and transfer money
  - View personal transaction history (only your own transactions)
  - Edit personal information
  - Close account

- **Admin Operations**
  - Register and login as admin (secure password)
  - View all accounts or accounts by branch
  - View total bank balance
  - View all transactions (admin only)
  - Export transactions to file
  - Appoint a new admin
  - Freeze/unfreeze user accounts
  - Credit money to any user account
  - View all user balances
  - View admin's own transactions
  - Delete user accounts
  - Process user requests for PIN/account number reset

- **Security & Validation**
  - All input is validated for correctness
  - Only admins can see all transactions or manage other users

---

## How to Compile and Run

Open your terminal and run:

```sh
cd /path/to/your/BankingSystem
g++ main.cpp Account.cpp DataManager.cpp Transaction.cpp -o BankingSystem.exe
./BankingSystem.exe
```

---

## File Structure

- `main.cpp` - Main application logic and menus
- `Account.h/cpp` - Account class and related logic
- `Transaction.h/cpp` - Transaction class and related logic
- `DataManager.h/cpp` - Data loading, saving, and utility functions
- `accounts.txt` - Stores user account data
- `transactions_ACC123.txt` - Stores transaction data
- `admins.txt` - Stores admin credentials

---

## Author

**Rohit Ahirwar**

---

## License

This project is for educational purposes. You are free to use and modify it.
