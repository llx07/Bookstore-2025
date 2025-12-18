#include "Parser/FieldParser.hpp"
Book::ISBN_T parseISBN(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(PRINTABLE);
    return util::toArray<Book::ISBN_T>(token);
}
Book::BOOKNAME_T parseBookName(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(PRINTABLE_WITHOUT_QUOTES);
    return util::toArray<Book::BOOKNAME_T>(token);
}
Book::AUTHOR_T parseAuthor(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(PRINTABLE_WITHOUT_QUOTES);
    return util::toArray<Book::AUTHOR_T>(token);
}
Book::KEYWORD_T parseKeyword(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(PRINTABLE_WITHOUT_QUOTES);
    auto keywords = util::split(token, '|');
    std::ranges::sort(keywords);
    expect(keywords[0].size()).ge(1);
    auto it = std::unique(keywords.begin(), keywords.end());
    expect(it == keywords.end()).toBe(true);
    return util::toArray<Book::KEYWORD_T>(token);
}
int parseQuantity(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(NUMERIC);
    int value = util::toInt(token);
    expect(value).ge(1).le(std::numeric_limits<int>::max());
    return value;
}
long long parsePrice(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(NUMERIC_DOT);
    expect(token.size()).le(13);
    long long value = util::toDecimal(token);
    expect(value).ge(0);
    return value;
}
int parseCount(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(NUMERIC);
    int value = util::toInt(token);
    expect(value).ge(1).le(std::numeric_limits<int>::max());
    return value;
}
User::USERID_T parse_userid(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(ALPHANUMERIC_UNDERSCORE);
    return util::toArray<User::USERID_T>(token);
}
User::PASSWORD_T parse_password(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(ALPHANUMERIC_UNDERSCORE);
    return util::toArray<User::PASSWORD_T>(token);
}
int parse_privilege(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(NUMERIC);
    int value = util::toInt(token);
    expect(value).toBeOneOf(0, 1, 3, 7);
    return value;
}
User::USERNAME_T parse_username(const std::string& token) {
    expect(token.empty()).Not().toBe(true);
    expect(token).consistedOf(PRINTABLE);
    return util::toArray<User::USERNAME_T>(token);
}