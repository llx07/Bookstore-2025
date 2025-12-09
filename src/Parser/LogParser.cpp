#include <string>

#include "Charset.hpp"
#include "Commands/LogCommands.hpp"
#include "Parser/Parser.hpp"
#include "Utils.hpp"
#include "Validator.hpp"

static int parseCount(const std::string& token) {
    expect(token).consistedOf(NUMERIC);
    int value = util::toInt(token);
    expect(value).ge(1).le(std::numeric_limits<int>::max());
    return value;
}
static auto handleSHOW_FINANCE(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() < 2 || tokens[1] != "finance") {
        throw ExecutionException("show finance error.");
    }
    if (tokens.size() == 2) {
        return std::make_unique<ShowFinanceCommand>();
    }
    if (tokens.size() == 3) {
        return std::make_unique<ShowFinanceCommand>(parseCount(tokens[2]));
    }
    throw ExecutionException("show finance error.");
}

REGISTER_HANDLER(show, handleSHOW_FINANCE);