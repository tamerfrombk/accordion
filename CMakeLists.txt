CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
PROJECT(accordion VERSION 1.0.0)

SET(CMAKE_C_STANDARD 11)
SET(CMAKE_C_STANDARD_REQUIRED ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# @PLATFORM: this works with Clang/GCC but will be an issue with MSVC
SET(CMAKE_C_WARNING_FLAGS "-Wall -Wextra -Wpedantic")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_WARNING_FLAGS}")

SET(APP_EXE accordion)

ADD_EXECUTABLE(${APP_EXE} src/main.c src/url_repo.c src/http_connection.c)

TARGET_INCLUDE_DIRECTORIES(${APP_EXE} PRIVATE ${CMAKE_SOURCE_DIR}/include)
TARGET_LINK_LIBRARIES(${APP_EXE} microhttpd hiredis)
