
#include <catch2/catch_test_macros.hpp>

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
        auto result = util::to_array<std::array<char, 10>>("hello");
        REQUIRE(result ==
                std::array<char, 10>{'h', 'e', 'l', 'l', 'o', '\0', '\0', '\0', '\0', '\0'});
    }

    SECTION("Empty string") {
        auto result = util::to_array<std::array<char, 5>>(std::string{});
        REQUIRE(result == std::array<char, 5>{'\0', '\0', '\0', '\0', '\0'});
    }
}