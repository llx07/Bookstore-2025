#include "Commands/CommandBase.hpp"

#include "BooksManager.hpp"
#include "LogManager.hpp"
#include "UsersManager.hpp"

UsersManager& Command::usr_mgr = UsersManager::getInstance();
BooksManager& Command::bk_mgr = BooksManager::getInstance();
LogManager& Command::log_mgr = LogManager::getInstance();