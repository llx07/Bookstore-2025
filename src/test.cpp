#include <bits/stdc++.h>

#include "MemoryRiver.hpp"

using namespace std;

int main() { 
    MemoryRiver<unsigned long long> mr("tmp_file");
    mr.initialise();
    mr.write_info(114, 1);
    int tmp;
    mr.get_info(tmp, 1);
    assert(tmp == 114);

    vector<int> pos;
    for(int i=0;i<100;i++){
        pos.push_back(mr.write(i));
    }
    
    for(int i=0;i<100;i++){
        unsigned long long val;
        mr.read(val, pos[i]);
        cerr << "i = " << i << ", val = " << val << endl;
        cerr << "i = " << i << ", pos = " << pos[i] << endl;
        assert(val == i);
        val <<= 32;
        mr.update(val, pos[i]);
    }
    for(int i=0;i<100;i++){
        unsigned long long val;
        mr.read(val, pos[i]);
        cerr << "i = " << i << ", val = " << val << endl;
        cerr << "i = " << i << ", pos = " << pos[i] << endl;
        assert((val>>32) == i);
        mr.Delete(pos[i]);
    }

    pos.clear();
    for(int i=0;i<100;i++){
        pos.push_back(mr.write(i));
        cerr << pos.back() << '\n';
    }

    assert(*max_element(pos.begin(), pos.end()) == 100);


    return 0; 

}