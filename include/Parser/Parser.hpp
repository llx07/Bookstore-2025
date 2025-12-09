#ifndef BOOKSTORE_PARSER_HPP
#define BOOKSTORE_PARSER_HPP

#include <functional>
#include <map>
#include <memory>

#include "../Commands/BookCommands.hpp"
#include "../Commands/CommandBase.hpp"

class ParseException : public std::exception {
public:
    explicit ParseException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }

private:
    std::string message;
};

using HANDLER_T = std::function<std::unique_ptr<Command>(const std::vector<std::string>&)>;

class HandlerRegistry {
public:
    static HandlerRegistry& get_instance() {
        static HandlerRegistry instance;
        return instance;
    }
    // Register Parser with command_name
    HandlerRegistry(const std::string& command_name, HANDLER_T func);
    ~HandlerRegistry() = default;
    // Returns a handler, or nullptr if no suitable handler found
    std::vector<HANDLER_T> get_handler(const std::string& command_name);

private:
    std::multimap<std::string, HANDLER_T> parsers;
    HandlerRegistry() = default;
};

std::unique_ptr<Command> parse_command(const std::vector<std::string>& tokens);

#define REGISTER_HANDLER(command_name, func) \
    static HandlerRegistry __registry_##command_name(#command_name, func)

#endif  // BOOKSTORE_PARSER_HPP
