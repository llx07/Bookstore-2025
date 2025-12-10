#ifndef BOOKSTORE_UTILS_HPP
#define BOOKSTORE_UTILS_HPP
#include <string>
#include <vector>

namespace util {
// Splits a string by delim. Multiple consecutive delimiter will yields empty strings.
// Return an empty vectors if s is empty.
std::vector<std::string> split(const std::string& s, char delim = '|');
// Conver a std::string into std::array<char,N>
//
// template arguments:
//   T: the type of std::array<char, N>;
template <typename T>
T toArray(const std::string& s) {
    static_assert(std::is_same_v<T, std::array<char, std::tuple_size_v<T>>>,
                  "T must be std::array<char, N>");

    constexpr size_t CAPACITY = std::tuple_size_v<T>;
    if (s.size() >= CAPACITY) {
        throw std::out_of_range("String too long");
    }

    T result;
    result.fill(0);
    std::copy(s.data(), s.data() + s.size(), result.begin());
    return result;
}

// convert a string into int
// Disallow trailing zero
// Example:
// "0" -> 0
// "123" -> 123
// "-123" -> -123
// "abc" -> Invalid
// "1234567890123" -> Invalid
int toInt(const std::string& value);

// conver a string of fixed point representation(at most 2 digits) into long long, multiplied by
// 100, but disallow leading zeroes or stray decimal point
// Example:
// "1"     -> 100
// "1.23"  -> 123
// "1."    -> Invalid
// ".1"    -> Invalid
// "01.23" -> Invalid
// "1.234" -> Invalid
// "abc"   -> Invalid
// "1.."   -> Invalid
long long toDecimal(const std::string& value);

void outputDecimal(std::ostream& out, long long val);

void printTableHead(std::ostream& out, const std::vector<int>& lengths);
void printTableMiddle(std::ostream& out, const std::vector<int>& lengths);
void printTableBottom(std::ostream& out, const std::vector<int>& lengths);
void printTableBody(std::ostream& out, const std::vector<int>& lengths,
                    const std::vector<std::string>& head);

};  // namespace util
#endif  // BOOKSTORE_UTILS_HPP
