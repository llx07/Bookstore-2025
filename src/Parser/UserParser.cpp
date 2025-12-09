#include <string>

#include "Charset.hpp"
#include "Commands/UserCommands.hpp"
#include "Parser/Parser.hpp"
#include "UsersManager.hpp"
#include "Utils.hpp"
#include "Validator.hpp"

static User::USERID_T parse_userid(const std::string& token) {
    expect(token).consistedOf(ALPHANUMERIC_UNDERSCORE);
    return util::to_array<User::USERID_T>(token);
}
static User::PASSWORD_T parse_password(const std::string& token) {
    expect(token).consistedOf(ALPHANUMERIC_UNDERSCORE);
    return util::to_array<User::PASSWORD_T>(token);
}
static int parse_privilege(const std::string& token) {
    expect(token).consistedOf(NUMERIC);
    int value = util::toInt(token);
    expect(value).toBeOneOf(0, 1, 3, 7);
    return value;
}
static User::USERNAME_T parse_username(const std::string& token) {
    expect(token).consistedOf(PRINTABLE);
    return util::to_array<User::USERNAME_T>(token);
}

// su [UserID] ([Password])?
static auto handleSU(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 1) {
        throw ParseException("su: args not enough");
    }
    if (tokens.size() == 2) {
        auto userid = parse_userid(tokens[1]);
        return std::make_unique<SwitchUserCommand>(userid);
    }
    if (tokens.size() == 3) {
        auto userid = parse_userid(tokens[1]);
        auto password = parse_password(tokens[2]);
        return std::make_unique<SwitchUserCommand>(userid, password);
    }
    throw ParseException("su: args too many");
}
// logout
static auto handleLOGOUT(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 1) {
        return std::make_unique<LogoutCommand>();
    }
    throw ParseException("logout: args too many");
}
// register [UserID] [Password] [Username]

static auto handleREGISTER(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 4) {
        return std::make_unique<RegisterCommand>(parse_userid(tokens[1]), parse_password(tokens[2]),
                                                 parse_username(tokens[3]));
    }
    throw ParseException("register: args number incorrect");
}

// passwd [UserID] ([CurrentPassword])? [NewPassword]
static auto handlePASSWD(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 3) {
        auto userid = parse_userid(tokens[1]);
        auto password = parse_password(tokens[2]);
        return std::make_unique<ChangePasswordCommand>(userid, password);
    }
    if (tokens.size() == 4) {
        auto userid = parse_userid(tokens[1]);
        auto password = parse_password(tokens[2]);
        auto password_new = parse_password(tokens[3]);
        return std::make_unique<ChangePasswordCommand>(userid, password, password_new);
    }
    throw ParseException("passwd: args number incorrect");
}
// useradd [UserID] [Password] [Privilege] [Username]
static auto handleUSERADD(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 5) {
        auto userid = parse_userid(tokens[1]);
        auto password = parse_password(tokens[2]);
        auto privilege = parse_privilege(tokens[3]);
        auto username = parse_username(tokens[4]);
        return std::make_unique<AddUserCommand>(userid, password, privilege, username);
    }
    throw ParseException("passwd: args number incorrect");
}
// delete [UserID]
static auto handleDELETE(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 2) {
        auto userid = parse_userid(tokens[1]);
        return std::make_unique<DeleteUserCommand>(userid);
    }
    throw ParseException("delete: args number incorrect");
}

REGISTER_HANDLER(su, handleSU);
REGISTER_HANDLER(logout, handleLOGOUT);
REGISTER_HANDLER(register, handleREGISTER);
REGISTER_HANDLER(passwd, handlePASSWD);
REGISTER_HANDLER(useradd, handleUSERADD);
REGISTER_HANDLER(delete, handleDELETE);
