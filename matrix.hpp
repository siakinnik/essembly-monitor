#pragma once
#include <string>

namespace Matrix {

class Client {
public:
    Client(std::string domain, std::string accessToken);
    bool sendMessage(const std::string& roomId, const std::string& text);

private:
    std::string m_domain;
    std::string m_accessToken;
};

} // namespace Matrix