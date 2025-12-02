#include <bits/stdc++.h>

#include <catch2/catch_test_macros.hpp>

#include "MemoryRiver.hpp"

using namespace std;

TEST_CASE("MemoryRiver Basic Read/Write", "[MemoryRiver]") {
    MemoryRiver<unsigned long long> mr("tmp_file");
    mr.initialise();
    mr.write_info(114, 1);
    int tmp;
    mr.get_info(tmp, 1);
    REQUIRE(tmp == 114);

    vector<int> pos;
    for (int i = 0; i < 100; i++) {
        pos.push_back(mr.write(i));
    }

    for (unsigned long long i = 0; i < 100; i++) {
        unsigned long long val;
        mr.read(val, pos[i]);
        cerr << "i = " << i << ", val = " << val << endl;
        cerr << "i = " << i << ", pos = " << pos[i] << endl;
        REQUIRE(val == i);
        val <<= 32;
        mr.update(val, pos[i]);
    }
    for (unsigned long long i = 0; i < 100; i++) {
        unsigned long long val;
        mr.read(val, pos[i]);
        REQUIRE((val >> 32) == i);
        mr.Delete(pos[i]);
    }

    pos.clear();
    for (int i = 0; i < 100; i++) {
        pos.push_back(mr.write(i));
    }

    remove("tmp_file");
}

TEST_CASE("MemoryRiver Persistent Read/Write", "[MemoryRiver]") {
    const int N = 100;
    vector<int> data;
    mt19937 gen{114514};
    for (int i = 0; i < N; i++) {
        data.push_back(gen());
    }
    vector<int> pos(data.size());
    do {
        MemoryRiver<int> mr("tmp_file");
        mr.initialise();
        for (int i=0;i<N;i++) {
            pos[i] = mr.write(data[i]);
        }
    } while (false);


    do {
        MemoryRiver<int> mr("tmp_file");
        mr.initialise();
        for (int i=0;i<N;i++) {
            int x;
            mr.read(x, pos[i]);
            REQUIRE(x == data[i]);
        }

        for (int i=0;i<N;i++) {
            mr.write(i);
        }

        for (int i=0;i<N;i++) {
            int x;
            mr.read(x, pos[i]);
            REQUIRE(x == data[i]);
        }
    } while (false);
}
