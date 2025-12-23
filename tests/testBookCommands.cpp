// #include <catch2/catch_test_macros.hpp>
// #include <catch2/matchers/catch_matchers_string.hpp>
// #include <sstream>
//
// #include "BooksManager.hpp"
// #include "Commands/BookCommands.hpp"
// #include "UsersManager.hpp"
// #include "Utils.hpp"
//
// static void setup() {
//     BooksManager::getInstance().reset();
//     UsersManager::getInstance().reset();
//
//     User user;
//     user.userid = util::toArray<User::USERID_T>("customer");
//     user.password = util::toArray<User::PASSWORD_T>("123");
//     user.privilege = 1;
//     UsersManager::getInstance().addUser(user);
//
//     user.userid = util::toArray<User::USERID_T>("salesman");
//     user.password = util::toArray<User::PASSWORD_T>("456");
//     user.privilege = 3;
//     UsersManager::getInstance().addUser(user);
//
//     BooksManager::getInstance().createBook(util::toArray<Book::ISBN_T>("ISBN1"));
//     Book book_data;
//     book_data.ISBN = util::toArray<Book::ISBN_T>("ISBN1");
//     book_data.book_name = util::toArray<Book::BOOKNAME_T>("Bookname");
//     book_data.author = util::toArray<Book::AUTHOR_T>("Author");
//     book_data.keywords = util::toArray<Book::KEYWORD_T>("KW1|KW2");
//     book_data.price = 12345;
//     book_data.quantity = 50;
//     BooksManager::getInstance().modifyBookData(util::toArray<Book::ISBN_T>("ISBN1"), book_data);
//
//     BooksManager::getInstance().createBook(util::toArray<Book::ISBN_T>("ISBN2"));
//     book_data.ISBN = util::toArray<Book::ISBN_T>("ISBN2");
//     book_data.book_name = util::toArray<Book::BOOKNAME_T>("Bookname2");
//     book_data.author = util::toArray<Book::AUTHOR_T>("Author2");
//     book_data.keywords = util::toArray<Book::KEYWORD_T>("KW2|KW3");
//     book_data.price = 782;
//     book_data.quantity = 100;
//     BooksManager::getInstance().modifyBookData(util::toArray<Book::ISBN_T>("ISBN2"), book_data);
// }
//
// TEST_CASE("BookCommands: SHOW command", "[BookCommands]") {
//     setup();
//
//     std::stringstream ss;
//     Session session(ss);
//     SECTION("Need login to use") {
//         ShowCommand cmd;
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("privilege not enough to
//                             operate"));
//     }
//     SECTION("Show all books") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         REQUIRE(session.getPrivilege() == 3);
//         ShowCommand cmd;
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//
//         std::vector<std::string> lines;
//         std::string line_now;
//         while (getline(ss, line_now)) {
//             lines.push_back(line_now);
//         }
//
//         REQUIRE(lines.size() == 2);
//         REQUIRE(lines[0] == "ISBN1\tBookname\tAuthor\tKW1|KW2\t123.45\t50");
//         REQUIRE(lines[1] == "ISBN2\tBookname2\tAuthor2\tKW2|KW3\t7.82\t100");
//     }
//     SECTION("Show books with keyword") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         REQUIRE(session.getPrivilege() == 3);
//         ShowCommand cmd;
//         cmd.keyword = util::toArray<Book::KEYWORD_T>("KW2");
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//
//         std::vector<std::string> lines;
//         std::string line_now;
//         while (getline(ss, line_now)) {
//             lines.push_back(line_now);
//         }
//
//         REQUIRE(lines.size() == 2);
//         REQUIRE(lines[0] == "ISBN1\tBookname\tAuthor\tKW1|KW2\t123.45\t50");
//         REQUIRE(lines[1] == "ISBN2\tBookname2\tAuthor2\tKW2|KW3\t7.82\t100");
//     }
//     SECTION("Show books with Author") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         REQUIRE(session.getPrivilege() == 3);
//         ShowCommand cmd;
//         cmd.author = util::toArray<Book::AUTHOR_T>("Author2");
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//
//         std::vector<std::string> lines;
//         std::string line_now;
//         while (getline(ss, line_now)) {
//             lines.push_back(line_now);
//         }
//
//         REQUIRE(lines.size() == 1);
//         REQUIRE(lines[0] == "ISBN2\tBookname2\tAuthor2\tKW2|KW3\t7.82\t100");
//     }
// }
//
// TEST_CASE("BookCommands: BUY command", "[BookCommands]") {
//     setup();
//     std::stringstream ss;
//     Session session(ss);
//     SECTION("Need login to use") {
//         BuyCommand cmd{util::toArray<Book::ISBN_T>("ISBN3"), 100};
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("privilege not enough to
//                             operate"));
//     }
//     SECTION("ISBN not exist") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         BuyCommand cmd{util::toArray<Book::ISBN_T>("ISBN3"), 100};
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("ISBN doesn't exist"));
//     }
//     SECTION("Buy too much") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         BuyCommand cmd{util::toArray<Book::ISBN_T>("ISBN1"), 100};
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("more than quantity in storage"));
//     }
//     SECTION("Correct Usage") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         BuyCommand cmd{util::toArray<Book::ISBN_T>("ISBN1"), 50};
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//         auto data = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN1"));
//         auto book = session.books_manager.getBookById(data);
//         REQUIRE(book.quantity == 0);
//
//         std::vector<std::string> lines;
//         std::string line_now;
//         while (getline(ss, line_now)) {
//             lines.push_back(line_now);
//         }
//
//         REQUIRE(lines.size() == 1);
//         REQUIRE(lines[0] == "6172.50");
//     }
// }
//
// TEST_CASE("BookCommands: SELECT command", "[BookCommands]") {
//     setup();
//     std::stringstream ss;
//     Session session(ss);
//     SECTION("Need privilege 3 to use") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         SelectCommand cmd{util::toArray<Book::ISBN_T>("ISBN3")};
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("privilege not enough to
//                             operate"));
//     }
//     SECTION("Book Not Exists") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         SelectCommand cmd{util::toArray<Book::ISBN_T>("ISBN3")};
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//         int id = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN3"));
//         REQUIRE(id != 0);
//         REQUIRE(session.getSelectedBook() == id);
//     }
//     SECTION("Book Exists") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         int id = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN2"));
//         REQUIRE(id != 0);
//         SelectCommand cmd{util::toArray<Book::ISBN_T>("ISBN2")};
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//         REQUIRE(session.getSelectedBook() == id);
//     }
// }
//
// TEST_CASE("BookCommands: MODIFY", "[BooksCommands]") {
//     setup();
//     std::stringstream ss;
//     Session session(ss);
//     SECTION("Need privilege 3 to use") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         ModifyCommand cmd{};
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("privilege not enough to
//                             operate"));
//     }
//     SECTION("Not Selected") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         ModifyCommand cmd{};
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("selected book is empty"));
//     }
//     SECTION("ISBN duplicate") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         int id = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN2"));
//         session.setSelectedBook(id);
//         ModifyCommand cmd{};
//         cmd.new_ISBN = util::toArray<Book::ISBN_T>("ISBN2");
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("ISBN mustn't be the same"));
//     }
//     SECTION("Empty modification") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         int id = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN2"));
//         session.setSelectedBook(id);
//         ModifyCommand cmd{};
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//     }
//     SECTION("Modify Price") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         int id = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN2"));
//         session.setSelectedBook(id);
//         ModifyCommand cmd{};
//         cmd.new_price = 1000;
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//
//         auto data = session.books_manager.getBookById(id);
//         REQUIRE(data.price == 1000);
//     }
//     SECTION("Modify ISBN") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         int id = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN2"));
//         session.setSelectedBook(id);
//         ModifyCommand cmd{};
//         cmd.new_ISBN = util::toArray<Book::ISBN_T>("ISBN3");
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//
//         id = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN2"));
//         REQUIRE(id == 0);
//         id = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN3"));
//         REQUIRE(id != 0);
//     }
// }
//
// TEST_CASE("BookCommands: IMPORT command", "[BooksCommand]") {
//     setup();
//     std::stringstream ss;
//     Session session(ss);
//     SECTION("privilege at least 3") {
//         session.loginPush(util::toArray<User::USERID_T>("customer"));
//         ImportCommand cmd{100, 2000};
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("privilege not enough to
//                             operate"));
//     }
//     SECTION("Not Selected") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         ImportCommand cmd{100, 2000};
//         REQUIRE_THROWS_WITH(cmd.execute(session, TODO, TODO),
//                             Catch::Matchers::ContainsSubstring("selected book is empty"));
//     }
//     SECTION("Modify Price") {
//         session.loginPush(util::toArray<User::USERID_T>("salesman"));
//         int id = session.books_manager.getIdByISBN(util::toArray<Book::ISBN_T>("ISBN2"));
//         auto data_bef = session.books_manager.getBookById(id);
//         session.setSelectedBook(id);
//         ImportCommand cmd{100, 2000};
//         REQUIRE_NOTHROW(cmd.execute(session, TODO, TODO));
//
//         auto data_aft = session.books_manager.getBookById(id);
//         REQUIRE(data_aft.quantity - data_bef.quantity == 100);
//     }
// }