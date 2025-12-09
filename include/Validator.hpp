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

private:
    const T value;
    bool inverted;
};
#endif  // BOOKSTORE_VALIDATOR_HPP
