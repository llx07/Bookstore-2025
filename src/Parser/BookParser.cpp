

#include <cmath>
#include <limits>
#include <ranges>
#include <set>
#include <string>

#include "BooksManager.hpp"
#include "Charset.hpp"
#include "Commands/BookCommands.hpp"
#include "Parser/Parser.hpp"
#include "Validator.hpp"

// Token will not be empty.
static Book::ISBN_T parseISBN(const std::string& token) {
    expect(token).consistedOf(PRINTABLE);
    return util::toArray<Book::ISBN_T>(token);
}
static Book::BOOKNAME_T parseBookName(const std::string& token) {
    expect(token).consistedOf(PRINTABLE_WITHOUT_QUOTES);
    return util::toArray<Book::BOOKNAME_T>(token);
}
static Book::AUTHOR_T parseAuthor(const std::string& token) {
    expect(token).consistedOf(PRINTABLE_WITHOUT_QUOTES);
    return util::toArray<Book::AUTHOR_T>(token);
}
static Book::KEYWORD_T parseKeyword(const std::string& token) {
    expect(token).consistedOf(PRINTABLE_WITHOUT_QUOTES);
    auto keywords = util::split(token, '|');
    std::ranges::sort(keywords);
    expect(keywords[0].size()).ge(1);
    auto it = std::unique(keywords.begin(), keywords.end());
    expect(it == keywords.end()).toBe(true);
    return util::toArray<Book::KEYWORD_T>(token);
}
static int parseQuantity(const std::string& token) {
    expect(token).consistedOf(NUMERIC);
    int value = util::toInt(token);
    expect(value).ge(1).le(std::numeric_limits<int>::max());
    return value;
}
static long long parsePrice(const std::string& token) {
    expect(token).consistedOf(NUMERIC_DOT);
    expect(token.size()).le(13);
    long long value = util::toDecimal(token);
    expect(value).ge(1);
    return value;
}

enum class Option { ISBN, BOOKNAME, AUTHOR, KEYWORD, PRICE };

static std::pair<Option, std::string> parseOption(const std::string& token) {
    expect(token).consistedOf(PRINTABLE);
    expect(token[0]).toBe('-');
    if (token.size() >= 7 && token.substr(1, 4) == "ISBN") {
        expect(token[5]).toBe('=');
        return std::make_pair(Option::ISBN, token.substr(6));
    }
    if (token.size() >= 9 && token.substr(1, 4) == "name") {
        // -name="12345"
        // 012345678
        expect(token[5]).toBe('=');
        expect(token[6]).toBe('"');
        expect(token.back()).toBe('"');
        expect(token.substr(7, token.size() - 7 - 1)).consistedOf(PRINTABLE_WITHOUT_QUOTES);
        return std::make_pair(Option::BOOKNAME, token.substr(7, token.size() - 7 - 1));
    }
    if (token.size() >= 11 && token.substr(1, 6) == "author") {
        // -author="xxx"
        // 0123456789012
        expect(token[7]).toBe('=');
        expect(token[8]).toBe('"');
        expect(token.back()).toBe('"');
        expect(token.substr(9, token.size() - 9 - 1)).consistedOf(PRINTABLE_WITHOUT_QUOTES);
        return std::make_pair(Option::AUTHOR, token.substr(9, token.size() - 9 - 1));
    }
    if (token.size() >= 12 && token.substr(1, 7) == "keyword") {
        // -keyword="x"
        // 012345678901
        expect(token[8]).toBe('=');
        expect(token[9]).toBe('"');
        expect(token.back()).toBe('"');
        expect(token.substr(10, token.size() - 10 - 1)).consistedOf(PRINTABLE_WITHOUT_QUOTES);
        return std::make_pair(Option::KEYWORD, token.substr(10, token.size() - 10 - 1));
    }
    if (token.size() >= 8 && token.substr(1, 5) == "price") {
        // -price=123.456
        // 0123456789
        expect(token[6]).toBe('=');
        parsePrice(token.substr(7));
        return std::make_pair(Option::PRICE, token.substr(7));
    }

    throw ParseException("Invalid option");
}

// show (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]")?
auto handleSHOW(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 1) {
        return std::make_unique<ShowCommand>();
    }
    if (tokens.size() == 2) {
        auto result = std::make_unique<ShowCommand>();
        auto [tp, val] = parseOption(tokens[1]);
        if (tp == Option::ISBN) {
            result->ISBN = util::toArray<Book::ISBN_T>(val);
        } else if (tp == Option::BOOKNAME) {
            result->name = util::toArray<Book::BOOKNAME_T>(val);
        } else if (tp == Option::AUTHOR) {
            result->author = util::toArray<Book::AUTHOR_T>(val);
        } else if (tp == Option::KEYWORD) {
            expect(val).consistedOf(PRINTABLE_WITHOUT_BAR);
            result->keyword = util::toArray<Book::KEYWORD_T>(val);
        } else {
            throw ParseException("show error: invalid option");
        }
        return result;
    }
    throw ParseException("show error: incorrect arg number.");
}

// buy [ISBN] [Quantity]
auto handleBUY(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 3) {
        return std::make_unique<BuyCommand>(parseISBN(tokens[1]), parseQuantity(tokens[2]));
    }
    throw ParseException("buy error: incorrect number of arg");
}

// select [ISBN]
auto handleSELECT(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 2) {
        return std::make_unique<SelectCommand>(parseISBN(tokens[1]));
    }
    throw ParseException("select error: incorrect number of arg");
}

// modify (-ISBN=[ISBN] | -name="[BookName]" | -author="[Author]" | -keyword="[Keyword]" |
// -price=[Price])+
auto handleMODIFY(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 1) {
        throw ParseException("modify error: nothing to modify");
    }
    std::set<Option> type_had;

    std::vector<std::pair<Option, std::string>> options;
    for (const auto& token : tokens | std::views::drop(1)) {
        auto [tp, val] = parseOption(token);
        if (type_had.contains(tp)) {
            throw ParseException("modify error: option duplicated");
        }
        type_had.insert(tp);
        options.emplace_back(tp, val);
    }

    auto result = std::make_unique<ModifyCommand>();
    for (const auto& [tp, val] : options) {
        switch (tp) {
            case Option::ISBN:
                result->new_ISBN = parseISBN(val);
                break;
            case Option::BOOKNAME:
                result->new_name = parseBookName(val);
                break;
            case Option::AUTHOR:
                result->new_author = parseAuthor(val);
                break;
            case Option::KEYWORD:
                result->new_keyword = parseKeyword(val);
                break;
            case Option::PRICE:
                result->new_price = parsePrice(val);
                break;
            default:
                throw ParseException("modify error: invalid option");
        }
    }
    return result;
}

// import [Quantity] [TotalCost]
auto handleIMPORT(const std::vector<std::string>& tokens) -> std::unique_ptr<Command> {
    if (tokens.size() == 3) {
        return std::make_unique<ImportCommand>(parseQuantity(tokens[1]), parsePrice(tokens[2]));
    }
    throw ParseException("import error: incorrect number of arg");
}

REGISTER_HANDLER(show, handleSHOW);
REGISTER_HANDLER(buy, handleBUY);
REGISTER_HANDLER(select, handleSELECT);
REGISTER_HANDLER(modify, handleMODIFY);
REGISTER_HANDLER(import, handleIMPORT);