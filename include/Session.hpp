#ifndef BOOKSTORE_SESSION_HPP
#define BOOKSTORE_SESSION_HPP

#include <charconv>
#include <iostream>
#include <stack>

#include "BooksManager.hpp"
#include "LogManager.hpp"
#include "UsersManager.hpp"
#include "Utils.hpp"

class Session {
public:
    static BooksManager& books_manager;
    static UsersManager& users_manager;
    static LogManager& log_manager;
    std::ostream& out_stream;
    explicit Session(std::ostream& os = std::cout);
    int getPrivilege();
    void loginPush(const User::USERID_T& userid);
    void loginPop();
    bool isLoginEmpty() const;
    void setSelectedBook(int bookID);
    int getSelectedBook();
    User::USERID_T getCurrentUser() {
        if (isLoginEmpty()) {
            return util::toArray<User::USERID_T>("<GUEST>");
        }
        return login_stack.top().userid;
    }
    static bool isLoggedIn(const User::USERID_T& userid);
    ~Session();

    long long getTimestamp() const;
    void updateTimestamp();

private:
    struct LoginState {
        User::USERID_T userid;  // XXX(llx) maybe we should store integer id here?
        int book_selected;
    };
    std::stack<LoginState> login_stack;
    long long timestamp;
};

#endif  // BOOKSTORE_SESSION_HPP
