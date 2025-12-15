#include "Commands/LogCommands.hpp"

#include <tuple>

#include "Utils.hpp"
void ShowFinanceCommand::execute(User::USERID_T& current_userid, int& current_bookid,
                                 std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 7) {
        throw ExecutionException("show finance error: privilege not enough to operate.");
    }
    if (count.has_value() && count == 0) {
        os << "\n";
        return;
    }

    long long income = 0, expense = 0;
    std::tie(income, expense) = log_mgr.getFinanceValue(count.value_or(0));

    os << "+ ";
    util::outputDecimal(os, income);
    os << " - ";
    util::outputDecimal(os, expense);
    os << "\n";
}
ShowFinanceCommand::ShowFinanceCommand(int _count) : count(_count) {}
void ReportFinanceCommand::execute(User::USERID_T& current_userid, int& current_bookid,
                                   std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 7) {
        throw ExecutionException("report finance error: privilege not enough to operate.");
    }
    std::vector<int> length{20, 30, 20};
    os << "Finance Report:\n";
    util::printTableHead(os, length);
    util::printTableBody(os, length, {"Time", "UserID", "Finance Change"});
    util::printTableMiddle(os, length);
    int count = log_mgr.getFinanceLogCount();
    for (int i = 1; i <= count; i++) {
        FinanceLogEntry entry = log_mgr.getFinanceLogEntry(i);
        std::string time_str = util::timestampToString(entry.timestamp);
        std::string userid_str = util::toString(entry.userid);
        std::string value =
            (entry.value > 0 ? "+ " : "- ") + util::decimalToString(std::abs(entry.value));
        util::printTableBody(os, length, {time_str, userid_str, value});
    }
    util::printTableBottom(os, length);
}
void ReportEmployeeCommand::execute(User::USERID_T& current_userid, int& current_bookid,
                                    std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 7) {
        throw ExecutionException("report employee error: privilege not enough to operate.");
    }

    const std::vector<int> length{20, 10, 40, 6};
    auto pairs = log_mgr.getUserIdPairs();
    for (int l = 0, r; l < pairs.size(); l = r + 1) {
        for (r = l; r + 1 < pairs.size() && pairs[r + 1].first == pairs[l].first; r = r + 1);
        // std::cerr << "userid = " << util::toString(pairs[l].first) << "\n";
        OperationLogEntry entry_first = log_mgr.getOperationLogEntry(pairs[l].second);
        if (entry_first.user_privilege < 3) continue;
        os << "Employee Report of:" << util::toString(pairs[l].first) << "\n";

        util::printTableHead(os, length);
        util::printTableBody(os, length, {"Time", "UserID", "Operation", "Status"});
        util::printTableMiddle(os, length);
        for (int i = l; i <= r; i++) {
            OperationLogEntry entry = log_mgr.getOperationLogEntry(pairs[i].second);
            std::string time_str = util::timestampToString(entry.timestamp);
            std::string userid_str = util::toString(entry.userid);
            std::string operation_str = util::toString(entry.op);
            std::string status_str = (entry.is_success) ? "Ok" : "Failed";
            util::printTableBody(os, length, {time_str, userid_str, operation_str, status_str});
        }
        util::printTableBottom(os, length);
        os << "\n";
    }
}
void LogCommand::execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 7) {
        throw ExecutionException("log error: privilege not enough to operate.");
    }

    std::vector<int> length{20, 10, 40, 6};
    os << "Operation Log:\n";
    util::printTableHead(os, length);
    util::printTableBody(os, length, {"Time", "UserID", "Operation", "Status"});
    util::printTableMiddle(os, length);
    int count = log_mgr.getOperationLogCount();
    for (int i = 1; i <= count; i++) {
        OperationLogEntry entry = log_mgr.getOperationLogEntry(i);
        std::string time_str = util::timestampToString(entry.timestamp);
        std::string userid_str = util::toString(entry.userid);
        std::string operation_str = util::toString(entry.op);
        std::string status_str = (entry.is_success) ? "Ok" : "Failed";
        util::printTableBody(os, length, {time_str, userid_str, operation_str, status_str});
    }
    util::printTableBottom(os, length);
}