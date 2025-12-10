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
    int user_privilege;
    Log::OPERATION_T op;
    int is_success;
};

class LogManager {
public:
    static LogManager& getInstance();
    // positive for income, negative for expense
    void addFinanceLog(long long timestamp, User::USERID_T userid, long long value);
    // count must be greater than zero
    std::pair<long long, long long> getFinanceValue(int cnt = 0);

    int addOperationLog(long long timestamp, User::USERID_T userid, int privilege,
                        Log::OPERATION_T op);
    void markOperationSuccess(int id);

    int getFinanceLogCount();
    FinanceLogEntry getFinanceLogEntry(int id);
    int getOperationLogCount();
    OperationLogEntry getOperationLogEntry(int id);

    std::vector<std::pair<User::USERID_T, int>> getUserIdPairs();

private:
    LogManager();
    ~LogManager() = default;
    MemoryRiver<FinanceLogEntry, 1> finance_log;
    MemoryRiver<OperationLogEntry, 1> operation_log;
    BlockList<User::USERID_T, int> user_index;
};

#endif  // BOOKSTORE_LOGMANAGER_HPP
