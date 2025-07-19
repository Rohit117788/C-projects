#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0601
#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <random>
#include <cctype>
#include "picosha2.h"
#include "user.h"
#include "admin.h"
#include "email_sender.h"
#include "config.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "wininet.lib")

HWND hMainWindow;
HWND hLoginWindow = NULL;
HWND hSignupWindow = NULL;
HWND hAdminWindow = NULL;
HWND hPasswordResetWindow = NULL;

HWND hUsernameEdit, hPasswordEdit, hLoginButton, hSignupButton, hForgotButton;
HWND hStatusLabel;

HWND hSignupUsernameEdit, hSignupEmailEdit, hSignupMobileEdit, hSignupPasswordEdit;
HWND hSignupConfirmPasswordEdit, hSignupButton2, hBackToLoginButton;

HWND hAdminUsernameEdit, hAdminPasswordEdit, hAdminLoginButton, hAdminRegisterButton;

HWND hResetEmailEdit, hResetOTPEdit, hResetNewPasswordEdit, hResetConfirmPasswordEdit;
HWND hSendOTPButton, hResetPasswordButton, hBackFromResetButton;

User* currentUser = nullptr;
Admin* currentAdmin = nullptr;

std::string currentOTP;
std::string currentResetEmail;

LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK SignupWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AdminWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK PasswordResetWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

std::string GetWindowText(HWND hwnd);
void SetWindowText(HWND hwnd, const std::string& text);
std::string HashPassword(const std::string& password);
std::string GenerateOTP();
void SendOTPEmail(const std::string& email, const std::string& otp);
void ShowMessage(const std::string& title, const std::string& message);
bool ValidateEmail(const std::string& email);
bool ValidateMobile(const std::string& mobile);
bool ValidatePassword(const std::string& password);
void ShowLoginWindow();
void ShowSignupWindow();
void ShowAdminWindow();
void ShowPasswordResetWindow();
void HideAllWindows();
void LoadUsers();
void SaveUsers();
void LoadAdmins();
void SaveAdmins();
void ClearSignupForm();
void ClearLoginForm();
void ClearPasswordResetForm();

std::vector<User> users;
std::vector<Admin> admins;

EmailSender emailSender(SENDGRID_API_KEY, FROM_EMAIL);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    LoadUsers();
    LoadAdmins();

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = MainWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"LoginSystemMain";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hMainWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        L"LoginSystemMain",
        L"Login System",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL, NULL, hInstance, NULL
    );

    if (hMainWindow == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hMainWindow, nCmdShow);
    UpdateWindow(hMainWindow);

    ShowLoginWindow();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void ShowLoginWindow() {
    HideAllWindows();
    
    if (hLoginWindow == NULL) {
        hLoginWindow = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"STATIC",
            L"",
            WS_CHILD | WS_VISIBLE,
            10, 10, 380, 280,
            hMainWindow, NULL, NULL, NULL
        );
        
        SetWindowLongPtr(hLoginWindow, GWLP_WNDPROC, (LONG_PTR)LoginWindowProc);

        CreateWindow(L"STATIC", L"Username:", WS_VISIBLE | WS_CHILD,
            20, 30, 80, 20, hLoginWindow, NULL, NULL, NULL);
        
        hUsernameEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            110, 30, 200, 25, hLoginWindow, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Password:", WS_VISIBLE | WS_CHILD,
            20, 70, 80, 20, hLoginWindow, NULL, NULL, NULL);
        
        hPasswordEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
            110, 70, 200, 25, hLoginWindow, NULL, NULL, NULL);

        hLoginButton = CreateWindow(L"BUTTON", L"Login", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            110, 110, 80, 30, hLoginWindow, (HMENU)1, NULL, NULL);

        hSignupButton = CreateWindow(L"BUTTON", L"Sign Up", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            200, 110, 80, 30, hLoginWindow, (HMENU)2, NULL, NULL);

        hForgotButton = CreateWindow(L"BUTTON", L"Forgot Password?", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            110, 150, 170, 25, hLoginWindow, (HMENU)3, NULL, NULL);

        CreateWindow(L"BUTTON", L"Admin Login", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            110, 190, 170, 30, hLoginWindow, (HMENU)4, NULL, NULL);

        CreateWindow(L"BUTTON", L"Exit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            110, 230, 170, 30, hLoginWindow, (HMENU)5, NULL, NULL);

        hStatusLabel = CreateWindow(L"STATIC", L"", WS_VISIBLE | WS_CHILD | SS_CENTER,
            20, 270, 340, 20, hLoginWindow, NULL, NULL, NULL);

        HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        
        SendMessage(hUsernameEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hPasswordEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hLoginButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hSignupButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hForgotButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hStatusLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
    }

    ShowWindow(hLoginWindow, SW_SHOW);
}

void ShowSignupWindow() {
    HideAllWindows();
    
    if (hSignupWindow == NULL) {
        hSignupWindow = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"STATIC",
            L"",
            WS_CHILD | WS_VISIBLE,
            10, 10, 380, 280,
            hMainWindow, NULL, NULL, NULL
        );
        
        SetWindowLongPtr(hSignupWindow, GWLP_WNDPROC, (LONG_PTR)SignupWindowProc);

        CreateWindow(L"STATIC", L"Username:", WS_VISIBLE | WS_CHILD,
            20, 20, 80, 20, hSignupWindow, NULL, NULL, NULL);
        
        hSignupUsernameEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            110, 20, 200, 25, hSignupWindow, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Email:", WS_VISIBLE | WS_CHILD,
            20, 50, 80, 20, hSignupWindow, NULL, NULL, NULL);
        
        hSignupEmailEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            110, 50, 200, 25, hSignupWindow, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Mobile:", WS_VISIBLE | WS_CHILD,
            20, 80, 80, 20, hSignupWindow, NULL, NULL, NULL);
        
        hSignupMobileEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            110, 80, 200, 25, hSignupWindow, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Password:", WS_VISIBLE | WS_CHILD,
            20, 110, 80, 20, hSignupWindow, NULL, NULL, NULL);
        
        hSignupPasswordEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
            110, 110, 200, 25, hSignupWindow, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Confirm:", WS_VISIBLE | WS_CHILD,
            20, 140, 80, 20, hSignupWindow, NULL, NULL, NULL);
        
        hSignupConfirmPasswordEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
            110, 140, 200, 25, hSignupWindow, NULL, NULL, NULL);

        hSignupButton2 = CreateWindow(L"BUTTON", L"Sign Up", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            110, 180, 80, 30, hSignupWindow, (HMENU)5, NULL, NULL);

        hBackToLoginButton = CreateWindow(L"BUTTON", L"Back to Login", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            200, 180, 100, 30, hSignupWindow, (HMENU)6, NULL, NULL);

        CreateWindow(L"BUTTON", L"Exit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            110, 220, 190, 30, hSignupWindow, (HMENU)7, NULL, NULL);

        HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        
        SendMessage(hSignupUsernameEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hSignupEmailEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hSignupMobileEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hSignupPasswordEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hSignupConfirmPasswordEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hSignupButton2, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBackToLoginButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    }

    ShowWindow(hSignupWindow, SW_SHOW);
}

void ShowAdminWindow() {
    HideAllWindows();
    
    if (hAdminWindow == NULL) {
        hAdminWindow = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"STATIC",
            L"",
            WS_CHILD | WS_VISIBLE,
            10, 10, 380, 280,
            hMainWindow, NULL, NULL, NULL
        );
        
        SetWindowLongPtr(hAdminWindow, GWLP_WNDPROC, (LONG_PTR)AdminWindowProc);

        CreateWindow(L"STATIC", L"Admin Username:", WS_VISIBLE | WS_CHILD,
            20, 30, 120, 20, hAdminWindow, NULL, NULL, NULL);
        
        hAdminUsernameEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            150, 30, 200, 25, hAdminWindow, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Admin Password:", WS_VISIBLE | WS_CHILD,
            20, 70, 120, 20, hAdminWindow, NULL, NULL, NULL);
        
        hAdminPasswordEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
            150, 70, 200, 25, hAdminWindow, NULL, NULL, NULL);

        hAdminLoginButton = CreateWindow(L"BUTTON", L"Admin Login", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            150, 110, 100, 30, hAdminWindow, (HMENU)7, NULL, NULL);

        hAdminRegisterButton = CreateWindow(L"BUTTON", L"Register Admin", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            260, 110, 100, 30, hAdminWindow, (HMENU)8, NULL, NULL);

        CreateWindow(L"BUTTON", L"Back to Login", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            150, 150, 100, 30, hAdminWindow, (HMENU)9, NULL, NULL);

        CreateWindow(L"BUTTON", L"Exit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            260, 150, 100, 30, hAdminWindow, (HMENU)10, NULL, NULL);

        HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        
        SendMessage(hAdminUsernameEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hAdminPasswordEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hAdminLoginButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hAdminRegisterButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    }

    ShowWindow(hAdminWindow, SW_SHOW);
}

void ShowPasswordResetWindow() {
    HideAllWindows();
    
    if (hPasswordResetWindow == NULL) {
        hPasswordResetWindow = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"STATIC",
            L"",
            WS_CHILD | WS_VISIBLE,
            10, 10, 380, 280,
            hMainWindow, NULL, NULL, NULL
        );
        
        SetWindowLongPtr(hPasswordResetWindow, GWLP_WNDPROC, (LONG_PTR)PasswordResetWindowProc);

        CreateWindow(L"STATIC", L"Email:", WS_VISIBLE | WS_CHILD,
            20, 20, 80, 20, hPasswordResetWindow, NULL, NULL, NULL);
        
        hResetEmailEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            110, 20, 200, 25, hPasswordResetWindow, NULL, NULL, NULL);

        hSendOTPButton = CreateWindow(L"BUTTON", L"Send OTP", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            320, 20, 60, 25, hPasswordResetWindow, (HMENU)10, NULL, NULL);

        CreateWindow(L"STATIC", L"OTP:", WS_VISIBLE | WS_CHILD,
            20, 50, 80, 20, hPasswordResetWindow, NULL, NULL, NULL);
        
        hResetOTPEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER,
            110, 50, 200, 25, hPasswordResetWindow, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"New Password:", WS_VISIBLE | WS_CHILD,
            20, 80, 100, 20, hPasswordResetWindow, NULL, NULL, NULL);
        
        hResetNewPasswordEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
            130, 80, 200, 25, hPasswordResetWindow, NULL, NULL, NULL);

        CreateWindow(L"STATIC", L"Confirm Password:", WS_VISIBLE | WS_CHILD,
            20, 110, 110, 20, hPasswordResetWindow, NULL, NULL, NULL);
        
        hResetConfirmPasswordEdit = CreateWindow(L"EDIT", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_PASSWORD,
            140, 110, 200, 25, hPasswordResetWindow, NULL, NULL, NULL);

        hResetPasswordButton = CreateWindow(L"BUTTON", L"Reset Password", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            130, 150, 100, 30, hPasswordResetWindow, (HMENU)11, NULL, NULL);

        hBackFromResetButton = CreateWindow(L"BUTTON", L"Back to Login", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            130, 150, 100, 30, hPasswordResetWindow, (HMENU)12, NULL, NULL);

        CreateWindow(L"BUTTON", L"Exit", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            240, 150, 100, 30, hPasswordResetWindow, (HMENU)13, NULL, NULL);

        HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        
        SendMessage(hResetEmailEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hResetOTPEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hResetNewPasswordEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hResetConfirmPasswordEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hSendOTPButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hResetPasswordButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hBackFromResetButton, WM_SETFONT, (WPARAM)hFont, TRUE);
    }

    ShowWindow(hPasswordResetWindow, SW_SHOW);
    ClearPasswordResetForm();
}

void HideAllWindows() {
    if (hLoginWindow) ShowWindow(hLoginWindow, SW_HIDE);
    if (hSignupWindow) ShowWindow(hSignupWindow, SW_HIDE);
    if (hAdminWindow) ShowWindow(hAdminWindow, SW_HIDE);
    if (hPasswordResetWindow) ShowWindow(hPasswordResetWindow, SW_HIDE);
}

std::string GetWindowText(HWND hwnd) {
    int length = GetWindowTextLength(hwnd);
    if (length == 0) return "";
    
    std::wstring wstr(length + 1, L'\0');
    GetWindowText(hwnd, &wstr[0], length + 1);
    
    std::string str;
    str.reserve(wstr.length());
    for (wchar_t wc : wstr) {
        if (wc == L'\0') break;
        str += static_cast<char>(wc);
    }
    return str;
}

void SetWindowText(HWND hwnd, const std::string& text) {
    std::wstring wstr(text.begin(), text.end());
    ::SetWindowText(hwnd, wstr.c_str());
}

std::string HashPassword(const std::string& password) {
    return picosha2::hash256_hex_string(password);
}

std::string GenerateOTP() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    return std::to_string(dis(gen));
}

void SendOTPEmail(const std::string& email, const std::string& otp) {
    if (emailSender.SendOTPEmail(email, otp)) {
        ShowMessage("OTP Sent", "OTP has been sent to your email: " + email);
    } else {
        ShowMessage("OTP Sent", "OTP: " + otp + "\n\nEmail API failed. This is a simulation.");
    }
}

void ShowMessage(const std::string& title, const std::string& message) {
    std::wstring wtitle(title.begin(), title.end());
    std::wstring wmessage(message.begin(), message.end());
    MessageBox(hMainWindow, wmessage.c_str(), wtitle.c_str(), MB_OK | MB_ICONINFORMATION);
}

bool ValidateEmail(const std::string& email) {
    return email.find('@') != std::string::npos && email.find('.') != std::string::npos;
}

bool ValidateMobile(const std::string& mobile) {
    if (mobile.length() != 10) {
        return false;
    }
    
    for (char c : mobile) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    
    return true;
}

bool ValidatePassword(const std::string& password) {
    if (password.length() < 8) return false;
    
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char c : password) {
        if (isupper(c)) hasUpper = true;
        else if (islower(c)) hasLower = true;
        else if (isdigit(c)) hasDigit = true;
        else hasSpecial = true;
    }
    
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

void LoadUsers() {
    std::ifstream file("user.txt");
    if (!file.is_open()) return;
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string username, email, mobile, passwordHash, backupCode;
        
        if (std::getline(ss, username, '|') &&
            std::getline(ss, email, '|') &&
            std::getline(ss, mobile, '|') &&
            std::getline(ss, passwordHash, '|') &&
            std::getline(ss, backupCode)) {
            
            users.emplace_back(username, email, mobile, passwordHash, backupCode);
        }
    }
    file.close();
}

void SaveUsers() {
    std::ofstream file("user.txt");
    if (!file.is_open()) return;
    
    for (const auto& user : users) {
        file << user.getUsername() << "|" << user.getEmail() << "|" 
             << user.getMobile() << "|" << user.getPasswordHash() << "|" 
             << user.getBackupCode() << "\n";
    }
    file.close();
}

void LoadAdmins() {
    std::ifstream file("admin.txt");
    if (!file.is_open()) return;
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string username, passwordHash, backupCode;
        
        if (std::getline(ss, username, '|') &&
            std::getline(ss, passwordHash, '|') &&
            std::getline(ss, backupCode)) {
            
            admins.emplace_back(username, passwordHash, backupCode);
        }
    }
    file.close();
}

void SaveAdmins() {
    std::ofstream file("admin.txt");
    if (!file.is_open()) return;
    
    for (const auto& admin : admins) {
        file << admin.getUsername() << "|" << admin.getPasswordHash() << "|" 
             << admin.getBackupCode() << "\n";
    }
    file.close();
}

void ClearSignupForm() {
    SetWindowText(hSignupUsernameEdit, "");
    SetWindowText(hSignupEmailEdit, "");
    SetWindowText(hSignupMobileEdit, "");
    SetWindowText(hSignupPasswordEdit, "");
    SetWindowText(hSignupConfirmPasswordEdit, "");
}

void ClearLoginForm() {
    SetWindowText(hUsernameEdit, "");
    SetWindowText(hPasswordEdit, "");
    SetWindowText(hStatusLabel, "");
}

void ClearPasswordResetForm() {
    SetWindowText(hResetEmailEdit, "");
    SetWindowText(hResetOTPEdit, "");
    SetWindowText(hResetNewPasswordEdit, "");
    SetWindowText(hResetConfirmPasswordEdit, "");
    currentOTP = "";
    currentResetEmail = "";
}

LRESULT CALLBACK LoginWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COMMAND) {
        switch (LOWORD(wParam)) {
            case 1:
            {
                std::string username = GetWindowText(hUsernameEdit);
                std::string password = GetWindowText(hPasswordEdit);
                
                if (username.empty() || password.empty()) {
                    SetWindowText(hStatusLabel, "Please enter username and password");
                    break;
                }
                
                std::string passwordHash = HashPassword(password);
                bool found = false;
                
                for (auto& user : users) {
                    if (user.getUsername() == username && user.getPasswordHash() == passwordHash) {
                        currentUser = &user;
                        found = true;
                        break;
                    }
                }
                
                if (found) {
                    ShowMessage("Login Successful", "Welcome " + username + "!");
                    ClearLoginForm();
                } else {
                    SetWindowText(hStatusLabel, "Invalid username or password");
                }
                break;
            }
            case 2:
                ShowSignupWindow();
                break;
            case 3:
                ShowPasswordResetWindow();
                break;
            case 4:
                ShowAdminWindow();
                break;
            case 5:
                PostQuitMessage(0);
                break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK SignupWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COMMAND) {
        switch (LOWORD(wParam)) {
            case 5:
            {
                std::string username = GetWindowText(hSignupUsernameEdit);
                std::string email = GetWindowText(hSignupEmailEdit);
                std::string mobile = GetWindowText(hSignupMobileEdit);
                std::string password = GetWindowText(hSignupPasswordEdit);
                std::string confirmPassword = GetWindowText(hSignupConfirmPasswordEdit);
                
                if (username.empty() || email.empty() || mobile.empty() || password.empty() || confirmPassword.empty()) {
                    ShowMessage("Error", "All fields are required");
                    break;
                }
                
                if (!ValidateEmail(email)) {
                    ShowMessage("Error", "Invalid email format");
                    break;
                }
                
                if (!ValidateMobile(mobile)) {
                    ShowMessage("Error", "Mobile number must be 10 digits");
                    break;
                }
                
                if (!ValidatePassword(password)) {
                    ShowMessage("Error", "Password must be at least 8 characters with uppercase, lowercase, digit, and special character");
                    break;
                }
                
                if (password != confirmPassword) {
                    ShowMessage("Error", "Passwords do not match");
                    break;
                }
                
                for (const auto& user : users) {
                    if (user.getUsername() == username) {
                        ShowMessage("Error", "Username already exists");
                        break;
                    }
                }
                
                std::string passwordHash = HashPassword(password);
                std::string backupCode = GenerateOTP();
                users.emplace_back(username, email, mobile, passwordHash, backupCode);
                SaveUsers();
                
                ShowMessage("Registration Successful", "Account created successfully!\n\nBackup code: " + backupCode);
                ClearSignupForm();
                ShowLoginWindow();
                break;
            }
            case 6:
                ShowLoginWindow();
                break;
            case 7:
                PostQuitMessage(0);
                break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK AdminWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COMMAND) {
        switch (LOWORD(wParam)) {
            case 7:
            {
                std::string username = GetWindowText(hAdminUsernameEdit);
                std::string password = GetWindowText(hAdminPasswordEdit);
                
                if (username.empty() || password.empty()) {
                    ShowMessage("Error", "Please enter username and password");
                    break;
                }
                
                std::string passwordHash = HashPassword(password);
                bool found = false;
                
                for (auto& admin : admins) {
                    if (admin.getUsername() == username && admin.getPasswordHash() == passwordHash) {
                        currentAdmin = &admin;
                        found = true;
                        break;
                    }
                }
                
                if (found) {
                    ShowMessage("Admin Login Successful", "Welcome Admin " + username + "!");
                    SetWindowText(hAdminUsernameEdit, "");
                    SetWindowText(hAdminPasswordEdit, "");
                } else {
                    ShowMessage("Error", "Invalid admin credentials");
                }
                break;
            }
            case 8:
            {
                if (!admins.empty()) {
                    ShowMessage("Error", "Admin already exists. Only one admin allowed.");
                    break;
                }
                
                std::string username = GetWindowText(hAdminUsernameEdit);
                std::string password = GetWindowText(hAdminPasswordEdit);
                
                if (username.empty() || password.empty()) {
                    ShowMessage("Error", "Please enter username and password");
                    break;
                }
                
                if (!ValidatePassword(password)) {
                    ShowMessage("Error", "Password must be at least 8 characters with uppercase, lowercase, digit, and special character");
                    break;
                }
                
                std::string passwordHash = HashPassword(password);
                std::string backupCode = GenerateOTP();
                admins.emplace_back(username, passwordHash, backupCode);
                SaveAdmins();
                
                ShowMessage("Admin Registration Successful", "Admin account created successfully!\n\nBackup code: " + backupCode);
                SetWindowText(hAdminUsernameEdit, "");
                SetWindowText(hAdminPasswordEdit, "");
                break;
            }
            case 9:
                ShowLoginWindow();
                break;
            case 10:
                PostQuitMessage(0);
                break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK PasswordResetWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_COMMAND) {
        switch (LOWORD(wParam)) {
            case 10:
            {
                std::string email = GetWindowText(hResetEmailEdit);
                
                if (email.empty()) {
                    ShowMessage("Error", "Please enter email address");
                    break;
                }
                
                if (!ValidateEmail(email)) {
                    ShowMessage("Error", "Invalid email format");
                    break;
                }
                
                bool found = false;
                for (const auto& user : users) {
                    if (user.getEmail() == email) {
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    ShowMessage("Error", "No account found with this email");
                    break;
                }
                
                currentOTP = GenerateOTP();
                currentResetEmail = email;
                SendOTPEmail(email, currentOTP);
                break;
            }
            case 11:
            {
                std::string otp = GetWindowText(hResetOTPEdit);
                std::string newPassword = GetWindowText(hResetNewPasswordEdit);
                std::string confirmPassword = GetWindowText(hResetConfirmPasswordEdit);
                
                if (otp.empty() || newPassword.empty() || confirmPassword.empty()) {
                    ShowMessage("Error", "All fields are required");
                    break;
                }
                
                if (otp != currentOTP) {
                    ShowMessage("Error", "Invalid OTP");
                    break;
                }
                
                if (!ValidatePassword(newPassword)) {
                    ShowMessage("Error", "Password must be at least 8 characters with uppercase, lowercase, digit, and special character");
                    break;
                }
                
                if (newPassword != confirmPassword) {
                    ShowMessage("Error", "Passwords do not match");
                    break;
                }
                
                for (auto& user : users) {
                    if (user.getEmail() == currentResetEmail) {
                        user.setPasswordHash(HashPassword(newPassword));
                        SaveUsers();
                        ShowMessage("Password Reset Successful", "Your password has been reset successfully!");
                        ClearPasswordResetForm();
                        ShowLoginWindow();
                        break;
                    }
                }
                break;
            }
            case 12:
                ClearPasswordResetForm();
                ShowLoginWindow();
                break;
            case 13:
                PostQuitMessage(0);
                break;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
