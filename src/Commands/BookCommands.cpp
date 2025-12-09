#include "Commands/BookCommands.hpp"

void ShowCommand::execute(Session& session) {
    if (session.get_privilege() < 1) {
        throw ExecutionException("show error: privilege not enough to operate.");
    }
    auto& os = session.out_stream;

    auto output = [&os](const auto& str) {
        for (auto it = str.begin(); *it; it++) {
            os << (*it);
        }
    };

    std::vector<Book> books;
    if (ISBN.has_value()) {
        books = session.books_manager.get_books_with_ISBN(ISBN.value());
    } else if (name.has_value()) {
        books = session.books_manager.get_books_with_name(name.value());
    } else if (author.has_value()) {
        books = session.books_manager.get_books_with_author(author.value());
    } else if (keyword.has_value()) {
        books = session.books_manager.get_books_with_keyword(keyword.value());
    } else {
        books = session.books_manager.get_all_books();
    }

    for (auto& book : books) {
        output(book.ISBN);
        os << '\t';
        output(book.book_name);
        os << '\t';
        output(book.author);
        os << '\t';
        output(book.keywords);
        os << '\t';
        os << book.price / 100 << '.' << std::setw(2) << std::setfill('0') << book.price % 100;
        os << '\t';
        os << book.quantity;
        os << '\n';
    }
}

void BuyCommand::execute(Session& session) {
    if (session.get_privilege() < 1) {
        throw ExecutionException("buy error: privilege not enough to operate.");
    }
    auto result = session.books_manager.get_books_with_ISBN(ISBN);
    if (result.empty()) {
        throw ExecutionException("buy error: book with ISBN doesn't exist");
    }
    auto book = result.front();
    // FIXME(llx) ambiguous description: can we buy book beyond quantity?
    if (book.quantity < quantity) {
        throw ExecutionException("buy error: quantity is more than quantity in storage");
    }

    session.books_manager.buy_book(ISBN, quantity);

    long long money_need = book.price * quantity;
    auto& os = session.out_stream;
    os << money_need / 100 << '.' << std::setw(2) << std::setfill('0') << money_need % 100 << '\n';
}
BuyCommand::BuyCommand(const Book::ISBN_T& _ISBN, int _quantity)
    : ISBN(_ISBN), quantity(_quantity) {}

void SelectCommand::execute(Session& session) {
    if (session.get_privilege() < 3) {
        throw ExecutionException("select error: privilege not enough to operate.");
    }

    auto result = session.books_manager.get_id_by_ISBN(ISBN);
    if (!result) {
        session.books_manager.create_book(ISBN);
        result = session.books_manager.get_id_by_ISBN(ISBN);
    }
    session.set_selected_book(result);
}
SelectCommand::SelectCommand(const Book::ISBN_T& _ISBN) : ISBN(_ISBN) {}

void ModifyCommand::execute(Session& session) {
    if (session.get_privilege() < 3) {
        throw ExecutionException("modify error: privilege not enough to operate.");
    }
    if (!session.get_selected_book()) {
        throw ExecutionException("modify error: selected book is empty");
    }
    Book book_data = session.books_manager.get_book_by_id(session.get_selected_book());
    if (new_ISBN && new_ISBN == book_data.ISBN) {
        throw ExecutionException("modify error: new ISBN mustn't be the same with before");
    }
    auto old_ISBN = book_data.ISBN;
    if (new_ISBN) {
        book_data.ISBN = new_ISBN.value();
    }
    if (new_name) {
        book_data.book_name = new_name.value();
    }
    if (new_author) {
        book_data.author = new_author.value();
    }
    if (new_keyword) {
        book_data.keywords = new_keyword.value();
    }
    if (new_price) {
        book_data.price = new_price.value();
    }
    session.books_manager.modify_book_data(old_ISBN, book_data);
}

void ImportCommand::execute(Session& session) {
    if (session.get_privilege() < 3) {
        throw ExecutionException("import error: privilege not enough to operate.");
    }
    if (!session.get_selected_book()) {
        throw ExecutionException("modify error: selected book is empty");
    }
    Book book_data = session.books_manager.get_book_by_id(session.get_selected_book());
    session.books_manager.import_book(book_data.ISBN, quantity);
}
ImportCommand::ImportCommand(int _quantity, long long _total_cost)
    : quantity(_quantity), total_cost(_total_cost) {}