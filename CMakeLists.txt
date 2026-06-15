cmake_minimum_required(VERSION 3.15)
project(EssemblyMonitor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  curl
  URL https://github.com/curl/curl/archive/refs/tags/curl-8_1_2.tar.gz
)

set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(curl)

add_executable(essembly_bot main.cpp)

# Линкуем
target_link_libraries(essembly_bot PRIVATE libcurl)