#include "BooksManager.hpp"

#include <algorithm>
#include <filesystem>
#include <ranges>

#include "Utils.hpp"

BooksManager& BooksManager::getInstance() {
    static BooksManager instance;
    return instance;
}
std::vector<Book> BooksManager::get_books_with_ISBN(const Book::ISBN_T& ISBN) {
    const std::vector<int> ids = isbn_index.query(ISBN);
    return get_books_by_ids(ids);
}
std::vector<Book> BooksManager::get_books_with_name(const Book::BOOKNAME_T& name) {
    const std::vector<int> ids = book_name_index.query(name);
    return get_books_by_ids(ids);
}
std::vector<Book> BooksManager::get_books_with_author(const Book::AUTHOR_T& author) {
    const std::vector<int> ids = author_index.query(author);
    return get_books_by_ids(ids);
}
std::vector<Book> BooksManager::get_books_with_keyword(const Book::KEYWORD_T& keyword) {
    const std::vector<int> ids = keyword_index.query(keyword);
    return get_books_by_ids(ids);
}
std::vector<Book> BooksManager::get_all_books() {
    const std::vector<int> ids = book_name_index.query_all();
    return get_books_by_ids(ids);
}
void BooksManager::create_book(const Book::ISBN_T& ISBN) {
    Book new_book;
    new_book.ISBN = ISBN;
    write_data(new_book);
}
void BooksManager::modify_book_data(const Book::ISBN_T& ISBN_before, const Book& data_new) {
    remove_data(ISBN_before);
    write_data(data_new);
}
void BooksManager::import_book(const Book::ISBN_T& ISBN, int quantity_imported) {
    int id = get_id_by_ISBN(ISBN);
    int quantity_now;
    main_data.read(quantity_now, id, offsetof(Book, quantity));
    main_data.update(quantity_now + quantity_imported, id, offsetof(Book, quantity));
}
bool BooksManager::buy_book(const Book::ISBN_T& ISBN, int quantity_bought) {
    int id = get_id_by_ISBN(ISBN);
    int quantity_now;
    main_data.read(quantity_now, id, offsetof(Book, quantity));
    if (quantity_now < quantity_bought) {
        return false;
    }
    main_data.update(quantity_now - quantity_bought, id, offsetof(Book, quantity));
    return true;
}
void BooksManager::reset() {
    this->~BooksManager();
    std::filesystem::remove("book_data");
    std::filesystem::remove("book_ISBN_index");
    std::filesystem::remove("book_name_index");
    std::filesystem::remove("book_author_index");
    std::filesystem::remove("book_keyword_index");
    new (this) BooksManager();
}
Book BooksManager::get_book_by_id(int id) {
    Book book_now;
    main_data.read(book_now, id);
    return book_now;
}

std::vector<Book> BooksManager::get_books_by_ids(const std::vector<int>& ids) {
    std::vector<Book> books;
    for (const int id : ids) {
        Book book_now;
        main_data.read(book_now, id);
        books.push_back(book_now);
    }
    // XXX(llx) maybe the memory limit is not enough for storing all books in a vector?
    std::ranges::sort(books, {}, &Book::ISBN);
    return books;
}
int BooksManager::get_id_by_ISBN(const Book::ISBN_T& ISBN) {
    const std::vector<int> ids = isbn_index.query(ISBN);
    assert(ids.size() <= 1);
    if (ids.empty()) return 0;
    return ids[0];
}
void BooksManager::remove_data(const Book::ISBN_T& ISBN) {
    int id = get_id_by_ISBN(ISBN);
    assert(id);
    Book book;
    main_data.read(book, id);
    isbn_index.erase(book.ISBN, id);
    book_name_index.erase(book.book_name, id);
    author_index.erase(book.author, id);
    auto keywords_list = util::split(book.keywords.data());
    for (const auto& keyword : keywords_list) {
        keyword_index.erase(util::to_array<Book::KEYWORD_T>(keyword), id);
    }
    main_data.erase(id);
}
void BooksManager::write_data(const Book& book) {
    if (get_id_by_ISBN(book.ISBN)) {
        throw std::runtime_error("ISBN Already Exists");
    }
    int id = main_data.write(book);
    isbn_index.insert(book.ISBN, id);
    book_name_index.insert(book.book_name, id);
    author_index.insert(book.author, id);
    auto keywords_list = util::split(book.keywords.data());
    for (const auto& keyword : keywords_list) {
        keyword_index.insert(util::to_array<Book::KEYWORD_T>(keyword), id);
    }
}

BooksManager::BooksManager() {
    main_data.initialise("book_data");
    isbn_index.initialise("book_ISBN_index");
    book_name_index.initialise("book_name_index");
    author_index.initialise("book_author_index");
    keyword_index.initialise("book_keyword_index");
}