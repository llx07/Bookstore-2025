#include "Commands/UserCommands.hpp"

void SwitchUserCommand::execute(User::USERID_T& current_userid, int& current_bookid,
                                std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 0) {
        throw ExecutionException("su error: privilege not enough to operate.");
    }

    if (!usr_mgr.useridExists(userid)) {
        throw ExecutionException("su error: userID don't exists.");
    }
    const User& user = usr_mgr.getUserByUserid(userid);
    if (password == std::nullopt) {  // no password provided
        // FIXME(llx) ambiguous meaning of “高于”
        if (usr_mgr.getUserByUserid(current_userid).privilege <= user.privilege) {
            throw ExecutionException("su error: privilege not enough to omit password.");
        }
    } else {
        if (!usr_mgr.isPasswordCorrect(userid, password.value())) {
            throw ExecutionException("su error: password incorrect.");
        }
    }

    current_userid = userid;
    current_bookid = 0;
    usr_mgr.modifyLoginCount(current_userid, 1);
}
SwitchUserCommand::SwitchUserCommand(const User::USERID_T& _userid) : userid(_userid) {}
SwitchUserCommand::SwitchUserCommand(const User::USERID_T& _userid,
                                     const User::PASSWORD_T& _password)
    : userid(_userid), password(_password) {}

void LogoutCommand::execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 1) {
        throw ExecutionException("logout error: privilege not enough to operate.");
    }
    usr_mgr.modifyLoginCount(current_userid, -1);
}

void RegisterCommand::execute(User::USERID_T& current_userid, int& current_bookid,
                              std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 0) {
        throw ExecutionException("register error: privilege not enough to operate.");
    }

    if (usr_mgr.useridExists(userid)) {
        throw ExecutionException("register error: userid already exists.");
    }
    User user;
    user.userid = userid;
    user.username = username;
    user.password = password;
    user.privilege = 1;
    usr_mgr.addUser(user);
}
RegisterCommand::RegisterCommand(const User::USERID_T& _userid, const User::PASSWORD_T& _password,
                                 const User::USERNAME_T& _username)
    : userid(_userid), password(_password), username(_username) {}

void ChangePasswordCommand::execute(User::USERID_T& current_userid, int& current_bookid,
                                    std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 1) {
        throw ExecutionException("passwd error: privilege not enough to operate.");
    }

    if (!usr_mgr.useridExists(userid)) {
        throw ExecutionException("passwd error: userid doesn't exist.");
    }
    if (current_password == std::nullopt) {
        if (usr_mgr.getUserByUserid(current_userid).privilege != 7) {
            throw ExecutionException(
                "passwd error: privilege not enough to omit current password.");
        }
    } else {
        if (!usr_mgr.isPasswordCorrect(userid, current_password.value())) {
            throw ExecutionException("passwd error: current password is incorrect.");
        }
    }
    usr_mgr.modifyPassword(userid, new_password);
}
ChangePasswordCommand::ChangePasswordCommand(const User::USERID_T& _userid,
                                             const User::PASSWORD_T& _new_password)
    : userid(_userid), new_password(_new_password) {}
ChangePasswordCommand::ChangePasswordCommand(const User::USERID_T& _userid,
                                             const User::PASSWORD_T& _current_password,
                                             const User::PASSWORD_T& _new_password)
    : userid(_userid), current_password(_current_password), new_password(_new_password) {}

void AddUserCommand::execute(User::USERID_T& current_userid, int& current_bookid,
                             std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 3) {
        throw ExecutionException("useradd error: privilege not enough to operate.");
    }

    if (usr_mgr.useridExists(userid)) {
        throw ExecutionException("useradd error: userid already exists.");
    }
    if (privilege >= usr_mgr.getUserByUserid(current_userid).privilege) {
        throw ExecutionException("useradd error: cannot add user with same or higher privilege.");
    }
    User user;
    user.userid = userid;
    user.username = username;
    user.password = password;
    user.privilege = privilege;
    usr_mgr.addUser(user);
}
AddUserCommand::AddUserCommand(const User::USERID_T& _userid, const User::PASSWORD_T& _password,
                               int _privilege, const User::USERNAME_T& _username)
    : userid(_userid), password(_password), privilege(_privilege), username(_username) {}

void DeleteUserCommand::execute(User::USERID_T& current_userid, int& current_bookid,
                                std::ostream& os) {
    if (usr_mgr.getUserByUserid(current_userid).privilege < 7) {
        throw ExecutionException("delete error: privilege not enough to operate.");
    }

    if (!usr_mgr.useridExists(userid)) {
        throw ExecutionException("delete error: userid doesn't exist.");
    }
    if (usr_mgr.getLoginCount(userid) > 0) {
        throw ExecutionException("delete error: userid have been logged in");
    }
    usr_mgr.eraseUser(userid);
}
DeleteUserCommand::DeleteUserCommand(const User::USERID_T& _userid) : userid(_userid) {}