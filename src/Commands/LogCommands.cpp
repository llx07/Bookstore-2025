#include "Commands/LogCommands.hpp"

#include <tuple>

#include "Utils.hpp"
void ShowFinanceCommand::execute(Session& session) {
    if (session.getPrivilege() < 7) {
        throw ExecutionException("show finance error: privilege not enough to operate.");
    }
    if (count.has_value() && count == 0) {
        session.out_stream << "\n";
        return;
    }

    long long income = 0, expense = 0;
    std::tie(income, expense) = session.log_manager.getFinanceValue(count.value_or(0));

    session.out_stream << "+ ";
    util::outputDecimal(session.out_stream, income);
    session.out_stream << " - ";
    util::outputDecimal(session.out_stream, expense);
    session.out_stream << "\n";
}
ShowFinanceCommand::ShowFinanceCommand(int _count) : count(_count) {}
void ReportFinanceCommand::execute(Session& session) {
    if (session.getPrivilege() < 7) {
        throw ExecutionException("report finance error: privilege not enough to operate.");
    }
    std::vector<int> length{20, 30, 20};
    session.out_stream << "Finance Report:\n";
    util::printTableHead(session.out_stream, length);
    util::printTableBody(session.out_stream, length, {"Time", "UserID", "Finance Change"});
    util::printTableMiddle(session.out_stream, length);
    int count = session.log_manager.getFinanceLogCount();
    for (int i = 1; i <= count; i++) {
        FinanceLogEntry entry = session.log_manager.getFinanceLogEntry(i);
        std::string time_str = util::timestampToString(entry.timestamp);
        std::string userid_str = util::toString(entry.userid);
        std::string value =
            (entry.value > 0 ? "+ " : "- ") + util::decimalToString(std::abs(entry.value));
        util::printTableBody(session.out_stream, length, {time_str, userid_str, value});
    }
    util::printTableBottom(session.out_stream, length);
}
void ReportEmployeeCommand::execute(Session& session) {
    if (session.getPrivilege() < 7) {
        throw ExecutionException("report employee error: privilege not enough to operate.");
    }

    const std::vector<int> length{20, 10, 40, 6};
    auto pairs = session.log_manager.getUserIdPairs();
    for (int l = 0, r; l < pairs.size(); l = r + 1) {
        for (r = l; r + 1 < pairs.size() && pairs[r + 1].first == pairs[l].first; r = r + 1);
        // std::cerr << "userid = " << util::toString(pairs[l].first) << "\n";
        OperationLogEntry entry_first = session.log_manager.getOperationLogEntry(pairs[l].second);
        if (entry_first.user_privilege < 3) continue;
        session.out_stream << "Employee Report of:" << util::toString(pairs[l].first) << "\n";

        util::printTableHead(session.out_stream, length);
        util::printTableBody(session.out_stream, length, {"Time", "UserID", "Operation", "Status"});
        util::printTableMiddle(session.out_stream, length);
        for (int i = l; i <= r; i++) {
            OperationLogEntry entry = session.log_manager.getOperationLogEntry(pairs[i].second);
            std::string time_str = util::timestampToString(entry.timestamp);
            std::string userid_str = util::toString(entry.userid);
            std::string operation_str = util::toString(entry.op);
            std::string status_str = (entry.is_success) ? "Ok" : "Failed";
            util::printTableBody(session.out_stream, length,
                                 {time_str, userid_str, operation_str, status_str});
        }
        util::printTableBottom(session.out_stream, length);
        session.out_stream << "\n";
    }
}
void LogCommand::execute(Session& session) {
    if (session.getPrivilege() < 7) {
        throw ExecutionException("log error: privilege not enough to operate.");
    }

    std::vector<int> length{20, 10, 40, 6};
    session.out_stream << "Operation Log:\n";
    util::printTableHead(session.out_stream, length);
    util::printTableBody(session.out_stream, length, {"Time", "UserID", "Operation", "Status"});
    util::printTableMiddle(session.out_stream, length);
    int count = session.log_manager.getOperationLogCount();
    for (int i = 1; i <= count; i++) {
        OperationLogEntry entry = session.log_manager.getOperationLogEntry(i);
        std::string time_str = util::timestampToString(entry.timestamp);
        std::string userid_str = util::toString(entry.userid);
        std::string operation_str = util::toString(entry.op);
        std::string status_str = (entry.is_success) ? "Ok" : "Failed";
        util::printTableBody(session.out_stream, length,
                             {time_str, userid_str, operation_str, status_str});
    }
    util::printTableBottom(session.out_stream, length);
}