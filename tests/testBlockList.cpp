#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>

#include "BlockList.hpp"

TEST_CASE("BlockList Basic Insert", "[BlockList]") {
    std::filesystem::remove("data_head");
    std::filesystem::remove("data_body");
    Blocklist<int, int> blocklist;
    blocklist.initialise("data");
    auto data = std::vector<int>{6, 1, 2, 3, 4};
    const int key = 114514;
    for (auto v : data) blocklist.insert(key, v);
    std::ranges::sort(data);
    auto result = blocklist.query(key);
    // std::cerr << "Result:";
    // for (auto v:result) {
    //     std::cerr << v << ' ';
    // }
    // std::cerr << '\n';
    REQUIRE(result == data);
}

TEST_CASE("BlockList Massive Insert", "[BlockList]") {
    std::filesystem::remove("data_head");
    std::filesystem::remove("data_body");
    Blocklist<int, unsigned> blocklist;
    blocklist.initialise("data");
    const int N = 10000;
    std::vector<unsigned> data;
    std::mt19937 gen{14514};
    for (int i = 0; i < N; i++) {
        data.push_back(gen());
    }
    const int key = 114514;
    for (auto v : data) blocklist.insert(key, v);
    std::ranges::sort(data);
    auto result = blocklist.query(key);
    // std::cerr << "Result:";
    // for (auto v:result) {
    //     std::cerr << v << ' ';
    // }
    // std::cerr << '\n';
    REQUIRE(result == data);
}

TEST_CASE("BlockList Basic Delete", "[BlockList]") {
    std::filesystem::remove("data_head");
    std::filesystem::remove("data_body");
    Blocklist<int, int> blocklist;
    blocklist.initialise("data");
    auto data = std::vector<int>{6, 1, 2, 3, 4};
    const int key = 114514;
    for (auto v : data) blocklist.insert(key, v);
    std::ranges::sort(data);
    auto result = blocklist.query(key);
    REQUIRE(result == data);
    for (auto v : data) blocklist.erase(key, v);
    result = blocklist.query(key);
    REQUIRE(result == std::vector<int>{});
    for (auto v : data) blocklist.insert(key, v);
    result = blocklist.query(key);
    REQUIRE(result == data);
}

TEST_CASE("BlockList Delete Nonexist element", "[BlockList]") {
    std::filesystem::remove("data_head");
    std::filesystem::remove("data_body");
    Blocklist<int, int> blocklist;
    blocklist.initialise("data");
    for (int i = 1; i <= 10; i++) {
        blocklist.insert(-i, -i);
    }
    std::mt19937 gen{14514};
    for (int i = 0; i < 10000; i++) {
        blocklist.erase(gen() % 100000, gen() % 100000);
    }

    for (int i = 1; i <= 10; i++) {
        auto result = blocklist.query(-i);
        REQUIRE(result == std::vector<int>{-i});
    }
}
TEST_CASE("BlockList Reopen", "[BlockList]") {
    std::filesystem::remove("data_head");
    std::filesystem::remove("data_body");
    std::vector<unsigned> data;
    const int N = 10000;
    std::mt19937 gen{14514};
    const int key = 114514;
    for (int i = 0; i < N; i++) {
        data.push_back(gen());
    }
    {
        Blocklist<int, unsigned> blocklist;
        blocklist.initialise("data");
        for (auto v : data) blocklist.insert(key, v);
    }
    std::ranges::sort(data);
    {
        Blocklist<int, unsigned> blocklist;
        blocklist.initialise("data");
        auto result = blocklist.query(key);
        REQUIRE(result == data);
    }
}