#ifndef BOOKSTORE_COMMANDS_HPP
#define BOOKSTORE_COMMANDS_HPP

#include <exception>
#include <string>

#include "BooksManager.hpp"
#include "LogManager.hpp"
#include "UsersManager.hpp"

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
    virtual void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) = 0;
    virtual ~Command() = default;

protected:
    static UsersManager& usr_mgr;
    static BooksManager& bk_mgr;
    static LogManager& log_mgr;
};
#endif  // BOOKSTORE_COMMANDS_HPP
