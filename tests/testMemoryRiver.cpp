#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "MemoryRiver.hpp"

TEST_CASE("MemoryRiver Info Read/Write", "[MemoryRiver]") {
    std::filesystem::remove("tmp_file");

    constexpr int N = 1024;
    int pos;
    {
        MemoryRiver<int, N> mr("tmp_file");
        mr.initialise();
        for (int i = 1; i <= N; i++) {
            mr.write_info(i * i, i);
        }
        pos = mr.write(200);
    }
    {
        MemoryRiver<int, N> mr("tmp_file");
        mr.initialise();
        // std::cerr << " ------------ \n";
        int tmp;
        for (int i = 1; i <= N; i++) {
            // std::cerr << "i = " << i << '\n';
            mr.get_info(tmp, i);
            // std::cerr << "data = " << tmp << '\n';
            REQUIRE(tmp == i * i);
        }
        mr.read(tmp, pos);
        REQUIRE(tmp == 200);
    }
}

TEST_CASE("MemoryRiver Basic Read/Write", "[MemoryRiver]") {
    std::filesystem::remove("tmp_file");
    MemoryRiver<unsigned long long> mr("tmp_file");
    mr.initialise();

    std::vector<int> pos;
    for (int i = 0; i < 100; i++) {
        pos.push_back(mr.write(i));
    }

    for (unsigned long long i = 0; i < 100; i++) {
        unsigned long long val;
        mr.read(val, pos[i]);
        REQUIRE(val == i);
        val <<= 32;
        mr.update(val, pos[i]);
    }

    pos.clear();
    for (int i = 0; i < 100; i++) {
        pos.push_back(mr.write(i));
    }
}

TEST_CASE("MemoryRiver Persistent Read/Write", "[MemoryRiver]") {
    std::filesystem::remove("tmp_file");
    const int N = 100;
    std::vector<int> data;
    std::mt19937 gen{114514};
    for (int i = 0; i < N; i++) {
        data.push_back(gen());
    }
    std::vector<int> pos(data.size());
    do {
        MemoryRiver<int> mr("tmp_file");
        mr.initialise();
        for (int i = 0; i < N; i++) {
            pos[i] = mr.write(data[i]);
        }
    } while (false);

    do {
        MemoryRiver<int> mr("tmp_file");
        mr.initialise();
        for (int i = 0; i < N; i++) {
            int x;
            mr.read(x, pos[i]);
            REQUIRE(x == data[i]);
        }

        for (int i = 0; i < N; i++) {
            mr.write(i);
        }

        for (int i = 0; i < N; i++) {
            int x;
            mr.read(x, pos[i]);
            REQUIRE(x == data[i]);
        }
    } while (false);
}

TEST_CASE("MemoryRiver Delete", "[MemoryRiver]") {
    std::filesystem::remove("tmp_file");
    MemoryRiver<unsigned> mr;
    mr.initialise("tmp_file");

    std::vector<int> pos;
    for (int i = 0; i < 100; i++) {
        pos.push_back(mr.write(i));
    }

    for (unsigned i = 0; i < 100; i++) {
        unsigned val;
        mr.read(val, pos[i]);
        REQUIRE(val == i);
        mr.Delete(pos[i]);
    }

    pos.clear();
    for (int i = 0; i < 100; i++) {
        pos.push_back(mr.write(i * 100));
    }

    for (unsigned i = 0; i < 100; i++) {
        unsigned val;
        mr.read(val, pos[i]);
        REQUIRE(val == i * 100);
    }
}

TEST_CASE("MemoryRiver Delete Multiple Times With Info", "[MemoryRiver]") {
    auto get_size = []() {
        std::fstream file;
        file.open("tmp_file", std::ios::in);
        auto st = file.tellp();
        file.seekp(0, std::ios::end);
        auto ed = file.tellp();
        return ed - st;
    };
    std::filesystem::remove("tmp_file");

    int size_before;
    {
        MemoryRiver<unsigned, 100> mr;
        mr.initialise("tmp_file");

        std::vector<int> pos;
        for (int i = 0; i < 100; i++) {
            mr.write_info(i, i + 1);
            pos.push_back(mr.write(i));
        }

        for (unsigned i = 0; i < 100; i++) {
            unsigned val;
            mr.read(val, pos[i]);
            REQUIRE(val == i);
            mr.Delete(pos[i]);
        }

        size_before = get_size();
    }

    {
        MemoryRiver<unsigned, 100> mr;
        mr.initialise("tmp_file");

        std::vector<int> pos;
        for (int i = 0; i < 100; i++) {
            pos.push_back(mr.write(i * 100));
        }

        for (unsigned i = 0; i < 100; i++) {
            int tmp;
            mr.get_info(tmp, i + 1);
            REQUIRE(tmp == i);
            unsigned val;
            mr.read(val, pos[i]);
            REQUIRE(val == i * 100);
        }

        int size_after = get_size();

        REQUIRE(size_before == size_after);
    }
}