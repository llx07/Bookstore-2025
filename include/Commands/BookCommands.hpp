#ifndef BOOKSTORE_BOOKCOMMANDS_HPP
#define BOOKSTORE_BOOKCOMMANDS_HPP
#include <optional>

#include "CommandBase.hpp"

// # 图书系统指令
// show (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]")?
// buy [ISBN] [Quantity]
// select [ISBN]
// modify (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]" |
// -price=[Price])+
// import [Quantity] [TotalCost]

class ShowCommand : public Command {
public:
    void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) override;
    ShowCommand() = default;
    std::optional<Book::ISBN_T> ISBN;
    std::optional<Book::BOOKNAME_T> name;
    std::optional<Book::AUTHOR_T> author;
    std::optional<Book::KEYWORD_T> keyword;
};

class BuyCommand : public Command {
public:
    void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) override;
    BuyCommand(const Book::ISBN_T& _ISBN, int _quantity);

private:
    Book::ISBN_T ISBN;
    int quantity;
};

class SelectCommand : public Command {
public:
    void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) override;
    SelectCommand(const Book::ISBN_T& _ISBN);

private:
    Book::ISBN_T ISBN;
};

class ModifyCommand : public Command {
public:
    void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) override;
    ModifyCommand() = default;
    std::optional<Book::ISBN_T> new_ISBN;
    std::optional<Book::BOOKNAME_T> new_name;
    std::optional<Book::AUTHOR_T> new_author;
    std::optional<Book::KEYWORD_T> new_keyword;
    std::optional<long long> new_price;
};

class ImportCommand : public Command {
public:
    void execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) override;
    ImportCommand(int _quantity, long long _total_cost);

private:
    int quantity;
    long long total_cost;
};

#endif  // BOOKSTORE_BOOKCOMMANDS_HPP
