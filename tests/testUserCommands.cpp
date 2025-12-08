#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "BooksManager.hpp"
#include "Commands/UserCommands.hpp"
#include "UsersManager.hpp"
#include "Utils.hpp"

void setup() {
    BooksManager::getInstance().reset();
    UsersManager::getInstance().reset();

    User user;
    user.userid = util::to_array<User::USERID_T>("customer");
    user.password = util::to_array<User::PASSWORD_T>("123");
    user.privilege = 1;
    UsersManager::getInstance().add_user(user);

    user.userid = util::to_array<User::USERID_T>("salesman");
    user.password = util::to_array<User::PASSWORD_T>("456");
    user.privilege = 3;
    UsersManager::getInstance().add_user(user);
}

TEST_CASE("UserCommands: SU command", "[UserCommands]") {
    setup();
    Session session;

    SECTION("Login with correct password") {
        SwitchUserCommand cmd(util::to_array<User::USERID_T>("customer"),
                              util::to_array<User::PASSWORD_T>("123"));
        REQUIRE_NOTHROW(cmd.execute(session));
        REQUIRE(session.is_loging_in(util::to_array<User::USERID_T>("customer")));
        REQUIRE(session.get_privilege() == 1);
    }

    SECTION("Login with wrong password") {
        SwitchUserCommand cmd(util::to_array<User::USERID_T>("customer"),
                              util::to_array<User::PASSWORD_T>("114514"));
        REQUIRE_THROWS_AS(cmd.execute(session), ExecutionException);
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("password incorrect"));
    }

    SECTION("Login to non-existent user") {
        SwitchUserCommand cmd(util::to_array<User::USERID_T>("nobody"),
                              util::to_array<User::PASSWORD_T>("114514"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("userID don't exists"));
    }

    SECTION("Su without password - privilege less than") {
        SwitchUserCommand cmd(util::to_array<User::USERID_T>("customer"));
        REQUIRE_THROWS_WITH(cmd.execute(session), Catch::Matchers::ContainsSubstring(
                                                      "privilege not enough to omit password"));
    }

    SECTION("Su without password - privilege higher than") {
        session.login_push(util::to_array<User::USERID_T>("root"));
        REQUIRE(session.get_privilege() == 7);
        SwitchUserCommand cmd(util::to_array<User::USERID_T>("customer"));
        REQUIRE_NOTHROW(cmd.execute(session));
        REQUIRE(session.is_loging_in(util::to_array<User::USERID_T>("customer")));
        REQUIRE(session.get_privilege() == 1);
    }
    SECTION("Su without password - privilege higher than 2") {
        session.login_push(util::to_array<User::USERID_T>("salesman"));
        REQUIRE(session.get_privilege() == 3);
        SwitchUserCommand cmd(util::to_array<User::USERID_T>("customer"));
        REQUIRE_NOTHROW(cmd.execute(session));
        REQUIRE(session.is_loging_in(util::to_array<User::USERID_T>("customer")));
        REQUIRE(session.get_privilege() == 1);
    }
    SECTION("Su without password - same privilege") {
        session.login_push(util::to_array<User::USERID_T>("customer"));
        REQUIRE(session.get_privilege() == 1);
        SwitchUserCommand cmd(util::to_array<User::USERID_T>("customer"));
        REQUIRE_THROWS_WITH(cmd.execute(session), Catch::Matchers::ContainsSubstring(
                                                      "privilege not enough to omit password"));
        REQUIRE(session.get_privilege() == 1);
    }
}

TEST_CASE("UserCommands: LOGOUT command", "[UserCommands]") {
    setup();
    Session session;
    SECTION("Correct Usage") {
        LogoutCommand cmd;
        session.login_push(util::to_array<User::USERID_T>("root"));
        REQUIRE_NOTHROW(cmd.execute(session));
    }
    SECTION("Logout when not logged in") {
        LogoutCommand cmd;
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("privilege not enough to operate"));
    }
}
TEST_CASE("UserCommands: REGISTER command", "[UserCommands]") {
    setup();
    Session session;
    SECTION("Success register") {
        RegisterCommand cmd(util::to_array<User::USERID_T>("newuser"),
                            util::to_array<User::PASSWORD_T>("password"),
                            util::to_array<User::USERID_T>("mycustomer"));
        REQUIRE_NOTHROW(cmd.execute(session));
        REQUIRE(session.users_manager.userid_exists(util::to_array<User::USERID_T>("newuser")));
    }

    SECTION("Register duplicate ID") {
        RegisterCommand cmd(util::to_array<User::USERID_T>("root"),
                            util::to_array<User::PASSWORD_T>("sjtu"),
                            util::to_array<User::USERID_T>("Hacker"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("userid already exists"));
    }
}
TEST_CASE("UserCommands: PASSWD command", "[UserCommands]") {
    setup();
    Session session;
    SECTION("Guest cannot change password") {
        ChangePasswordCommand cmd(util::to_array<User::USERID_T>("root"),
                                  util::to_array<User::PASSWORD_T>("sjtu"),
                                  util::to_array<User::PASSWORD_T>("123456"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("privilege not enough to operate"));
    }
    SECTION("Userid not exist") {
        session.login_push(util::to_array<User::USERID_T>("customer"));
        ChangePasswordCommand cmd(util::to_array<User::USERID_T>("strange_user"),
                                  util::to_array<User::PASSWORD_T>("654321"),
                                  util::to_array<User::PASSWORD_T>("123456"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("userid doesn't exist"));
    }
    SECTION("No password - privilege not enough") {
        session.login_push(util::to_array<User::USERID_T>("customer"));
        ChangePasswordCommand cmd(util::to_array<User::USERID_T>("customer"),
                                  util::to_array<User::PASSWORD_T>("123456"));
        REQUIRE_THROWS_WITH(
            cmd.execute(session),
            Catch::Matchers::ContainsSubstring("privilege not enough to omit current password"));
    }
    SECTION("No password - privilege enough") {
        session.login_push(util::to_array<User::USERID_T>("root"));
        ChangePasswordCommand cmd(util::to_array<User::USERID_T>("customer"),
                                  util::to_array<User::PASSWORD_T>("123456"));
        REQUIRE_NOTHROW(cmd.execute(session));
    }
    SECTION("Incorrect Password") {
        session.login_push(util::to_array<User::USERID_T>("customer"));
        ChangePasswordCommand cmd(util::to_array<User::USERID_T>("customer"),
                                  util::to_array<User::PASSWORD_T>("456"),
                                  util::to_array<User::PASSWORD_T>("123"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("current password is incorrect"));
    }
    SECTION("Correctly changes password") {
        session.login_push(util::to_array<User::USERID_T>("customer"));
        ChangePasswordCommand cmd(util::to_array<User::USERID_T>("customer"),
                                  util::to_array<User::PASSWORD_T>("123"),
                                  util::to_array<User::PASSWORD_T>("456"));
        REQUIRE_NOTHROW(cmd.execute(session));
    }
}
TEST_CASE("UserCommands: USERADD", "[UserCommands]") {
    setup();
    Session session;
    SECTION("Privilege is at least 3") {
        session.login_push(util::to_array<User::USERID_T>("customer"));
        AddUserCommand cmd(util::to_array<User::USERID_T>("newuser"),
                           util::to_array<User::PASSWORD_T>("456"), 3,
                           util::to_array<User::USERNAME_T>("123"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("privilege not enough to operate"));
    }
    SECTION("Cannot add user with same id") {
        session.login_push(util::to_array<User::USERID_T>("salesman"));
        AddUserCommand cmd(util::to_array<User::USERID_T>("customer"),
                           util::to_array<User::PASSWORD_T>("456"), 1,
                           util::to_array<User::USERNAME_T>("123"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("userid already exists"));
    }
    SECTION("Cannot add user with same or higher privilege") {
        session.login_push(util::to_array<User::USERID_T>("salesman"));
        AddUserCommand cmd(util::to_array<User::USERID_T>("new_root"),
                           util::to_array<User::PASSWORD_T>("456"), 7,
                           util::to_array<User::USERNAME_T>("123"));
        REQUIRE_THROWS_WITH(
            cmd.execute(session),
            Catch::Matchers::ContainsSubstring("cannot add user with same or higher privilege"));
    }
    SECTION("correct add user") {
        session.login_push(util::to_array<User::USERID_T>("salesman"));
        AddUserCommand cmd(util::to_array<User::USERID_T>("new_customer"),
                           util::to_array<User::PASSWORD_T>("456"), 1,
                           util::to_array<User::USERNAME_T>("123"));
        REQUIRE_NOTHROW(cmd.execute(session));
    }
}
TEST_CASE("UserCommands: DELETE Tests", "[UserCommands]") {
    Session session;
    setup();

    SECTION("Privilege check (<7 cannot delete)") {
        session.login_push(util::to_array<User::USERID_T>("salesman"));
        DeleteUserCommand cmd(util::to_array<User::USERID_T>("customer"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("privilege not enough"));
    }

    SECTION("Root can delete user") {
        session.login_push(util::to_array<User::USERID_T>("root"));

        DeleteUserCommand cmd(util::to_array<User::USERID_T>("customer"));
        REQUIRE_NOTHROW(cmd.execute(session));

        CHECK_FALSE(
            session.users_manager.userid_exists(util::to_array<User::USERID_T>("customer")));
    }

    SECTION("Cannot delete logged in user") {
        session.login_push(util::to_array<User::USERID_T>("customer"));
        session.login_push(util::to_array<User::USERID_T>("root"));
        DeleteUserCommand cmd(util::to_array<User::USERID_T>("customer"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("userid have been logged in"));
    }

    SECTION("Delete non-existent user") {
        session.login_push(util::to_array<User::USERID_T>("root"));
        DeleteUserCommand cmd(util::to_array<User::USERID_T>("nobody"));
        REQUIRE_THROWS_WITH(cmd.execute(session),
                            Catch::Matchers::ContainsSubstring("userid doesn't exist"));
    }
}