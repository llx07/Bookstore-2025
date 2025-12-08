#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <random>

#include "BooksManager.hpp"
#include "Utils.hpp"

Book gen_book(const std::string& ISBN, const std::string& book_name, const std::string& author,
              const std::string& keywords, int quantity, long long price) {
    Book book;
    book.ISBN = util::to_array<Book::ISBN_T>(ISBN);
    book.book_name = util::to_array<Book::BOOKNAME_T>(book_name);
    book.author = util::to_array<Book::AUTHOR_T>(author);
    book.keywords = util::to_array<Book::KEYWORD_T>(keywords);
    book.quantity = quantity;
    book.price = price;
    return book;
}
TEST_CASE("BooksManager Basic Usage", "[BooksManager]") {
    auto& manager = BooksManager::getInstance();
    manager.reset();
    SECTION("Create") {
        const auto& ISBN = util::to_array<Book::ISBN_T>("A");
        manager.create_book(ISBN);
        auto result = manager.get_books_with_ISBN(ISBN);

        REQUIRE(result.size() == 1);
        REQUIRE(result[0].ISBN == ISBN);
        REQUIRE(std::ranges::all_of(result[0].author, [](char c) { return c == 0; }));
        REQUIRE(std::ranges::all_of(result[0].book_name, [](char c) { return c == 0; }));
        REQUIRE(std::ranges::all_of(result[0].keywords, [](char c) { return c == 0; }));
        REQUIRE(result[0].price == 0);
    }
    SECTION("Modification") {
        const auto& ISBN = util::to_array<Book::ISBN_T>("A");
        manager.create_book(ISBN);
        Book data_before = gen_book("A", "Old Name", "Old Author", "K1|K2", 0, 0);
        manager.modify_book_data(ISBN, data_before);

        auto result = manager.get_books_with_ISBN(ISBN);
        REQUIRE(result.size() == 1);
        result = manager.get_books_with_keyword(util::to_array<Book::KEYWORD_T>("K1"));
        REQUIRE(result.size() == 1);

        Book data_after = gen_book("C", "New Name", "New Author", "K3", 0, 0);
        manager.modify_book_data(ISBN, data_after);

        result = manager.get_books_with_ISBN(ISBN);
        REQUIRE(result.empty());
        result = manager.get_books_with_keyword(util::to_array<Book::KEYWORD_T>("K1"));
        REQUIRE(result.empty());
        result = manager.get_books_with_ISBN(util::to_array<Book::ISBN_T>("C"));
        REQUIRE(result.size() == 1);
        REQUIRE(result[0].book_name == util::to_array<Book::BOOKNAME_T>("New Name"));

        result = manager.get_books_with_keyword(util::to_array<Book::KEYWORD_T>("K3"));
        REQUIRE(result.size() == 1);
    }
    SECTION("Keyword indexing") {
        manager.create_book(util::to_array<Book::ISBN_T>("C"));
        manager.modify_book_data(util::to_array<Book::ISBN_T>("C"),
                                 gen_book("C", "", "", "KW1|KW2", 0, 0));

        manager.create_book(util::to_array<Book::ISBN_T>("D"));
        manager.modify_book_data(util::to_array<Book::ISBN_T>("D"),
                                 gen_book("D", "", "", "KW2|KW3", 0, 0));
        manager.create_book(util::to_array<Book::ISBN_T>("E"));
        manager.modify_book_data(util::to_array<Book::ISBN_T>("E"),
                                 gen_book("E", "", "", "KW1|KW4", 0, 0));
        auto result = manager.get_books_with_keyword(util::to_array<Book::KEYWORD_T>("KW1"));
        REQUIRE(result.size() == 2);
        result = manager.get_books_with_keyword(util::to_array<Book::KEYWORD_T>("KW3"));
        REQUIRE(result.size() == 1);
    }
    SECTION("Import and Buy Logic") {
        manager.create_book(util::to_array<Book::ISBN_T>("C"));

        manager.import_book(util::to_array<Book::ISBN_T>("C"), 1000);
        auto book = manager.get_books_with_ISBN(util::to_array<Book::ISBN_T>("C"))[0];
        REQUIRE(book.quantity == 1000);

        REQUIRE(manager.buy_book(util::to_array<Book::ISBN_T>("C"), 10000) == false);
        REQUIRE(manager.buy_book(util::to_array<Book::ISBN_T>("C"), 123) == true);
        book = manager.get_books_with_ISBN(util::to_array<Book::ISBN_T>("C"))[0];
        REQUIRE(book.quantity == 877);
    }
    SECTION("Order of queries") {
        const std::string& CHAR =
            "!#$%&'()*+,-./"
            "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abcdefghijklmnopqrstuvwxyz{|}~";

        std::mt19937 gen(1234);
        std::vector<std::string> ISBNS;
        for (int i = 0; i < 10; i++) {
            std::string now;
            for (int _ = 0; _ < 20; _++) {
                now.push_back(CHAR[gen() % CHAR.size()]);
            }
            manager.create_book(util::to_array<Book::ISBN_T>(now));
            manager.modify_book_data(util::to_array<Book::ISBN_T>(now),
                                     gen_book(now, "", "DWRG", "KW1|KW4", 0, 0));

            ISBNS.push_back(now);
        }

        std::ranges::sort(ISBNS);

        auto result = manager.get_books_with_author(util::to_array<Book::AUTHOR_T>("DWRG"));
        assert(result.size() == ISBNS.size());
        for (int i = 0; i < result.size(); i++) {
            assert(result[i].ISBN == util::to_array<Book::ISBN_T>(ISBNS[i]));
        }

        auto result2 = manager.get_books_with_keyword(util::to_array<Book::KEYWORD_T>("KW1"));
        assert(result == result2);
    }
}

TEST_CASE("BooksManager Edge Cases", "[BooksManager]") {
    auto& manager = BooksManager::getInstance();
    manager.reset();

    for (int _ = 0; _ < 10; _++) {
        assert(manager.get_books_with_ISBN(Book::ISBN_T{}).empty());
        assert(manager.get_books_with_author(Book::AUTHOR_T{}).empty());
        assert(manager.get_books_with_keyword(Book::KEYWORD_T{}).empty());
        assert(manager.get_books_with_name(Book::BOOKNAME_T{}).empty());
    }
}