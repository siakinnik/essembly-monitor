#pragma once

#include <string>

namespace Telegram {

class Bot {
public:
    explicit Bot(std::string token);
    bool sendMessage(const std::string& chatId, const std::string& text);
    bool sendMarkdownMessage(const std::string& chatId, const std::string& text);

private:
    std::string m_token;
};

} // namespace Telegram