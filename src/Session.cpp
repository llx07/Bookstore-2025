#include "Session.hpp"

BooksManager& Session::books_manager = BooksManager::getInstance();
UsersManager& Session::users_manager = UsersManager::getInstance();

Session::Session(std::ostream& os) : out_stream(os) {}

int Session::get_privilege() {
    if (login_stack.empty()) return 0;
    return users_manager.get_user_by_userid(login_stack.top().userid).privilege;
}
void Session::login_push(const User::USERID_T& userid) {
    users_manager.modify_login_count(userid, 1);
    login_stack.emplace(userid, 0);
}

void Session::login_pop() {
    assert(!login_stack.empty());
    users_manager.modify_login_count(login_stack.top().userid, -1);
    login_stack.pop();
}
bool Session::login_empty() const { return login_stack.empty(); }
void Session::set_selected_book(int bookID) {
    assert(!login_stack.empty());
    login_stack.top().book_selected = bookID;
}
int Session::get_selected_book() {
    assert(!login_stack.empty());
    return login_stack.top().book_selected;
}
bool Session::is_logged_in(const User::USERID_T& userid) {
    return users_manager.get_login_count(userid) > 0;
}