#ifndef BOOKSTORE_LOGCOMMANDS_HPP
#define BOOKSTORE_LOGCOMMANDS_HPP
#include <optional>

#include "Commands/CommandBase.hpp"
#include "LogManager.hpp"

class ShowFinanceCommand : public Command {
public:
    void execute(Session& session) override;
    ShowFinanceCommand() = default;
    explicit ShowFinanceCommand(int _count);

private:
    std::optional<int> count;
};
#endif  // BOOKSTORE_LOGCOMMANDS_HPP
