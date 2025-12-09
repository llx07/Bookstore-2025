#ifndef BOOKSTORE_SESSION_HPP
#define BOOKSTORE_SESSION_HPP

#include <iostream>
#include <stack>

#include "BooksManager.hpp"
#include "LogManager.hpp"
#include "UsersManager.hpp"

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
    bool loginEmpty() const;
    void setSelectedBook(int bookID);
    int getSelectedBook();
    static bool isLoggedIn(const User::USERID_T& userid);
    ~Session();

private:
    struct LoginState {
        User::USERID_T userid;  // XXX(llx) maybe we should store integer id here?
        int book_selected;
    };
    std::stack<LoginState> login_stack;
};

#endif  // BOOKSTORE_SESSION_HPP
