cmake_minimum_required(VERSION 3.10)
project(health_backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR})

set(SRC
    src/server.cpp
    src/backend/HealthBackend.cpp
    src/helpers/Logger.cpp
)

add_executable(server_app ${SRC})
target_compile_options(server_app PRIVATE -Wall -Wextra -O2)
