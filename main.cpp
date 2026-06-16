#include <iostream>
#include <string>
#include <thread> // sleep
#include <chrono> // time
#include <fstream>
#include <filesystem>
#include <curl/curl.h>
#include "json.hpp"

#include "constants.hpp"
#include "telegram.hpp"

using json = nlohmann::json;

struct Secrets
{
    std::string tgToken;
    std::string tgChatId;
    std::string matrixDomain;
    std::string matrixRoomId;
    std::string matrixAccessToken;
} gSecrets;

static CURL *gCurl = nullptr;
static bool gIsOpen = false;
static Telegram::Bot *gTgBot = nullptr;
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

void RunSetupWizard()
{
    std::cout << "\n=============================================" << std::endl;
    std::cout << "     Essembly Monitor - First Setup Wizard   " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Leave fields empty if you want to skip that platform.\n"
              << std::endl;

    json cfg;
    std::string input;

    std::cout << "[Telegram] Enter Bot Token: ";
    std::getline(std::cin, input);
    cfg["telegram"]["token"] = input;

    std::cout << "[Telegram] Enter Chat/Channel ID (e.g. -100xxx): ";
    std::getline(std::cin, input);
    cfg["telegram"]["chatId"] = input;

    std::cout << "\n[Matrix] Enter Homeserver Domain (e.g. matrix.org): ";
    std::getline(std::cin, input);
    cfg["matrix"]["domain"] = input;

    std::cout << "[Matrix] Enter Room ID (e.g. !abc:matrix.org): ";
    std::getline(std::cin, input);
    cfg["matrix"]["roomId"] = input;

    std::cout << "[Matrix] Enter Access Token: ";
    std::getline(std::cin, input);
    cfg["matrix"]["accessToken"] = input;

    std::ofstream outFile("config.json");
    if (outFile.is_open())
    {
        outFile << cfg.dump(4);
        outFile.close();
        std::cout << "\n[SUCCESS] config.json created successfully!\n"
                  << std::endl;
    }
    else
    {
        std::cerr << "\n[ERROR] Could not write config.json!" << std::endl;
    }
}

bool LoadConfig()
{
    std::string configPath = "config.json";

    if (!std::filesystem::exists(configPath))
    {
        RunSetupWizard();
    }

    std::ifstream file(configPath);
    if (!file.is_open())
        return false;

    try
    {
        json cfg = json::parse(file);

        gSecrets.tgToken = cfg["telegram"].value("token", "");
        gSecrets.tgChatId = cfg["telegram"].value("chatId", "");
        gSecrets.matrixDomain = cfg["matrix"].value("domain", "");
        gSecrets.matrixRoomId = cfg["matrix"].value("roomId", "");
        gSecrets.matrixAccessToken = cfg["matrix"].value("accessToken", "");

        return true;
    }
    catch (...)
    {
        std::cerr << "[ERROR] config.json is corrupted! Delete it to run wizard again." << std::endl;
        return false;
    }
}

void App_Init()
{
    curl_global_init(CURL_GLOBAL_ALL);
    gCurl = curl_easy_init();

    if (!LoadConfig())
    {
        std::cerr << "[CRITICAL] Config failed to load." << std::endl;
        exit(1);
    }

    if (!gSecrets.tgToken.empty() && !gSecrets.tgChatId.empty())
    {
        gTgBot = new Telegram::Bot(gSecrets.tgToken);
        std::cout << "[INIT] Telegram module activated." << std::endl;
    }
    else
    {
        std::cout << "[INIT] Telegram module skipped (empty config)." << std::endl;
    }

    if (!gSecrets.matrixDomain.empty() && !gSecrets.matrixRoomId.empty() && !gSecrets.matrixAccessToken.empty())
    {
        std::cout << "[INIT] Matrix module activated (Pending implementation)." << std::endl;
    }
    else
    {
        std::cout << "[INIT] Matrix module skipped (empty config)." << std::endl;
    }

    bool current_status = false;
    if (Fetch_Status(current_status))
    {
        gIsOpen = current_status;
        std::string statusText = gIsOpen ? "🟢OPEN" : "🔴CLOSED";
        std::cout << "[SYSTEM] Status changed to: " << statusText << std::endl;

        std::string msg = "*[Essembly status update]*\n\nStatus: " + statusText + "\n\n#essembly\n@essembly\\_status";

        if (gTgBot)
        {
            gTgBot->sendMarkdownMessage(gSecrets.tgChatId, msg);
        }
    }

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
            std::string statusText = gIsOpen ? "🟢OPEN" : "🔴CLOSED";
            std::cout << "[SYSTEM] Status changed to: " << statusText << std::endl;

            std::string msg = "[Essembly status update]*\n\nStatus: " + statusText + "\n\n#essembly\n@essembly\\_status";

            if (gTgBot)
            {
                gTgBot->sendMarkdownMessage(gSecrets.tgChatId, msg);
            }
        }
    }
}

void App_Shutdown()
{
    if (gTgBot)
    {
        delete gTgBot;
    }
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