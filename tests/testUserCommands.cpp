// #include <catch2/catch_test_macros.hpp>
// #include <catch2/matchers/catch_matchers_string.hpp>
//
// #include "BooksManager.hpp"
// #include "Commands/UserCommands.hpp"
// #include "UsersManager.hpp"
// #include "Utils.hpp"
//
// static void setup() {
//     BooksManager::getInstance().reset();
//     UsersManager::getInstance().reset();
//
//     User user;
//     user.userid = util::toArray<User::USERID_T>("customer");
//     user.password = util::toArray<User::PASSWORD_T>("123");
//     user.privilege = 1;
//     UsersManager::getInstance().addUser(user);
//
//     user.userid = util::toArray<User::USERID_T>("salesman");
//     user.password = util::toArray<User::PASSWORD_T>("456");
//     user.privilege = 3;
//     UsersManager::getInstance().addUser(user);
// }
//
// TEST_CASE("UserCommands: SU command", "[UserCommands]") {
//     setup();
//     Session session;
//
//     SECTION("Login with correct password") {
//         SwitchUserCommand cmd(util::toArray<User::USERID_T>("customer"),
//                               util::toArray<User::PASSWORD_T>("123"));
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//         REQUIRE(session.isLoggedIn(util::toArray<User::USERID_T>("customer")));
//         REQUIRE(session.getPrivilege() == 1);
//     }
//
//     SECTION("Login with wrong password") {
//         SwitchUserCommand cmd(util::toArray<User::USERID_T>("customer"),
//                               util::toArray<User::PASSWORD_T>("114514"));
//         REQUIRE_THROWS_AS(cmd.execute(session, TODO, TODO), ExecutionException);
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("password incorrect"));
//     }
//
//     SECTION("Login to non-existent user") {
//         SwitchUserCommand cmd(util::toArray<User::USERID_T>("nobody"),
//                               util::toArray<User::PASSWORD_T>("114514"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("userID don't exists"));
//     }
//
//     SECTION("Su without password - privilege less than") {
//         SwitchUserCommand cmd(util::toArray<User::USERID_T>("customer"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO), Catch::Matchers::ContainsSubstring(
//                                                       "privilege not enough to omit password"));
//     }
//
//     SECTION("Su without password - privilege higher than") {
//         session.loginPush(util::toArray<User::USERID_T>("root"));
//         REQUIRE(session.getPrivilege() == 7);
//         SwitchUserCommand cmd(util::toArray<User::USERID_T>("customer"));
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//         REQUIRE(session.isLoggedIn(util::toArray<User::USERID_T>("customer")));
//         REQUIRE(session.getPrivilege() == 1);
//     }
//     SECTION("Su without password - privilege higher than 2") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         REQUIRE(session.getPrivilege() == 3);
//         SwitchUserCommand cmd(util::toArray<User::USERID_T>("customer"));
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//         REQUIRE(session.isLoggedIn(util::toArray<User::USERID_T>("customer")));
//         REQUIRE(session.getPrivilege() == 1);
//     }
//     SECTION("Su without password - same privilege") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         REQUIRE(session.getPrivilege() == 1);
//         SwitchUserCommand cmd(util::toArray<User::USERID_T>("customer"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO), Catch::Matchers::ContainsSubstring(
//                                                       "privilege not enough to omit password"));
//         REQUIRE(session.getPrivilege() == 1);
//     }
// }
//
// TEST_CASE("UserCommands: LOGOUT command", "[UserCommands]") {
//     setup();
//     Session session;
//     SECTION("Correct Usage") {
//         LogoutCommand cmd;
//         session.loginPush(util::toArray<User::USERID_T>("root"));
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//     }
//     SECTION("Logout when not logged in") {
//         LogoutCommand cmd;
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("privilege not enough to
//                             operate"));
//     }
// }
// TEST_CASE("UserCommands: REGISTER command", "[UserCommands]") {
//     setup();
//     Session session;
//     SECTION("Success register") {
//         RegisterCommand cmd(util::toArray<User::USERID_T>("newuser"),
//                             util::toArray<User::PASSWORD_T>("password"),
//                             util::toArray<User::USERID_T>("mycustomer"));
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//         REQUIRE(session.users_manager.useridExists(util::toArray<User::USERID_T>("newuser")));
//     }
//
//     SECTION("Register duplicate ID") {
//         RegisterCommand cmd(util::toArray<User::USERID_T>("root"),
//                             util::toArray<User::PASSWORD_T>("sjtu"),
//                             util::toArray<User::USERID_T>("Hacker"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("userid already exists"));
//     }
// }
// TEST_CASE("UserCommands: PASSWD command", "[UserCommands]") {
//     setup();
//     Session session;
//     SECTION("Guest cannot change password") {
//         ChangePasswordCommand cmd(util::toArray<User::USERID_T>("root"),
//                                   util::toArray<User::PASSWORD_T>("sjtu"),
//                                   util::toArray<User::PASSWORD_T>("123456"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("privilege not enough to
//                             operate"));
//     }
//     SECTION("Userid not exist") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         ChangePasswordCommand cmd(util::toArray<User::USERID_T>("strange_user"),
//                                   util::toArray<User::PASSWORD_T>("654321"),
//                                   util::toArray<User::PASSWORD_T>("123456"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("userid doesn't exist"));
//     }
//     SECTION("No password - privilege not enough") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         ChangePasswordCommand cmd(util::toArray<User::USERID_T>("customer"),
//                                   util::toArray<User::PASSWORD_T>("123456"));
//         REQUIRE_THROWS_WITH(
//             cmd.execute(session, TODO, TODO),
//             Catch::Matchers::ContainsSubstring("privilege not enough to omit current password"));
//     }
//     SECTION("No password - privilege enough") {
//         session.loginPush(util::toArray<User::USERID_T>("root"));
//         ChangePasswordCommand cmd(util::toArray<User::USERID_T>("customer"),
//                                   util::toArray<User::PASSWORD_T>("123456"));
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//     }
//     SECTION("Incorrect Password") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         ChangePasswordCommand cmd(util::toArray<User::USERID_T>("customer"),
//                                   util::toArray<User::PASSWORD_T>("456"),
//                                   util::toArray<User::PASSWORD_T>("123"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("current password is incorrect"));
//     }
//     SECTION("Correctly changes password") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         ChangePasswordCommand cmd(util::toArray<User::USERID_T>("customer"),
//                                   util::toArray<User::PASSWORD_T>("123"),
//                                   util::toArray<User::PASSWORD_T>("456"));
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//     }
// }
// TEST_CASE("UserCommands: USERADD", "[UserCommands]") {
//     setup();
//     Session session;
//     SECTION("Privilege is at least 3") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         AddUserCommand cmd(util::toArray<User::USERID_T>("newuser"),
//                            util::toArray<User::PASSWORD_T>("456"), 3,
//                            util::toArray<User::USERNAME_T>("123"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("privilege not enough to
//                             operate"));
//     }
//     SECTION("Cannot add user with same id") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         AddUserCommand cmd(util::toArray<User::USERID_T>("customer"),
//                            util::toArray<User::PASSWORD_T>("456"), 1,
//                            util::toArray<User::USERNAME_T>("123"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("userid already exists"));
//     }
//     SECTION("Cannot add user with same or higher privilege") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         AddUserCommand cmd(util::toArray<User::USERID_T>("new_root"),
//                            util::toArray<User::PASSWORD_T>("456"), 7,
//                            util::toArray<User::USERNAME_T>("123"));
//         REQUIRE_THROWS_WITH(
//             cmd.execute(session, TODO, TODO),
//             Catch::Matchers::ContainsSubstring("cannot add user with same or higher privilege"));
//     }
//     SECTION("correct add user") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         AddUserCommand cmd(util::toArray<User::USERID_T>("new_customer"),
//                            util::toArray<User::PASSWORD_T>("456"), 1,
//                            util::toArray<User::USERNAME_T>("123"));
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//     }
// }
// TEST_CASE("UserCommands: DELETE Tests", "[UserCommands]") {
//     Session session;
//     setup();
//
//     SECTION("Privilege check (<7 cannot delete)") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         DeleteUserCommand cmd(util::toArray<User::USERID_T>("customer"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("privilege not enough"));
//     }
//
//     SECTION("Root can delete user") {
//         session.loginPush(util::toArray<User::USERID_T>("root"));
//
//         DeleteUserCommand cmd(util::toArray<User::USERID_T>("customer"));
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//
//         CHECK_FALSE(session.users_manager.useridExists(util::toArray<User::USERID_T>("customer")));
//     }
//
//     SECTION("Cannot delete logged in user") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         session.loginPush(util::toArray<User::USERID_T>("root"));
//         DeleteUserCommand cmd(util::toArray<User::USERID_T>("customer"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("userid have been logged in"));
//     }
//
//     SECTION("Delete non-existent user") {
//         session.loginPush(util::toArray<User::USERID_T>("root"));
//         DeleteUserCommand cmd(util::toArray<User::USERID_T>("nobody"));
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("userid doesn't exist"));
//     }
// }