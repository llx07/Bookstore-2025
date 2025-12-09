#ifndef BOOKSTORE_CHARSET_HPP
#define BOOKSTORE_CHARSET_HPP

#include <string>

inline const std::string NUMERIC = "0123456789";
inline const std::string NUMERIC_DOT = "0123456789.";
inline const std::string ALPHANUMERIC_UNDERSCORE =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789_";
inline std::string generateVisibleAscii(char exclude = '\0') {
    std::string result;
    for (char c = 32; c <= 126; ++c) {
        if (c == exclude) {
            continue;
        }
        result += c;
    }
    return result;
}
inline const std::string PRINTABLE = generateVisibleAscii();
inline const std::string PRINTABLE_WITHOUT_QUOTES = generateVisibleAscii('"');
inline const std::string PRINTABLE_WITHOUT_BAR = generateVisibleAscii('|');

#endif  // BOOKSTORE_CHARSET_HPP
