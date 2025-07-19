#include "email_sender.h"
#include <windows.h>
#include <wininet.h>
#include <sstream>

#pragma comment(lib, "wininet.lib")

EmailSender::EmailSender(const std::string& apiKey, const std::string& fromEmail)
    : apiKey(apiKey), fromEmail(fromEmail) {
}

bool EmailSender::SendOTPEmail(const std::string& toEmail, const std::string& otp) {
    HINTERNET hInternet = InternetOpenA("LoginSystem", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    HINTERNET hConnect = InternetConnectA(hInternet, "api.sendgrid.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", "/v3/mail/send", NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    std::string authHeader = "Authorization: Bearer " + apiKey;
    std::string contentTypeHeader = "Content-Type: application/json";

    std::string jsonData = "{\"personalizations\":[{\"to\":[{\"email\":\"" + toEmail + "\"}]}],\"from\":{\"email\":\"" + fromEmail + "\"},\"subject\":\"Password Reset OTP\",\"content\":[{\"type\":\"text/plain\",\"value\":\"Your OTP for password reset is: " + otp + "\"}]}";

    HttpAddRequestHeadersA(hRequest, authHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD);
    HttpAddRequestHeadersA(hRequest, contentTypeHeader.c_str(), -1, HTTP_ADDREQ_FLAG_ADD);

    BOOL result = HttpSendRequestA(hRequest, NULL, 0, (LPVOID)jsonData.c_str(), jsonData.length());
    
    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &statusCode, &statusCodeSize, NULL);

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return result && (statusCode >= 200 && statusCode < 300);
} 