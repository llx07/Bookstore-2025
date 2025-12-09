#ifndef BOOKSTORE_VALIDATOR_HPP
#define BOOKSTORE_VALIDATOR_HPP

#include <algorithm>
#include <exception>
#include <regex>
#include <type_traits>

#include "Utils.hpp"

class ExpectException : public std::exception {
    const char* what() const noexcept override { return "Validator failed."; };
};

template <typename T>
struct is_basic_string : std::false_type {};

template <typename CharT, typename Traits, typename Alloc>
struct is_basic_string<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};

template <typename T>
inline constexpr bool is_basic_string_v = is_basic_string<T>::value;

template <typename T>
class expect {
public:
    explicit expect(const T& _value)
        : And(*this), Or(*this), but(*this), value(_value), inverted(false) {}
    // Validates if value is equal to other
    expect& toBe(const T& other) {
        bool is_ok = value == other;
        if (is_ok == inverted) {
            throw ExpectException{};
        }
        return *this;
    }
    template <typename U>
    // Validates if value is an instance of U
    expect& toBe() {
        static_assert(std::is_base_of_v<T, U>, "U must be derived from T");
        bool is_ok = dynamic_cast<const U*>(&value) != nullptr;
        if (is_ok == inverted) {
            throw ExpectException{};
        }
        return *this;
    }
    template <typename... Args>
    expect& toBeOneOf(Args... args) {
        static_assert((std::is_convertible_v<Args, T> && ...), "All arguments must be T");
        bool is_ok = ((args == value) || ...);
        if (is_ok == inverted) {
            throw ExpectException{};
        }
        return *this;
    }
    expect& le(const T& other) {
        bool is_ok = value <= other;
        if (is_ok == inverted) {
            throw ExpectException{};
        }
        return *this;
    }
    expect& ge(const T& other) {
        bool is_ok = value >= other;
        if (is_ok == inverted) {
            throw ExpectException{};
        }
        return *this;
    }
    expect& Not() {
        inverted = !inverted;
        return *this;
    }

    // Check if all characters of value is in pat>
    expect& consistedOf(const T& pat)
        requires is_basic_string_v<T>  // use requires here to prevent compile errors.
    {
        using CharT = typename T::value_type;
        bool is_ok = std::ranges::all_of(value, [&pat](const CharT& ch) {
            return pat.find(ch) != std::basic_string<CharT>::npos;
        });
        if (is_ok == inverted) {
            throw ExpectException{};
        }
        return *this;
    }

    // Check if value match the regex pattern reg.
    // This may cause performance issues.
    expect& toMatch(const T& reg_exp)
        requires is_basic_string_v<T>
    {
        using CharT = typename T::value_type;
        std::basic_regex<CharT> pat(reg_exp);
        bool is_ok = std::regex_match(value, pat);
        if (is_ok == inverted) {
            throw ExpectException{};
        }
        return *this;
    }
    // Check if the function returns true.
    template <typename Pred>
    expect& satisfy(Pred p) {
        bool is_ok = p(value);
        if (is_ok == inverted) {
            throw ExpectException{};
        }
        return *this;
    }

    expect& And;
    expect& Or;
    expect& but;

    // Convert a string to std::array.
    // May throw std::out_of_range
    template <typename CharArrayT>
    CharArrayT toCharArray()
        requires is_basic_string_v<T>
    {
        return util::to_array<CharArrayT>(value);
    }

    // convert a string into int
    // Disallow trailing zero
    // Example:
    // "0" -> 0
    // "123" -> 123
    // "-123" -> -123
    // "abc" -> Invalid
    // "1234567890123" -> Invalid
    int toInt()
        requires is_basic_string_v<T>
    {
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
    long long toDecimal()
        requires is_basic_string_v<T>
    {
        if (value.empty()) {
            throw std::invalid_argument("Input string cannot be empty");
        }
        if (value.length() > 1 && value[0] == '0' && value[1] != '.') {
            throw std::invalid_argument("Leading zeros are not allowed");
        }
        auto dot_pos = value.find('.');
        if (dot_pos == std::string_view::npos) {
            return toInt() * 100;
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
            throw std::invalid_argument("Invalid character before decimla point");
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

private:
    const T value;
    bool inverted;
};
#endif  // BOOKSTORE_VALIDATOR_HPP
