#ifndef BOOKSTORE_USERSMANAGER_HPP
#define BOOKSTORE_USERSMANAGER_HPP

#include <array>

#include "BlockList.hpp"

// Structure for user
struct User {
    static constexpr int USERID_MAX_LEN = 30;
    static constexpr int PASSWORD_MAX_LEN = 30;
    static constexpr int USERNAME_MAX_LEN = 30;
    // One additional character for storing the \0.
    typedef std::array<char, USERID_MAX_LEN + 1> USERID_T;
    typedef std::array<char, USERNAME_MAX_LEN + 1> USERNAME_T;
    typedef std::array<char, PASSWORD_MAX_LEN + 1> PASSWORD_T;

    USERID_T userid{};
    USERNAME_T username{};
    PASSWORD_T password{};
    int privilege;
    int login_count;

    User() : privilege(0), login_count(false) {
        userid.fill(0);
        username.fill(0);
        password.fill(0);
    }
    auto operator<=>(const User&) const = default;
};

// A Singleton class for manipulating the data of users.
class UsersManager {
public:
    // Returns the singleton for UsersManager.
    static UsersManager& getInstance();
    // Disables copy and assignment for singleton class.
    UsersManager(const UsersManager&) = delete;
    UsersManager& operator=(const UsersManager&) = delete;

    int get_login_count(const User::USERID_T& userid);
    // Modifies login count by k.
    void modify_login_count(const User::USERID_T& userid, int k);

    // Returns true if user with userid exists
    bool userid_exists(const User::USERID_T& userid);
    // Returns the user with userid.
    User get_user_by_userid(const User::USERID_T& userid);
    // Returns true if the password is correct for userid.
    bool is_password_correct(const User::USERID_T& userid, const User::PASSWORD_T& password);
    // Modifies the password of userid to new_password.
    void modify_password(const User::USERID_T& userid, const User::PASSWORD_T& new_password);
    // Adds a new user.
    void add_user(const User& user);
    // Erases the user with userid given.
    void erase_user(const User::USERID_T& userid);

    // Hack function for testing.
    void reset();

private:
    // XXX(llx) maybe we can use int to save in block list?
    BlockList<User::USERID_T, User> user_data;
    UsersManager();
    ~UsersManager() = default;
};

#endif  // BOOKSTORE_USERSMANAGER_HPP
