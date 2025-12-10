#include "LogManager.hpp"

#include <cmath>

#include "Utils.hpp"

LogManager& LogManager::getInstance() {
    static LogManager instance;
    return instance;
}
void LogManager::addFinanceLog(long long timestamp, User::USERID_T userid, long long value) {
    int log_count;
    finance_log.getInfo(log_count, 1);
    FinanceLogEntry entry{timestamp, userid, value};
    finance_log.write(entry);
    ++log_count;
    finance_log.writeInfo(log_count, 1);
}
std::pair<long long, long long> LogManager::getFinanceValue(int cnt) {
    int log_count;
    finance_log.getInfo(log_count, 1);
    if (!cnt) cnt = log_count;

    if (log_count < cnt) {
        throw std::out_of_range("cnt is too large");
    }
    long long income = 0, expense = 0;
    for (int i = log_count; i > log_count - cnt; i--) {
        FinanceLogEntry entry;
        finance_log.read(entry, i);
        long long value = entry.value;
        if (value > 0) {
            income += value;
        } else {
            expense -= value;
        }
    }
    return std::make_pair(income, expense);
}
int LogManager::addOperationLog(long long timestamp, User::USERID_T userid, int privilege,
                                Log::OPERATION_T op) {
    int log_count;
    operation_log.getInfo(log_count, 1);
    OperationLogEntry entry{timestamp, userid, privilege, op, 0};
    ++log_count;
    operation_log.writeInfo(log_count, 1);

    int id = operation_log.write(entry);
    if (util::toString(userid) != "<GUEST>") user_index.insert(userid, id);
    return id;
}
void LogManager::markOperationSuccess(int id) {
    OperationLogEntry entry;
    operation_log.read(entry, id);
    entry.is_success = 1;
    operation_log.update(entry, id);
}
int LogManager::getFinanceLogCount() {
    int count = 0;
    finance_log.getInfo(count, 1);
    return count;
}
FinanceLogEntry LogManager::getFinanceLogEntry(int id) {
    FinanceLogEntry entry;
    finance_log.read(entry, id);
    return entry;
}
int LogManager::getOperationLogCount() {
    int count = 0;
    operation_log.getInfo(count, 1);
    return count;
}
OperationLogEntry LogManager::getOperationLogEntry(int id) {
    OperationLogEntry entry;
    operation_log.read(entry, id);
    return entry;
}
std::vector<std::pair<User::USERID_T, int>> LogManager::getUserIdPairs() {
    return user_index.queryAllKeyValuePairs();
}
LogManager::LogManager() {
    finance_log.initialise("log_finance");
    operation_log.initialise("log_operation");
    user_index.initialise("log_user_index");
}