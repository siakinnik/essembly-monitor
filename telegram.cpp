#include "telegram.hpp"
#include <curl/curl.h>
#include <iostream>

namespace Telegram {

static size_t dumbCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}

Bot::Bot(std::string token) : m_token(std::move(token)) {}

bool Bot::sendMessage(const std::string& chatId, const std::string& text) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    char* escapedText = curl_easy_escape(curl, text.c_str(), static_cast<int>(text.length()));
    
    std::string url = "https://api.telegram.org/bot" + m_token + 
                      "/sendMessage?chat_id=" + chatId + 
                      "&text=" + std::string(escapedText);
    
    curl_free(escapedText); 

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dumbCallback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "[TELEGRAM ERROR] " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    return true;
}

} // namespace Telegram