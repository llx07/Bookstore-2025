#include "BooksManager.hpp"

#include <algorithm>
#include <filesystem>
#include <ranges>

#include "Utils.hpp"

BooksManager& BooksManager::getInstance() {
    static BooksManager instance;
    return instance;
}
std::vector<Book> BooksManager::getBooksWithISBN(const Book::ISBN_T& ISBN) {
    const std::vector<int> ids = isbn_index.query(ISBN);
    return getBooksByIds(ids);
}
std::vector<Book> BooksManager::getBooksWithName(const Book::BOOKNAME_T& name) {
    const std::vector<int> ids = book_name_index.query(name);
    return getBooksByIds(ids);
}
std::vector<Book> BooksManager::getBooksWithAuthor(const Book::AUTHOR_T& author) {
    const std::vector<int> ids = author_index.query(author);
    return getBooksByIds(ids);
}
std::vector<Book> BooksManager::getBooksWithKeyword(const Book::KEYWORD_T& keyword) {
    const std::vector<int> ids = keyword_index.query(keyword);
    return getBooksByIds(ids);
}
std::vector<Book> BooksManager::getAllBooks() {
    const std::vector<int> ids = book_name_index.queryAll();
    return getBooksByIds(ids);
}
void BooksManager::createBook(const Book::ISBN_T& ISBN) {
    Book new_book;
    new_book.ISBN = ISBN;
    writeData(new_book);
}
void BooksManager::modifyBookData(const Book::ISBN_T& ISBN_before, const Book& data_new) {
    removeData(ISBN_before);
    writeData(data_new);
}
void BooksManager::importBook(const Book::ISBN_T& ISBN, int quantity_imported) {
    int id = getIdByISBN(ISBN);
    int quantity_now;
    main_data.read(quantity_now, id, offsetof(Book, quantity));
    main_data.update(quantity_now + quantity_imported, id, offsetof(Book, quantity));
}
bool BooksManager::buyBook(const Book::ISBN_T& ISBN, int quantity_bought) {
    int id = getIdByISBN(ISBN);
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
Book BooksManager::getBookById(int id) {
    Book book_now;
    main_data.read(book_now, id);
    return book_now;
}

std::vector<Book> BooksManager::getBooksByIds(const std::vector<int>& ids) {
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
int BooksManager::getIdByISBN(const Book::ISBN_T& ISBN) {
    const std::vector<int> ids = isbn_index.query(ISBN);
    assert(ids.size() <= 1);
    if (ids.empty()) return 0;
    return ids[0];
}
void BooksManager::removeData(const Book::ISBN_T& ISBN) {
    int id = getIdByISBN(ISBN);
    if(!id)return;
    Book book;
    main_data.read(book, id);
    isbn_index.erase(book.ISBN, id);
    book_name_index.erase(book.book_name, id);
    author_index.erase(book.author, id);
    auto keywords_list = util::split(book.keywords.data());
    for (const auto& keyword : keywords_list) {
        keyword_index.erase(util::toArray<Book::KEYWORD_T>(keyword), id);
    }
    main_data.erase(id);
}
void BooksManager::writeData(const Book& book) {
    if (getIdByISBN(book.ISBN)) {
        throw std::runtime_error("ISBN Already Exists");
    }
    int id = main_data.write(book);
    isbn_index.insert(book.ISBN, id);
    book_name_index.insert(book.book_name, id);
    author_index.insert(book.author, id);
    auto keywords_list = util::split(book.keywords.data());
    for (const auto& keyword : keywords_list) {
        keyword_index.insert(util::toArray<Book::KEYWORD_T>(keyword), id);
    }
}

BooksManager::BooksManager() {
    main_data.initialise("book_data");
    isbn_index.initialise("book_ISBN_index");
    book_name_index.initialise("book_name_index");
    author_index.initialise("book_author_index");
    keyword_index.initialise("book_keyword_index");
}