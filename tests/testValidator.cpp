#include <catch2/catch_test_macros.hpp>

#include "Validator.hpp"

TEST_CASE("Basic Validator", "[Validator]") {
    expect(1).toBe(1);
    expect(12345).toBeOneOf(1, 123, 1234, 12345);
    expect(12345).le(23456);
    expect(std::string("zzz")).ge(std::string("aaa"));
    expect('s').ge('a').le('z');
    REQUIRE_THROWS_AS(expect('s').ge('A').le('Z'), ExpectException);
}
TEST_CASE("Validator Not Clause", "[Validator]") {
    expect(1).Not().toBe(2);
    REQUIRE_THROWS_AS(expect(1).Not().toBe(3).Or.toBe(4).Or.toBe(1), ExpectException);
}
TEST_CASE("String Validator", "[Validator]") {
    const std::string& ascii_lowercase{"abcdefghijklmnopqrstuvwxyz"};
    expect(std::string{"helloworldhasonlylowercases"}).consistedOf(ascii_lowercase);
    REQUIRE_THROWS_AS(expect(std::string{"hello world has characters that is not lowercase"})
                          .consistedOf(ascii_lowercase),
                      ExpectException);

    expect(std::string{"TEST1"}).toMatch(".{4}\\d");
    expect(std::string{"TEST1"}).toMatch(".+?\\d");
    REQUIRE_THROWS_AS(expect(std::string{"TEST1"}).toMatch(".{6}"), ExpectException);
}
TEST_CASE("Validator Custom Function", "[Validator]") {
    expect(std::string{"12345"}).satisfy([](auto str) {
        return std::ranges::all_of(str, [](char t) { return '0' <= t && t <= '9'; });
    });
    REQUIRE_THROWS_AS(expect(std::string{"-12345"}).satisfy([](auto str) {
        return std::ranges::all_of(str, [](char t) { return '0' <= t && t <= '9'; });
    }),
                      ExpectException);
}
