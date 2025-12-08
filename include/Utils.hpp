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

};  // namespace util
#endif  // BOOKSTORE_UTILS_HPP
