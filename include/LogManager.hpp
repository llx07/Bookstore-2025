#ifndef BOOKSTORE_LOGMANAGER_HPP
#define BOOKSTORE_LOGMANAGER_HPP
#include <utility>

#include "MemoryRiver.hpp"

class LogManager {
public:
    static LogManager& getInstance();
    // positive for income, negative for expense
    void add_finance_log(long long value);
    // count must be greater than zero
    std::pair<long long, long long> get_finance_log(int cnt = 0);

private:
    LogManager();
    ~LogManager() = default;
    MemoryRiver<long long, 1> finance_log;
};

#endif  // BOOKSTORE_LOGMANAGER_HPP
