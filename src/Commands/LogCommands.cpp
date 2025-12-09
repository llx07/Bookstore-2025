#include "Commands/LogCommands.hpp"

#include <tuple>

#include "Utils.hpp"
void ShowFinanceCommand::execute(Session& session) {
    if (count.has_value() && count == 0) {
        session.out_stream << "\n";
        return;
    }

    long long income = 0, expense = 0;
    std::tie(income, expense) = session.log_manager.get_finance_log(count.value_or(0));

    session.out_stream << "+ ";
    util::outputDecimal(session.out_stream, income);
    session.out_stream << " - ";
    util::outputDecimal(session.out_stream, expense);
    session.out_stream << "\n";
}
ShowFinanceCommand::ShowFinanceCommand(int _count) : count(_count) {}