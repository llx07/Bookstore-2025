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
            command->execute(session);
        } catch (const std::exception& e) {
            std::cerr << "[VERBOSE] From main:" << e.what() << std::endl;
            std::cout << "Invalid\n";
        }
    }
}