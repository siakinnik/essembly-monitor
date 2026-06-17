# Essembly Monitor

`Essembly Monitor` is a lightweight, cross-platform C++17 console bot that monitors the status of the **Essembly** hackerspace/community via its SpaceAPI. It instantly broadcasts state changes (🟢OPEN / 🔴CLOSED) to **Telegram** channels/chats and/or **Matrix** rooms.

---

## Features

* **Multi-Platform Notifications:** Simultaneous or selective delivery to both Telegram and Matrix.
* **Built-in Setup Wizard:** Automatically triggers on the first launch to guide you through token/ID generation and generates the `config.json` file.
* **Modular Infrastructure:** Leaving Telegram or Matrix credentials empty during setup will gracefully skip that specific module without breaking the application.
* **Automated Dependency Management:** Uses CMake `FetchContent` to download `libcurl` and automatically fetches the latest `nlohmann/json.hpp` header at configuration time.

---

## Requirements & Dependencies

To build and run this project, ensure you have:

* A compiler supporting **C++17** (GCC, Clang, MSVC)
* **CMake** version 3.15 or higher
* SSL development libraries installed on your system (e.g., `openssl` for Linux/macOS. Windows uses the native `Schannel` provider).

---

## Building the Project

The project is configured to build a stripped-down, static version of `libcurl` to minimize external runtime dependencies.

1. Clone or download the source code.
2. Open your terminal in the project root directory and run:

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Once compiled, you will find the `essembly_bot` executable in your build folder.

## Configuration & First Run

When you execute essembly_bot for the first time, it will detect that config.json is missing and launch the interactive First Setup Wizard directly in your console:

``` bash
=============================================
     Essembly Monitor - First Setup Wizard   
=============================================
Leave fields empty if you want to skip that platform.

[Telegram] Enter Bot Token: 
[Telegram] Enter Chat/Channel ID (e.g. -100xxx): 

[Matrix] Enter Homeserver Domain (e.g. matrix.org): 
[Matrix] Enter Room ID (e.g. !abc:matrix.org): 
[Matrix] Enter Access Token:
```

💡 Tip: If you only want to use Telegram, just press Enter to leave the Matrix fields empty (or vice versa). The bot will adapt accordingly.

### Manual Configuration

Alternatively, you can manually create a config.json file in the same directory as the executable:

``` JSON
{
    "telegram": {
        "token": "123456789:ABCdefGhIJKlmNoPQRsTUVwxyZ",
        "chatId": "-100123456789"
    },
    "matrix": {
        "domain": "matrix.org",
        "roomId": "!abcdeFGHijKLmnoPQR:matrix.org",
        "accessToken": "syt_YOUR_MATRIX_ACCESS_TOKEN_HERE"
    }
}
```

If your configuration file ever gets corrupted, simply delete config.json and restart the application to trigger the wizard again.

## Internals

* **API Endpoint: [https://essembly.de/spaceapi.json](https://essembly.de/spaceapi.json)**

* **Polling Interval: The bot queries the endpoint every 6000ms (6 seconds), as defined in Config::loopDelayMs within constants.hpp.**

* **Network Protocol: Matrix messages are dispatched using standard HTTP PUT requests paired with high-resolution unique transaction IDs (txid) to ensure reliable synchronization.**
