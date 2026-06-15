#include <iostream>
#include <string>
#include <curl/curl.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    std::cout << "[INFO] Initializing..." << std::endl;

    if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
        std::cerr << "[ERROR] curl_global_init failed!" << std::endl;
        return 1;
    }

    CURL* curl = curl_easy_init();
    if (curl) {
        std::string response;
        std::cout << "[INFO] Fetching JSON from Essembly..." << std::endl;
        
        curl_easy_setopt(curl, CURLOPT_URL, "https://essembly.de/spaceapi.json");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); 
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); 

        CURLcode res = curl_easy_perform(curl);
        
        if (res == CURLE_OK) {
            std::cout << "[SUCCESS] Response size: " << response.length() << " bytes" << std::endl;
            std::cout << "[DATA] " << response.substr(0, 150) << "..." << std::endl;
        } else {
            std::cerr << "[ERROR] curl_easy_perform failed: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    } else {
        std::cerr << "[ERROR] curl_easy_init failed!" << std::endl;
    }

    curl_global_cleanup();
    std::cout << "[INFO] Finished." << std::endl;
    return 0;
}