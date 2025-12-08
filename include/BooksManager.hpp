#ifndef BOOKSTORE_BOOKSMANAGER_HPP
#define BOOKSTORE_BOOKSMANAGER_HPP

#include <array>
#include <vector>

#include "BlockList.hpp"

// The data structure for a book
struct Book {
    static constexpr int ISBN_MAX_LEN = 20;
    static constexpr int BOOK_NAME_MAX_LEN = 60;
    static constexpr int AUTHOR_MAX_LEN = 60;
    static constexpr int KEYWORDS_MAX_LEN = 60;
    // One additional character for storing the \0.
    typedef std::array<char, ISBN_MAX_LEN + 1> ISBN_T;
    typedef std::array<char, BOOK_NAME_MAX_LEN + 1> BOOKNAME_T;
    typedef std::array<char, AUTHOR_MAX_LEN + 1> AUTHOR_T;
    typedef std::array<char, KEYWORDS_MAX_LEN + 1> KEYWORD_T;
    ISBN_T ISBN;
    BOOKNAME_T book_name;
    AUTHOR_T author;
    KEYWORD_T keywords;  // May have multiple keywords, separated with '|'
    int quantity;
    long long price;
    Book() : quantity(0), price(0) {
        ISBN.fill(0);
        book_name.fill(0);
        author.fill(0);
        keywords.fill(0);
    }
    bool operator==(const Book&) const = default;
};

// A singleton class that provides methods to manipulate the data of books.
class BooksManager {
public:
    // Returns the singleton for BooksManager.
    static BooksManager& getInstance();
    // Disables copy and assignment for singleton class.
    BooksManager(const BooksManager&) = delete;
    BooksManager& operator=(const BooksManager&) = delete;
    // Searches for books with given queries. Return books in the order of ascending ISBN.
    // Returns an empty vector if no book found.
    std::vector<Book> get_books_with_ISBN(const Book::ISBN_T& ISBN);
    std::vector<Book> get_books_with_name(const Book::BOOKNAME_T& name);
    std::vector<Book> get_books_with_author(const Book::AUTHOR_T& author);
    std::vector<Book> get_books_with_keyword(const Book::KEYWORD_T& keyword);
    std::vector<Book> get_all_books();

    // Creates a new book with only the info of ISBN.
    // The ISBN SHALL not exist before.
    void create_book(const Book::ISBN_T& ISBN);

    // Modify the data of the book with data_new
    void modify_book_data(const Book::ISBN_T& ISBN_before, const Book& data_new);
    void import_book(const Book::ISBN_T& ISBN, int quantity);
    // Returns true if the operation is success.
    bool buy_book(const Book::ISBN_T& ISBN, int quantity);

    // Helper function for testing.
    void reset();
    // get a book by id
    Book get_book_by_id(int id);
    // Helper function for converting ids into books.
    std::vector<Book> get_books_by_ids(const std::vector<int>& ids);
    // Returns the id of a book by ISBN. Returns 0 if no such book exists.
    int get_id_by_ISBN(const Book::ISBN_T& ISBN);

private:
    // The primary data for all books
    MemoryRiver<Book, 0> main_data;
    // Indexes for quickly searching books.
    BlockList<Book::ISBN_T, int> isbn_index;
    BlockList<Book::BOOKNAME_T, int> book_name_index;
    BlockList<Book::AUTHOR_T, int> author_index;
    // Note: only one keyword is stored in each key-value pair
    BlockList<Book::KEYWORD_T, int> keyword_index;

    // Helper function for erasing all data and indexes of the book
    void remove_data(const Book::ISBN_T& ISBN);
    // Helper function for writing all data and indexes of a book.
    void write_data(const Book& book);
    BooksManager();
    ~BooksManager() = default;
};

#endif  // BOOKSTORE_BOOKSMANAGER_HPP
