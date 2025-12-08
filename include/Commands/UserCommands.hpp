#ifndef BOOKSTORE_USERCOMMANDS_HPP
#define BOOKSTORE_USERCOMMANDS_HPP

#include <optional>

#include "Commands/CommandBase.hpp"
#include "UsersManager.hpp"

// # 帐户系统指令
// su [UserID] ([Password])?
// logout
// register [UserID] [Password] [Username]
// passwd [UserID] ([CurrentPassword])? [NewPassword]
// useradd [UserID] [Password] [Privilege] [Username]
// delete [UserID]

class SwitchUserCommand : public Command {
public:
    void execute(Session& session) override;
    SwitchUserCommand(const User::USERID_T& _userid);
    SwitchUserCommand(const User::USERID_T& _userid, const User::PASSWORD_T& _password);

private:
    User::USERID_T userid;
    std::optional<User::PASSWORD_T> password;
};

class LogoutCommand : public Command {
public:
    void execute(Session& session) override;
};

class RegisterCommand : public Command {
public:
    void execute(Session& session) override;
    RegisterCommand(const User::USERID_T& _userid, const User::PASSWORD_T& _password,
                    const User::USERNAME_T& _username);

private:
    User::USERID_T userid;
    User::PASSWORD_T password;
    User::USERNAME_T username;
};

class ChangePasswordCommand : public Command {
public:
    void execute(Session& session) override;
    ChangePasswordCommand(const User::USERID_T& _userid, const User::PASSWORD_T& _new_password);
    ChangePasswordCommand(const User::USERID_T& _userid, const User::PASSWORD_T& _current_password,
                          const User::PASSWORD_T& _new_password);

private:
    User::USERID_T userid;
    std::optional<User::PASSWORD_T> current_password;
    User::PASSWORD_T new_password;
};

class AddUserCommand : public Command {
public:
    void execute(Session& session) override;
    AddUserCommand(const User::USERID_T& _userid, const User::PASSWORD_T& _password, int _privilege,
                   const User::USERNAME_T& _username);

private:
    User::USERID_T userid;
    User::PASSWORD_T password;
    int privilege;
    User::USERNAME_T username;
};

class DeleteUserCommand : public Command {
public:
    void execute(Session& session) override;
    DeleteUserCommand(const User::USERID_T& _userid);

private:
    User::USERID_T userid;
};

#endif  // BOOKSTORE_USERCOMMANDS_HPP
