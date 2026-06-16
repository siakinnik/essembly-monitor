#include <iostream>
#include <string>
#include <thread> // sleep
#include <chrono> // time
#include <curl/curl.h>
#include "json.hpp"

#include "constants.hpp"

using json = nlohmann::json;

static CURL *gCurl = nullptr;
static bool gIsOpen = false;

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s)
{
    s->append((char *)contents, size * nmemb);
    return size * nmemb;
}

bool Fetch_Status(bool &out_status)
{
    std::string response;
    curl_easy_reset(gCurl);
    curl_easy_setopt(gCurl, CURLOPT_URL, Config::apiUrl);
    curl_easy_setopt(gCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(gCurl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(gCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(gCurl, CURLOPT_TIMEOUT, 5L);

    if (curl_easy_perform(gCurl) != CURLE_OK)
        return false; // network error & etc.

    try
    {
        out_status = json::parse(response)["state"]["open"].get<bool>();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void App_Init()
{
    curl_global_init(CURL_GLOBAL_ALL);
    gCurl = curl_easy_init();
    std::cout << "[INIT] Essembly Monitor successfully initialized!" << std::endl;
}
void App_Update()
{
    bool current_status = false;
    if (Fetch_Status(current_status))
    {
        if (current_status != gIsOpen)
        {
            gIsOpen = current_status;
            std::cout << "[SYSTEM] Status changed to: " << (gIsOpen ? "OPEN" : "CLOSED") << std::endl;
        }
    }
}

void App_Shutdown()
{
    curl_easy_cleanup(gCurl);
    curl_global_cleanup();
}

int main()
{
    App_Init();

    // Super Loop
    while (true)
    {
        App_Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(Config::loopDelayMs));
    }

    App_Shutdown();
    return 0;
}