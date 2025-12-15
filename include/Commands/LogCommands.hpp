#ifndef BOOKSTORE_LOGCOMMANDS_HPP
#define BOOKSTORE_LOGCOMMANDS_HPP
#include <optional>

#include "Commands/CommandBase.hpp"
#include "LogManager.hpp"

class ShowFinanceCommand : public Command {
public:
    void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) override;
    ShowFinanceCommand() = default;
    explicit ShowFinanceCommand(int _count);

private:
    std::optional<int> count;
};

class ReportFinanceCommand : public Command {
public:
    void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) override;
    ReportFinanceCommand() = default;
};
class ReportEmployeeCommand : public Command {
public:
    void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) override;
    ReportEmployeeCommand() = default;
};
class LogCommand : public Command {
public:
    void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) override;
    LogCommand() = default;
};
#endif  // BOOKSTORE_LOGCOMMANDS_HPP
