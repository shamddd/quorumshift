#!/usr/bin/env bash
set -e

SDK_PATH="/Library/Developer/CommandLineTools/SDKs/MacOSX15.4.sdk"
if [ ! -d "$SDK_PATH" ]; then
    SDK_PATH=$(xcrun --show-sdk-path)
fi

CXX_FLAGS="-std=c++20 -isysroot $SDK_PATH -I$SDK_PATH/usr/include/c++/v1 -Iinclude"

mkdir -p bin

echo "Compiling QuorumShift core library & tests..."
xcrun clang++ $CXX_FLAGS src/storage.cpp src/network.cpp src/node.cpp src/cluster.cpp src/logger.cpp src/quorum_engine.cpp tests/test_storage.cpp -o bin/test_storage
xcrun clang++ $CXX_FLAGS src/storage.cpp src/network.cpp src/node.cpp src/cluster.cpp src/logger.cpp src/quorum_engine.cpp tests/test_consensus.cpp -o bin/test_consensus
xcrun clang++ $CXX_FLAGS src/storage.cpp src/network.cpp src/node.cpp src/cluster.cpp src/logger.cpp src/quorum_engine.cpp tests/test_network.cpp -o bin/test_network
xcrun clang++ $CXX_FLAGS src/storage.cpp src/network.cpp src/node.cpp src/cluster.cpp src/logger.cpp src/quorum_engine.cpp tests/test_cluster.cpp -o bin/test_cluster
xcrun clang++ $CXX_FLAGS src/storage.cpp src/network.cpp src/node.cpp src/cluster.cpp src/logger.cpp src/quorum_engine.cpp tests/test_quorum.cpp -o bin/test_quorum

xcrun clang++ $CXX_FLAGS src/storage.cpp src/network.cpp src/node.cpp src/cluster.cpp src/logger.cpp src/quorum_engine.cpp src/main.cpp -o bin/dss-cli
xcrun clang++ $CXX_FLAGS src/storage.cpp src/network.cpp src/node.cpp src/cluster.cpp src/logger.cpp src/quorum_engine.cpp src/benchmark_main.cpp -o bin/dss-bench

echo "Running test suite..."
./bin/test_storage
./bin/test_consensus
./bin/test_network
./bin/test_cluster
./bin/test_quorum

echo "All C++ QuorumShift unit tests PASSED!"
