#include "LogManager.hpp"

#include <cmath>
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
std::pair<long long, long long> LogManager::getFinanceLog(int cnt) {
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
LogManager::LogManager() { finance_log.initialise("log_finance"); }