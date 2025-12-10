#ifndef BOOKSTORE_LOGMANAGER_HPP
#define BOOKSTORE_LOGMANAGER_HPP
#include <utility>

#include "MemoryRiver.hpp"
#include "UsersManager.hpp"

namespace Log {
using OPERATION_T = std::array<char, 300>;
};

struct FinanceLogEntry {
    long long timestamp;
    User::USERID_T userid;
    long long value;  // positive for income, negative for expense
};
struct OperationLogEntry {
    long long timestamp;
    User::USERID_T userid;
    Log::OPERATION_T op;
};

class LogManager {
public:
    static LogManager& getInstance();
    // positive for income, negative for expense
    void addFinanceLog(long long timestamp, User::USERID_T userid, long long value);
    // count must be greater than zero
    std::pair<long long, long long> getFinanceLog(int cnt = 0);

private:
    LogManager();
    ~LogManager() = default;
    MemoryRiver<FinanceLogEntry, 1> finance_log;
};

#endif  // BOOKSTORE_LOGMANAGER_HPP
