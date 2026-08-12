cmake_minimum_required(VERSION 3.20)
project(distributed_storage_simulator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Handle macOS C++ standard library include path if needed
if(APPLE)
    if(EXISTS "/Library/Developer/CommandLineTools/SDKs/MacOSX26.5.sdk/usr/include/c++/v1")
        include_directories("/Library/Developer/CommandLineTools/SDKs/MacOSX26.5.sdk/usr/include/c++/v1")
    elseif(EXISTS "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1")
        include_directories("/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/c++/v1")
    endif()
endif()

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter)
endif()

# Include directories
include_directories(include)

# Core library source files
set(CORE_SOURCES
    src/storage.cpp
    src/network.cpp
    src/node.cpp
    src/cluster.cpp
    src/logger.cpp
)

add_library(dss_core STATIC ${CORE_SOURCES})

# Threading library
find_package(Threads REQUIRED)
target_link_libraries(dss_core PUBLIC Threads::Threads)

# Main CLI Executable
add_executable(dss-cli src/main.cpp)
target_link_libraries(dss-cli PRIVATE dss_core)

# Benchmark Executable
add_executable(dss-bench src/benchmark_main.cpp)
target_link_libraries(dss-bench PRIVATE dss_core)

# Testing
enable_testing()

add_executable(test_storage tests/test_storage.cpp)
target_link_libraries(test_storage PRIVATE dss_core)
add_test(NAME test_storage COMMAND test_storage)

add_executable(test_consensus tests/test_consensus.cpp)
target_link_libraries(test_consensus PRIVATE dss_core)
add_test(NAME test_consensus COMMAND test_consensus)

add_executable(test_network tests/test_network.cpp)
target_link_libraries(test_network PRIVATE dss_core)
add_test(NAME test_network COMMAND test_network)

add_executable(test_cluster tests/test_cluster.cpp)
target_link_libraries(test_cluster PRIVATE dss_core)
add_test(NAME test_cluster COMMAND test_cluster)
