#include <iostream>
#include <stack>

#include "BooksManager.hpp"
#include "Commands/UserCommands.hpp"
#include "LogManager.hpp"
#include "Parser/Parser.hpp"
#include "UsersManager.hpp"
#include "Utils.hpp"

int main() {
    std::stack<std::pair<User::USERID_T, int>> login_stack;

    std::string line;
    while (std::getline(std::cin, line)) {
        auto tokens = util::split(line, ' ');
        erase_if(tokens, [](const std::string& token) { return token.empty(); });
        if (tokens.empty()) {
            continue;
        }

        if (tokens.size() == 1 && (tokens[0] == "quit" || tokens[0] == "exit")) {
            break;
        }

        try {
            auto command = parseCommand(tokens);

            std::string command_stripped{};
            for (int i = 0; i < tokens.size(); i++) {
                command_stripped += tokens[i];
                if (i != tokens.size() - 1) {
                    command_stripped += " ";
                }
            }

            User::USERID_T current_user = !login_stack.empty()
                                              ? login_stack.top().first
                                              : util::toArray<User::USERID_T>("<GUEST>");
            int privilege = UsersManager::getInstance().getUserByUserid(current_user).privilege;
            int current_book = !login_stack.empty() ? login_stack.top().second : 0;
            int log_id = LogManager::getInstance().addOperationLog(
                util::getTimestamp(), current_user, privilege,
                util::toArray<Log::OPERATION_T>(command_stripped));

            if (dynamic_cast<SwitchUserCommand*>(command.get())) {
                User::USERID_T new_userid = current_user;
                int new_selected_book = 0;
                command->execute(new_userid, new_selected_book, std::cout);
                login_stack.emplace(new_userid, new_selected_book);
            } else if (dynamic_cast<LogoutCommand*>(command.get())) {
                command->execute(current_user, current_book, std::cout);
                login_stack.pop();
            } else if (dynamic_cast<SelectCommand*>(command.get())) {
                command->execute(current_user, current_book, std::cout);
                login_stack.top().second = current_book;
            } else {
                command->execute(current_user, current_book, std::cout);
            }
            // operation success
            LogManager::getInstance().markOperationSuccess(log_id);
        } catch (const std::exception& e) {
            std::cerr << "[VERBOSE] From main:" << e.what() << std::endl;
            std::cout << "Invalid\n";
        }
    }

    while (!login_stack.empty()) {
        LogoutCommand{}.execute(login_stack.top().first, login_stack.top().second, std::cout);
        login_stack.pop();
    }
}