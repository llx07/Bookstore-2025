#ifndef BOOKSTORE_UTILS_HPP
#define BOOKSTORE_UTILS_HPP
#include <cassert>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Utils.hpp"

namespace util {
std::vector<std::string> split(const std::string& s, char delim = '|') {
    if (s.empty()) return {};
    std::vector<std::string> segments(1);
    for (auto ch : s) {
        if (ch == delim) {
            segments.emplace_back();
        } else {
            segments.back().push_back(ch);
        }
    }
    return segments;
}

int toInt(const std::string& value) {
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

long long toDecimal(const std::string& value) {
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

void outputDecimal(std::ostream& out, long long val) {
    out << val / 100 << "." << std::setw(2) << std::setfill('0') << val % 100;
}

void printTableHead(std::ostream& out, const std::vector<int>& lengths) {
    out << "┌";
    for (int i = 0; i < lengths.size(); i++) {
        for (int j = 0; j < lengths[i]; j++) out << "─";
        if (i == lengths.size() - 1) {
            out << "┐";
        } else {
            out << "┬";
        }
    }
    out << "\n";
}
void printTableMiddle(std::ostream& out, const std::vector<int>& lengths) {
    out << "├";
    for (int i = 0; i < lengths.size(); i++) {
        for (int j = 0; j < lengths[i]; j++) out << "─";
        if (i == lengths.size() - 1) {
            out << "┤";
        } else {
            out << "┼";
        }
    }
    out << "\n";
}
void printTableBottom(std::ostream& out, const std::vector<int>& lengths) {
    out << "└";
    for (int i = 0; i < lengths.size(); i++) {
        for (int j = 0; j < lengths[i]; j++) out << "─";
        if (i == lengths.size() - 1) {
            out << "┘";
        } else {
            out << "┴";
        }
    }
    out << "\n";
}
void printTableBody(std::ostream& out, const std::vector<int>& lengths,
                    const std::vector<std::string>& head) {
    assert(head.size() == lengths.size());
    out << "│";

    for (int i = 0; i < lengths.size(); i++) {
        out << std::setw(lengths[i]) << std::setfill(' ') << std::left;
        if (head[i].size() > lengths[i]) {
            int have_length = std::min((int)head[i].size(), lengths[i] - 3);
            std::string data = head[i].substr(0, have_length) + "...";
            out << data;
        } else {
            out << head[i];
        }
        out << "│";
    }
    out << "\n";
}

};  // namespace util
#endif  // BOOKSTORE_UTILS_HPP
