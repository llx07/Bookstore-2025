#include <algorithm>
#include <array>
#include <iostream>
#include <string>

#include "BlockList.hpp"

using string64 = std::array<char, 64>;

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int n;
    std::cin >> n;

    Blocklist<string64, int> data;
    data.initialise("data");

    auto get_string64 = []() -> string64 {
        std::string name;
        std::cin >> name;
        string64 nm;
        std::fill(nm.begin(), nm.end(), 0);
        for (int i = 0; i < name.size(); i++) {
            nm[i] = name[i];
        }
        return nm;
    };

    while (n--) {
        std::string op;
        std::cin >> op;
        if (op[0] == 'i') {
            auto nm = get_string64();
            int value;
            std::cin >> value;
            data.insert(nm, value);
        } else if (op[0] == 'f') {
            auto nm = get_string64();
            auto res = data.query(nm);
            if (res.empty())
                std::cout << "null";
            else
                for (int v : res) {
                    std::cout << v << ' ';
                }
            std::cout << '\n';
        } else if (op[0] == 'd') {
            auto nm = get_string64();
            int value;
            std::cin >> value;
            data.erase(nm, value);
        }
    }
    return 0;
}