#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <random>
#include <string>

#include "UsersManager.hpp"
#include "Utils.hpp"

User gen_user(const std::string& userid, const std::string& username, const std::string& password,
              int privilege) {
    User user;
    user.userid = util::toArray<User::USERID_T>(userid);
    user.username = util::toArray<User::USERNAME_T>(username);
    user.password = util::toArray<User::PASSWORD_T>(password);
    user.privilege = privilege;
    return user;
}

TEST_CASE("UsersManager Basic Usage", "[UsersManager]") {
    UsersManager& manager = UsersManager::getInstance();
    manager.reset();

    SECTION("Automatically creates root account") {
        REQUIRE(manager.useridExists(util::toArray<User::USERID_T>("root")));
    }

    SECTION("Add new user") {
        User user = gen_user("myuserid", "myusername", "123456", 1);
        manager.addUser(user);
        REQUIRE(manager.useridExists(util::toArray<User::USERID_T>("myuserid")));
    }

    SECTION("Check & Modify Password") {
        User user = gen_user("myuserid", "myusername", "123456", 1);
        manager.addUser(user);
        REQUIRE(manager.isPasswordCorrect(util::toArray<User::USERID_T>("myuserid"),
                                          util::toArray<User::PASSWORD_T>("123456")));
        REQUIRE(!manager.isPasswordCorrect(util::toArray<User::USERID_T>("myuserid"),
                                           util::toArray<User::PASSWORD_T>("654321")));
        manager.modifyPassword(util::toArray<User::USERID_T>("myuserid"),
                               util::toArray<User::PASSWORD_T>("genshin_impact"));
        REQUIRE(!manager.isPasswordCorrect(util::toArray<User::USERID_T>("myuserid"),
                                           util::toArray<User::PASSWORD_T>("123456")));
        REQUIRE(manager.isPasswordCorrect(util::toArray<User::USERID_T>("myuserid"),
                                          util::toArray<User::PASSWORD_T>("genshin_impact")));
    }

    SECTION("Erase User") {
        User user = gen_user("myuserid", "myusername", "123456", 1);
        manager.addUser(user);
        REQUIRE(manager.useridExists(util::toArray<User::USERID_T>("myuserid")));
        manager.eraseUser(util::toArray<User::USERID_T>("myuserid"));
        REQUIRE(!manager.useridExists(util::toArray<User::USERID_T>("myuserid")));
    }

    SECTION("Gets User") {
        User user = gen_user("myuserid", "1+1", "2", 1);
        manager.addUser(user);
        auto result = manager.getUserByUserid(util::toArray<User::USERID_T>("myuserid"));
        assert(user == result);
    }
}