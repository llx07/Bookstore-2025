#include "UsersManager.hpp"

#include "Utils.hpp"

UsersManager& UsersManager::getInstance() {
    static UsersManager instance;
    return instance;
}
int UsersManager::get_login_count(const User::USERID_T& userid) {
    return get_user_by_userid(userid).login_count;
}
void UsersManager::modify_login_count(const User::USERID_T& userid, int k) {
    // XXX(llx) space of performance improvement here.
    User user = get_user_by_userid(userid);
    user.login_count += k;
    erase_user(userid);
    add_user(user);
}
bool UsersManager::userid_exists(const User::USERID_T& userid) {
    return !user_data.query(userid).empty();
}
User UsersManager::get_user_by_userid(const User::USERID_T& userid) {
    auto result = user_data.query(userid);
    assert(!result.empty());
    return result[0];
}
bool UsersManager::is_password_correct(const User::USERID_T& userid,
                                       const User::PASSWORD_T& password) {
    return get_user_by_userid(userid).password == password;
}
void UsersManager::modify_password(const User::USERID_T& userid,
                                   const User::PASSWORD_T& new_password) {
    User user = get_user_by_userid(userid);
    user.password = new_password;
    erase_user(userid);
    add_user(user);
}
void UsersManager::add_user(const User& user) { user_data.insert(user.userid, user); }
void UsersManager::erase_user(const User::USERID_T& userid) {
    auto result = user_data.query(userid);
    assert(result.size() <= 1);
    if (!result.empty()) {
        user_data.erase(userid, result[0]);
    }
}
void UsersManager::reset() {
    this->~UsersManager();
    std::filesystem::remove("user_data");
    new (this) UsersManager();
}

UsersManager::UsersManager() {
    user_data.initialise("user_data");
    if (user_data.query(util::to_array<User::USERID_T>("root")).empty()) {
        User root_user;
        root_user.userid = util::to_array<User::USERID_T>("root");
        root_user.username = util::to_array<User::USERID_T>("root");
        root_user.password = util::to_array<User::PASSWORD_T>("sjtu");
        root_user.privilege = 7;
        add_user(root_user);
    }
}