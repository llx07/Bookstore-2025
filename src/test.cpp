#include <bits/stdc++.h>

#include "MemoryRiver.hpp"

using namespace std;

int write() {

    MemoryRiver<unsigned long long> mr("tmp_file");

    return mr.write(1);
}

int main() {
    int pos = write();
    MemoryRiver<unsigned long long> mr("tmp_file");
    unsigned long long x;
    mr.read(x, pos);
    assert(x==1);


    return 0;
}