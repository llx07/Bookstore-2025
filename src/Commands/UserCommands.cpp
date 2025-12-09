#include "Commands/UserCommands.hpp"

void SwitchUserCommand::execute(Session& session) {
    if (session.getPrivilege() < 0) {
        throw ExecutionException("su error: privilege not enough to operate.");
    }

    if (!session.users_manager.useridExists(userid)) {
        throw ExecutionException("su error: userID don't exists.");
    }
    const User& user = session.users_manager.getUserByUserid(userid);
    if (password == std::nullopt) {  // no password provided
        // FIXME(llx) ambiguous meaning of “高于”
        if (session.getPrivilege() <= user.privilege) {
            throw ExecutionException("su error: privilege not enough to omit password.");
        }
    } else {
        if (!session.users_manager.isPasswordCorrect(userid, password.value())) {
            throw ExecutionException("su error: password incorrect.");
        }
    }

    session.loginPush(userid);
}
SwitchUserCommand::SwitchUserCommand(const User::USERID_T& _userid) : userid(_userid) {}
SwitchUserCommand::SwitchUserCommand(const User::USERID_T& _userid,
                                     const User::PASSWORD_T& _password)
    : userid(_userid), password(_password) {}

void LogoutCommand::execute(Session& session) {
    if (session.getPrivilege() < 1) {
        throw ExecutionException("logout error: privilege not enough to operate.");
    }
    if (session.loginEmpty()) {
        throw ExecutionException("logout error: no user logged in");
    }
    session.loginPop();
}

void RegisterCommand::execute(Session& session) {
    if (session.getPrivilege() < 0) {
        throw ExecutionException("register error: privilege not enough to operate.");
    }

    if (session.users_manager.useridExists(userid)) {
        throw ExecutionException("register error: userid already exists.");
    }
    User user;
    user.userid = userid;
    user.username = username;
    user.password = password;
    user.privilege = 1;
    session.users_manager.addUser(user);
}
RegisterCommand::RegisterCommand(const User::USERID_T& _userid, const User::PASSWORD_T& _password,
                                 const User::USERNAME_T& _username)
    : userid(_userid), password(_password), username(_username) {}

void ChangePasswordCommand::execute(Session& session) {
    if (session.getPrivilege() < 1) {
        throw ExecutionException("passwd error: privilege not enough to operate.");
    }

    if (!session.users_manager.useridExists(userid)) {
        throw ExecutionException("passwd error: userid doesn't exist.");
    }
    if (current_password == std::nullopt) {
        if (session.getPrivilege() != 7) {
            throw ExecutionException(
                "passwd error: privilege not enough to omit current password.");
        }
    } else {
        if (!session.users_manager.isPasswordCorrect(userid, current_password.value())) {
            throw ExecutionException("passwd error: current password is incorrect.");
        }
    }
    session.users_manager.modifyPassword(userid, new_password);
}
ChangePasswordCommand::ChangePasswordCommand(const User::USERID_T& _userid,
                                             const User::PASSWORD_T& _new_password)
    : userid(_userid), new_password(_new_password) {}
ChangePasswordCommand::ChangePasswordCommand(const User::USERID_T& _userid,
                                             const User::PASSWORD_T& _current_password,
                                             const User::PASSWORD_T& _new_password)
    : userid(_userid), current_password(_current_password), new_password(_new_password) {}

void AddUserCommand::execute(Session& session) {
    if (session.getPrivilege() < 3) {
        throw ExecutionException("useradd error: privilege not enough to operate.");
    }

    if (session.users_manager.useridExists(userid)) {
        throw ExecutionException("useradd error: userid already exists.");
    }
    if (privilege >= session.getPrivilege()) {
        throw ExecutionException("useradd error: cannot add user with same or higher privilege.");
    }
    User user;
    user.userid = userid;
    user.username = username;
    user.password = password;
    user.privilege = privilege;
    session.users_manager.addUser(user);
}
AddUserCommand::AddUserCommand(const User::USERID_T& _userid, const User::PASSWORD_T& _password,
                               int _privilege, const User::USERNAME_T& _username)
    : userid(_userid), password(_password), privilege(_privilege), username(_username) {}

void DeleteUserCommand::execute(Session& session) {
    if (session.getPrivilege() < 7) {
        throw ExecutionException("delete error: privilege not enough to operate.");
    }

    if (!session.users_manager.useridExists(userid)) {
        throw ExecutionException("delete error: userid doesn't exist.");
    }
    if (session.isLoggedIn(userid)) {
        throw ExecutionException("delete error: userid have been logged in");
    }
    session.users_manager.eraseUser(userid);
}
DeleteUserCommand::DeleteUserCommand(const User::USERID_T& _userid) : userid(_userid) {}