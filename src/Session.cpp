#include "Session.hpp"

BooksManager& Session::books_manager = BooksManager::getInstance();
UsersManager& Session::users_manager = UsersManager::getInstance();
LogManager& Session::log_manager = LogManager::getInstance();

Session::Session(std::ostream& os) : out_stream(os) {}

int Session::getPrivilege() {
    if (login_stack.empty()) return 0;
    return users_manager.getUserByUserid(login_stack.top().userid).privilege;
}
void Session::loginPush(const User::USERID_T& userid) {
    users_manager.modifyLoginCount(userid, 1);
    login_stack.emplace(userid, 0);
}

void Session::loginPop() {
    assert(!login_stack.empty());
    users_manager.modifyLoginCount(login_stack.top().userid, -1);
    login_stack.pop();
}
bool Session::isLoginEmpty() const { return login_stack.empty(); }
void Session::setSelectedBook(int bookID) {
    assert(!login_stack.empty());
    login_stack.top().book_selected = bookID;
}
int Session::getSelectedBook() {
    assert(!login_stack.empty());
    return login_stack.top().book_selected;
}
bool Session::isLoggedIn(const User::USERID_T& userid) {
    return users_manager.getLoginCount(userid) > 0;
}
Session::~Session() {
    while (!login_stack.empty()) {
        loginPop();
    }
}
long long Session::getTimestamp() const { return timestamp; }
void Session::updateTimestamp() {
    auto now = std::chrono::system_clock::now();
    timestamp =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}