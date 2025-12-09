#ifndef BOOKSTORE_COMMANDS_HPP
#define BOOKSTORE_COMMANDS_HPP

#include <exception>
#include <string>

#include "Session.hpp"

class ExecutionException : public std::exception {
public:
    explicit ExecutionException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }

private:
    std::string message;
};

class Command {
public:
    // May throws ExecutionException
    virtual void execute(Session& session) = 0;
    virtual ~Command() = default;
};
#endif  // BOOKSTORE_COMMANDS_HPP
