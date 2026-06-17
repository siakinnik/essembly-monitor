#include "matrix.hpp"
#include "json.hpp"
#include <curl/curl.h>
#include <iostream>

using json = nlohmann::json;

namespace Matrix {

static size_t dumbCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    return size * nmemb;
}

Client::Client(std::string domain, std::string accessToken) 
    : m_domain(std::move(domain)), m_accessToken(std::move(accessToken)) {}

bool Client::sendMessage(const std::string& roomId, const std::string& text) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string txid = std::to_string(std::chrono::system_clock::now().time_since_epoch().count());

    // Формируем URL
    std::string url = "https://" + m_domain + "/_matrix/client/r0/rooms/" + roomId + "/send/m.room.message/" + txid;

    json body;
    body["msgtype"] = "m.text";
    body["body"] = text;
    std::string jsonStr = body.dump();

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string authHeader = "Authorization: Bearer " + m_accessToken;
    headers = curl_slist_append(headers, authHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); // Matrix использует PUT для отправки событий по txid
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, dumbCallback);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    CURLcode res = curl_easy_perform(curl);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        std::cerr << "[MATRIX ERROR] " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    return true;
}

} // namespace Matrix