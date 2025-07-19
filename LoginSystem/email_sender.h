#ifndef EMAIL_SENDER_H
#define EMAIL_SENDER_H

#include <string>

class EmailSender {
private:
    std::string apiKey;
    std::string fromEmail;

public:
    EmailSender(const std::string& apiKey, const std::string& fromEmail);
    bool SendOTPEmail(const std::string& toEmail, const std::string& otp);
};

#endif 