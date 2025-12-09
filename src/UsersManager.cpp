#include "UsersManager.hpp"

#include "Utils.hpp"

UsersManager& UsersManager::getInstance() {
    static UsersManager instance;
    return instance;
}
int UsersManager::getLoginCount(const User::USERID_T& userid) {
    return getUserByUserid(userid).login_count;
}
void UsersManager::modifyLoginCount(const User::USERID_T& userid, int k) {
    // XXX(llx) space of performance improvement here.
    User user = getUserByUserid(userid);
    user.login_count += k;
    eraseUser(userid);
    addUser(user);
}
bool UsersManager::useridExists(const User::USERID_T& userid) {
    return !user_data.query(userid).empty();
}
User UsersManager::getUserByUserid(const User::USERID_T& userid) {
    auto result = user_data.query(userid);
    assert(!result.empty());
    return result[0];
}
bool UsersManager::isPasswordCorrect(const User::USERID_T& userid,
                                     const User::PASSWORD_T& password) {
    return getUserByUserid(userid).password == password;
}
void UsersManager::modifyPassword(const User::USERID_T& userid,
                                  const User::PASSWORD_T& new_password) {
    User user = getUserByUserid(userid);
    user.password = new_password;
    eraseUser(userid);
    addUser(user);
}
void UsersManager::addUser(const User& user) { user_data.insert(user.userid, user); }
void UsersManager::eraseUser(const User::USERID_T& userid) {
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
    if (user_data.query(util::toArray<User::USERID_T>("root")).empty()) {
        User root_user;
        root_user.userid = util::toArray<User::USERID_T>("root");
        root_user.username = util::toArray<User::USERID_T>("root");
        root_user.password = util::toArray<User::PASSWORD_T>("sjtu");
        root_user.privilege = 7;
        addUser(root_user);
    }
}