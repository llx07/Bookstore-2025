#ifndef BOOKSTORE_SESSION_HPP
#define BOOKSTORE_SESSION_HPP

#include <iostream>
#include <stack>

#include "BooksManager.hpp"
#include "UsersManager.hpp"

class Session {
public:
    static BooksManager& books_manager;
    static UsersManager& users_manager;
    std::ostream& out_stream;
    explicit Session(std::ostream& os = std::cout);
    int get_privilege();
    void login_push(const User::USERID_T& userid);
    void login_pop();
    void select_book(int bookID);
    static bool is_loging_in(const User::USERID_T& userid);

private:
    struct LoginState {
        User::USERID_T userid;  // XXX(llx) maybe we should store integer id here?
        int book_selected;
    };
    std::stack<LoginState> login_stack;
};

#endif  // BOOKSTORE_SESSION_HPP
