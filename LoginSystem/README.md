# C++ GUI Login System

A simple login system built with pure C++ and Windows API.

## Features

- User registration and login
- Admin login
- Password reset with OTP
- Email verification
- Password hashing

## How to Compile

### Using MinGW:
```powershell
g++ -o LoginSystem.exe main.cpp user.cpp admin.cpp email_sender.cpp -luser32 -lcomctl32 -lgdi32 -lwininet
```

### Using Visual Studio:
```cmd
cl /EHsc main.cpp user.cpp admin.cpp email_sender.cpp /link user32.lib comctl32.lib wininet.lib /out:LoginSystem.exe
```

## How to Run

```powershell
.\LoginSystem.exe
```

## Files

- `main.cpp` - Main application code
- `user.h/cpp` - User class
- `admin.h/cpp` - Admin class
- `email_sender.h/cpp` - Email functionality
- `config.h` - Configuration settings
- `picosha2.h` - Password hashing library

## Setup

1. Copy `config.example.h` to `config.h`
2. Replace the API key in `config.h` with your SendGrid API key
3. Replace the email in `config.h` with your verified sender email
4. Compile and run

## Notes

- Uses SendGrid for email OTP
- Passwords are hashed with SHA-256
- Data stored in text files
