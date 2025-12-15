cmake_minimum_required(VERSION 3.10)
project(HealthBackend)

# Use C++17 (Required for std::filesystem in Logger.cpp and inline variables)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Make sure CMake re-runs if new source files are added
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")

# 1. Find System Dependencies
find_package(Threads REQUIRED)

# 2. Create the executable from all discovered sources
add_executable(HealthServer ${SOURCES})

# 3. Include directories for project headers and third-party single-file libs
target_include_directories(HealthServer PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
)

# 4. Link libraries
target_link_libraries(HealthServer PRIVATE Threads::Threads)

# Convenience: place generated binaries in `build/bin`
set_target_properties(HealthServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)