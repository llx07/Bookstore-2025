
#include <catch2/catch_test_macros.hpp>
#include <iostream>

#include "Utils.hpp"

TEST_CASE("util::split", "[Utils][split]") {
    SECTION("Empty input") {
        auto result = util::split("");
        REQUIRE(result.empty());
    }
    SECTION("Splitting with no delimiter") {
        auto result = util::split("hello, world!");
        REQUIRE(result == std::vector<std::string>{"hello, world!"});
    }
    SECTION("Basic splitting") {
        auto result = util::split("hello|world");
        REQUIRE(result == std::vector<std::string>{"hello", "world"});
    }
    SECTION("Multiple splitting") {
        auto result = util::split("hi|i|like|genshin|impact");
        REQUIRE(result == std::vector<std::string>{"hi", "i", "like", "genshin", "impact"});
    }
    SECTION("Custom delimiter") {
        auto result = util::split("hi,i,like,genshin,impact", ',');
        REQUIRE(result == std::vector<std::string>{"hi", "i", "like", "genshin", "impact"});
    }
    SECTION("Consecutive delimiter") {
        auto result = util::split("multiple||delimiter");
        REQUIRE(result == std::vector<std::string>{"multiple", "", "delimiter"});
    }
    SECTION("Leading delimiter") {
        auto result = util::split("|a");
        REQUIRE(result == std::vector<std::string>{"", "a"});
    }

    SECTION("Trailing delimiter") {
        auto result = util::split("a|");
        REQUIRE(result == std::vector<std::string>{"a", ""});
    }

    SECTION("String containing only delimiters") {
        auto result = util::split("||");
        REQUIRE(result == std::vector<std::string>{"", "", ""});
    }
}

TEST_CASE("util::to_array", "[Utils][to_array]") {
    SECTION("Basic Usage") {
        auto result = util::toArray<std::array<char, 10>>("hello");
        REQUIRE(result ==
                std::array<char, 10>{'h', 'e', 'l', 'l', 'o', '\0', '\0', '\0', '\0', '\0'});
    }

    SECTION("Empty string") {
        auto result = util::toArray<std::array<char, 5>>(std::string{});
        REQUIRE(result == std::array<char, 5>{'\0', '\0', '\0', '\0', '\0'});
    }
}

TEST_CASE("Validator Convert to Int", "[Validator]") {
    REQUIRE(util::toInt(std::string{"0"}) == 0);
    REQUIRE(util::toInt(std::string{"123"}) == 123);
    REQUIRE(util::toInt(std::string{"-123"}) == -123);
    REQUIRE(util::toInt(std::string{"12300"}) == 12300);
    REQUIRE_THROWS(util::toInt(std::string{"abc"}));
    REQUIRE_THROWS(util::toInt(std::string{"123456789132"}));
    REQUIRE_THROWS(util::toInt(std::string{"0123"}));
}

TEST_CASE("Validator Convert to Decimal", "[Validator]") {
    REQUIRE(util::toDecimal(std::string{"0"}) == 0);
    REQUIRE(util::toDecimal(std::string{"1"}) == 100);
    REQUIRE(util::toDecimal(std::string{"1.2"}) == 120);
    REQUIRE(util::toDecimal(std::string{"12.34"}) == 1234);
    REQUIRE(util::toDecimal(std::string{"0.34"}) == 34);
    REQUIRE_THROWS(util::toDecimal(std::string{"1."}));
    REQUIRE_THROWS(util::toDecimal(std::string{".1"}));
    REQUIRE_THROWS(util::toDecimal(std::string{"00.1"}));
    REQUIRE_THROWS(util::toDecimal(std::string{"01.23"}));
    REQUIRE_THROWS(util::toDecimal(std::string{"1.234"}));
    REQUIRE_THROWS(util::toDecimal(std::string{"1.abc"}));
    REQUIRE_THROWS(util::toDecimal(std::string{"hello"}));
    REQUIRE_THROWS(util::toDecimal(std::string{"hello.5"}));
    REQUIRE_THROWS(util::toDecimal(std::string{"1.."}));
    REQUIRE_THROWS(util::toDecimal(std::string{"1.20.2"}));
}

TEST_CASE("Test Table", "[Table]") {
    std::vector<int> L{1, 5, 3, 9, 7};
    util::printTableHead(std::cerr, L);
    util::printTableBody(std::cerr, L, {"a", "BB", "C", "D", "E"});
    util::printTableMiddle(std::cerr, L);
    util::printTableBody(std::cerr, L, {"1", "2", "3", "4", "5"});
    util::printTableBody(std::cerr, L, {"1", "2long", "3", "4verylonglonglong", "5"});
    util::printTableBottom(std::cerr, L);
}