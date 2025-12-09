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
    std::tie(income, expense) = session.log_manager.getFinanceLog(count.value_or(0));

    session.out_stream << "+ ";
    util::outputDecimal(session.out_stream, income);
    session.out_stream << " - ";
    util::outputDecimal(session.out_stream, expense);
    session.out_stream << "\n";
}
ShowFinanceCommand::ShowFinanceCommand(int _count) : count(_count) {}