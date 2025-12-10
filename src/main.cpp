#include <iostream>

#include "Parser/Parser.hpp"
#include "Session.hpp"
#include "Utils.hpp"

int main() {
    Session session;
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
        session.updateTimestamp();
        try {
            auto command = parseCommand(tokens);

            std::string command_stripped{};
            for (int i = 0; i < tokens.size(); i++) {
                command_stripped += tokens[i];
                if (i != tokens.size() - 1) {
                    command_stripped += " ";
                }
            }

            // parse success
            int log_id = session.log_manager.addOperationLog(
                session.getTimestamp(), session.getCurrentUser(), session.getPrivilege(),
                util::toArray<Log::OPERATION_T>(command_stripped));
            command->execute(session);
            // operation success
            session.log_manager.markOperationSuccess(log_id);
        } catch (const std::exception& e) {
            std::cerr << "[VERBOSE] From main:" << e.what() << std::endl;
            std::cout << "Invalid\n";
        }
    }
}