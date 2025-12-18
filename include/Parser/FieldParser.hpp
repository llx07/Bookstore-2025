#ifndef BOOKSTORE_FIELDPARSER_HPP
#define BOOKSTORE_FIELDPARSER_HPP

#include "BooksManager.hpp"
#include "Charset.hpp"
#include "LogManager.hpp"
#include "UsersManager.hpp"
#include "Validator.hpp"

Book::ISBN_T parseISBN(const std::string& token);
Book::BOOKNAME_T parseBookName(const std::string& token);
Book::AUTHOR_T parseAuthor(const std::string& token);
Book::KEYWORD_T parseKeyword(const std::string& token);
int parseQuantity(const std::string& token);
long long parsePrice(const std::string& token);
int parseCount(const std::string& token);
User::USERID_T parse_userid(const std::string& token);
User::PASSWORD_T parse_password(const std::string& token);
int parse_privilege(const std::string& token);
User::USERNAME_T parse_username(const std::string& token);

#endif  // BOOKSTORE_FIELDPARSER_HPP
