#include <string>

#include "Charset.hpp"
#include "Commands/LogCommands.hpp"
#include "Parser/FieldParser.hpp"
#include "Parser/Parser.hpp"
#include "Utils.hpp"
#include "Validator.hpp"

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
static auto handleREPORT_FINANCE(const std::vector<std::string>& tokens)
    -> std::unique_ptr<Command> {
    if (tokens.size() < 2 || tokens[1] != "finance") {
        throw ExecutionException("report finance error.");
    }
    if (tokens.size() == 2) {
        return std::make_unique<ReportFinanceCommand>();
    }
    throw ExecutionException("report finance error.");
}
static auto handleREPORT_EMPLOYEE(const std::vector<std::string>& tokens)
    -> std::unique_ptr<Command> {
    if (tokens.size() < 2 || tokens[1] != "employee") {
        throw ExecutionException("report employee error.");
    }
    if (tokens.size() == 2) {
        return std::make_unique<ReportEmployeeCommand>();
    }
    throw ExecutionException("report employee error.");
}
static auto handleLOG(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 1) {
        return std::make_unique<LogCommand>();
    }
    throw ExecutionException("log error.");
}

REGISTER_HANDLER(show, handleSHOW_FINANCE);
REGISTER_HANDLER(report, handleREPORT_FINANCE);
REGISTER_HANDLER(report, handleREPORT_EMPLOYEE);
REGISTER_HANDLER(log, handleLOG);