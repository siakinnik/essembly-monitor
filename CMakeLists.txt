cmake_minimum_required(VERSION 3.15)
project(EssemblyMonitor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CURL_STATICLIB ON CACHE BOOL "" FORCE)

set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_LDAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TELNET ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_DICT ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_TFTP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_IMAP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_POP3 ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_SMTP ON CACHE BOOL "" FORCE)
set(CURL_DISABLE_GOPHER ON CACHE BOOL "" FORCE)

if(WIN32)
    set(CURL_USE_SCHANNEL ON CACHE BOOL "" FORCE)
else()
    set(CURL_USE_OPENSSL ON CACHE BOOL "" FORCE)
endif()

file(DOWNLOAD 
    "https://github.com/nlohmann/json/releases/latest/download/json.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/json.hpp"
)

include(FetchContent)
FetchContent_Declare(
  curl
  URL https://github.com/curl/curl/archive/refs/tags/curl-8_1_2.tar.gz
)
FetchContent_MakeAvailable(curl)

add_executable(essembly_bot main.cpp telegram.cpp matrix.cpp)
target_link_libraries(essembly_bot PRIVATE libcurl)