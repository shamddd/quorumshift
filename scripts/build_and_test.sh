#!/usr/bin/env bash
set -e

SDK_PATH="/Library/Developer/CommandLineTools/SDKs/MacOSX15.4.sdk"
if [ ! -d "$SDK_PATH" ]; then
    SDK_PATH=$(xcrun --show-sdk-path)
fi

CXX_FLAGS="-std=c++20 -isysroot $SDK_PATH -I$SDK_PATH/usr/include/c++/v1 -Iinclude"
CORE_SRCS="src/storage.cpp src/network.cpp src/node.cpp src/cluster.cpp src/logger.cpp src/quorum_engine.cpp src/adaptive_replica.cpp"

mkdir -p bin

echo "Compiling AdaptiveReplica core library & tests..."
xcrun clang++ $CXX_FLAGS $CORE_SRCS tests/test_storage.cpp -o bin/test_storage
xcrun clang++ $CXX_FLAGS $CORE_SRCS tests/test_consensus.cpp -o bin/test_consensus
xcrun clang++ $CXX_FLAGS $CORE_SRCS tests/test_network.cpp -o bin/test_network
xcrun clang++ $CXX_FLAGS $CORE_SRCS tests/test_cluster.cpp -o bin/test_cluster
xcrun clang++ $CXX_FLAGS $CORE_SRCS tests/test_quorum.cpp -o bin/test_quorum
xcrun clang++ $CXX_FLAGS $CORE_SRCS tests/test_adaptive_replica.cpp -o bin/test_adaptive_replica

xcrun clang++ $CXX_FLAGS $CORE_SRCS src/main.cpp -o bin/dss-cli
xcrun clang++ $CXX_FLAGS $CORE_SRCS src/benchmark_main.cpp -o bin/dss-bench

echo "Running test suite..."
./bin/test_storage
./bin/test_consensus
./bin/test_network
./bin/test_cluster
./bin/test_quorum
./bin/test_adaptive_replica

echo "All C++ AdaptiveReplica unit tests PASSED!"
