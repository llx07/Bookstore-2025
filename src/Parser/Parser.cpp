#include "Parser/Parser.hpp"

#include <utility>

#include "Charset.hpp"
#include "Commands/UserCommands.hpp"
#include "Validator.hpp"

HandlerRegistry::HandlerRegistry(const std::string& command_name, HANDLER_T func) {
    get_instance().parsers.emplace(command_name, func);
}
std::vector<HANDLER_T> HandlerRegistry::get_handler(const std::string& command_name) {
    if (parsers.find(command_name) == parsers.end()) {
        return {};
    }
    std::vector<HANDLER_T> handlers;
    auto range = parsers.equal_range(command_name);
    for (auto it = range.first; it != range.second; it++) {
        handlers.push_back(it->second);
    }
    return handlers;
}

std::unique_ptr<Command> parse_command(const std::vector<std::string>& tokens) {
    if (tokens.empty()) {
        throw ParseException("Empty input");
    }
    // TODO(llx) add parser for log commands
    const auto handlers = HandlerRegistry::get_instance().get_handler(tokens[0]);
    // std::cerr << "found " << handlers.size() << " handlers\n";
    for (const auto& handler : handlers) {
        try {
            auto command = handler(tokens);
            return command;
        } catch (std::exception& e) {
            // ignore the error, and try to use next handler
            // std::cerr << "From parse_command:" << e.what() << std::endl;
        }
    }
    throw ParseException("Invalid Command");
}