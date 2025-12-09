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
inline int toInt(const std::string& value) {
    if (value.empty()) {
        throw std::invalid_argument("Input string cannot be empty");
    }
    if (value.length() > 1) {
        if (value[0] == '0') {
            throw std::invalid_argument("Leading zeros are not allowed");
        }
        if (value[0] == '-' && value.length() > 2 && value[1] == '0') {
            throw std::invalid_argument("Leading zeros are not allowed after sign");
        }
    }
    size_t pos = 0;
    int result;

    try {
        result = std::stoi(std::string(value), &pos);
    } catch (const std::out_of_range&) {
        throw std::out_of_range("Input value is out of integer range");
    } catch (const std::invalid_argument&) {
        throw std::invalid_argument("Invalid character in input string");
    }

    // make sure the whole string is consumed
    if (pos != value.length()) {
        throw std::invalid_argument("Invalid character in input string");
    }
    return result;
}

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
inline long long toDecimal(const std::string& value) {
    if (value.empty()) {
        throw std::invalid_argument("Input string cannot be empty");
    }
    if (value.length() > 1 && value[0] == '0' && value[1] != '.') {
        throw std::invalid_argument("Leading zeros are not allowed");
    }
    auto dot_pos = value.find('.');
    if (dot_pos == std::string_view::npos) {
        size_t pos = 0;
        auto integer_value = std::stoll(value, &pos);
        if (pos != value.size()) {
            throw std::invalid_argument("Invalid character before decimal point");
        }
        return integer_value * 100;
    }
    if (dot_pos == 0 || dot_pos == value.length() - 1) {
        throw std::invalid_argument("Stray decimal point is not allowed");
    }

    std::string integer_part = value.substr(0, dot_pos);
    std::string decimal_part = value.substr(dot_pos + 1);

    if (decimal_part.size() > 2) {
        throw std::invalid_argument("At most 2 digits are allowed");
    }

    size_t pos = 0;
    auto integer_value = std::stoll(integer_part, &pos);
    if (pos != integer_part.size()) {
        throw std::invalid_argument("Invalid character before decimal point");
    }

    auto decimal_value = std::stoll(decimal_part, &pos);
    if (pos != decimal_part.size()) {
        throw std::invalid_argument("Invalid character after decimal point");
    }
    if (decimal_part.size() == 1) {
        decimal_value *= 10;
    }
    return integer_value * 100 + decimal_value;
}

};  // namespace util
#endif  // BOOKSTORE_UTILS_HPP
