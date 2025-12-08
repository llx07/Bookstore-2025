#ifndef BOOKSTORE_UTILS_HPP
#define BOOKSTORE_UTILS_HPP
#include <string>
#include <vector>

namespace util {
// Splits a string by delim. Multiple consecutive delimiter will yields empty strings.
// Return an empty vectors if s is empty.
inline std::vector<std::string> split(const std::string& s, char delim = '|') {
    if (s.empty()) return {};
    std::vector<std::string> segments(1);
    for (auto ch : s) {
        if (ch == delim) {
            segments.push_back(std::string{});
        } else {
            segments.back().push_back(ch);
        }
    }
    return segments;
}

// Conver a std::string into std::array<char,N>
//
// template arguments:
//   T: the type of std::array<char, N>;
template <typename T>
T to_array(const std::string& s) {
    static_assert(std::is_same_v<T, std::array<char, std::tuple_size_v<T>>>,
                  "T must be std::array<char, N>");

    constexpr size_t CAPACITY = std::tuple_size_v<T>;
    assert(s.size() < CAPACITY);

    T result;
    result.fill(0);
    std::copy(s.data(), s.data() + s.size(), result.begin());
    return result;
}

};  // namespace util
#endif  // BOOKSTORE_UTILS_HPP
