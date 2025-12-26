#include "Commands/BookCommands.hpp"

#include "Utils.hpp"

void ShowCommand::execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) {
    std::string command_str = "show";
    if (ISBN.has_value()) command_str = command_str + " -ISBN=" + util::toString(ISBN.value());
    if (name.has_value()) command_str = command_str + " -name=" + util::toString(name.value());
    if (author.has_value())
        command_str = command_str + " -author=" + util::toString(author.value());
    if (keyword.has_value())
        command_str = command_str + " -keyword=" + util::toString(keyword.value());

    int log_id = LogManager::getInstance().addOperationLog(
        util::getTimestamp(), current_userid, usr_mgr.getUserByUserid(current_userid).privilege,
        util::toArray<Log::OPERATION_T>(command_str));
    if (usr_mgr.getUserByUserid(current_userid).privilege < 1) {
        throw ExecutionException("show error: privilege not enough to operate.");
    }

    auto output = [&os](const auto& str) {
        for (auto it = str.begin(); *it; it++) {
            os << (*it);
        }
    };

    std::vector<Book> books;
    if (ISBN.has_value()) {
        books = bk_mgr.getBooksWithISBN(ISBN.value());
    } else if (name.has_value()) {
        books = bk_mgr.getBooksWithName(name.value());
    } else if (author.has_value()) {
        books = bk_mgr.getBooksWithAuthor(author.value());
    } else if (keyword.has_value()) {
        books = bk_mgr.getBooksWithKeyword(keyword.value());
    } else {
        books = bk_mgr.getAllBooks();
    }
    if (books.empty()) {
        os << "\n";
    } else {
        for (auto& book : books) {
            output(book.ISBN);
            os << '\t';
            output(book.book_name);
            os << '\t';
            output(book.author);
            os << '\t';
            output(book.keywords);
            os << '\t';
            util::outputDecimal(os, book.price);
            os << '\t';
            os << book.quantity;
            os << '\n';
        }
    }

    LogManager::getInstance().markOperationSuccess(log_id);
}

void BuyCommand::execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) {
    std::string command_str =
        "buy -ISBN=" + util::toString(ISBN) + " -quantity=" + std::to_string(quantity);
    int log_id = LogManager::getInstance().addOperationLog(
        util::getTimestamp(), current_userid, usr_mgr.getUserByUserid(current_userid).privilege,
        util::toArray<Log::OPERATION_T>(command_str));

    if (usr_mgr.getUserByUserid(current_userid).privilege < 1) {
        throw ExecutionException("buy error: privilege not enough to operate.");
    }
    auto result = bk_mgr.getBooksWithISBN(ISBN);
    if (result.empty()) {
        throw ExecutionException("buy error: book with ISBN doesn't exist");
    }
    auto book = result.front();
    if (book.quantity < quantity) {
        throw ExecutionException("buy error: quantity is more than quantity in storage");
    }

    bk_mgr.buyBook(ISBN, quantity);

    long long money_need = book.price * quantity;
    util::outputDecimal(os, money_need);
    os << "\n";
    log_mgr.addFinanceLog(util::getTimestamp(), current_userid, money_need);
    LogManager::getInstance().markOperationSuccess(log_id);
}
BuyCommand::BuyCommand(const Book::ISBN_T& _ISBN, int _quantity)
    : ISBN(_ISBN), quantity(_quantity) {}

void SelectCommand::execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) {
    std::string command_str = "select -ISBN=" + util::toString(ISBN);
    int log_id = LogManager::getInstance().addOperationLog(
        util::getTimestamp(), current_userid, usr_mgr.getUserByUserid(current_userid).privilege,
        util::toArray<Log::OPERATION_T>(command_str));
    if (usr_mgr.getUserByUserid(current_userid).privilege < 3) {
        throw ExecutionException("select error: privilege not enough to operate.");
    }

    auto result = bk_mgr.getIdByISBN(ISBN);
    if (!result) {
        bk_mgr.createBook(ISBN);
        result = bk_mgr.getIdByISBN(ISBN);
    }
    current_bookid = result;
    LogManager::getInstance().markOperationSuccess(log_id);
}
SelectCommand::SelectCommand(const Book::ISBN_T& _ISBN) : ISBN(_ISBN) {}

void ModifyCommand::execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) {
    std::string command_str = "modify";
    if (new_ISBN.has_value())
        command_str = command_str + " -ISBN=" + util::toString(new_ISBN.value());
    if (new_name.has_value())
        command_str = command_str + " -name=" + util::toString(new_name.value());
    if (new_author.has_value())
        command_str = command_str + " -author=" + util::toString(new_author.value());
    if (new_keyword.has_value())
        command_str = command_str + " -keyword=" + util::toString(new_keyword.value());
    if (new_price.has_value())
        command_str = command_str + " -price=" + std::to_string(new_price.value());
    int log_id = LogManager::getInstance().addOperationLog(
        util::getTimestamp(), current_userid, usr_mgr.getUserByUserid(current_userid).privilege,
        util::toArray<Log::OPERATION_T>(command_str));

    if (usr_mgr.getUserByUserid(current_userid).privilege < 3) {
        throw ExecutionException("modify error: privilege not enough to operate.");
    }
    if (!current_bookid) {
        throw ExecutionException("modify error: selected book is empty");
    }
    Book book_data = bk_mgr.getBookById(current_bookid);
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
    bk_mgr.modifyBookData(old_ISBN, book_data);

    LogManager::getInstance().markOperationSuccess(log_id);
}

void ImportCommand::execute(User::USERID_T& current_userid, int& current_bookid, std::ostream& os) {
    std::string command_str = "import -quantity=" + std::to_string(quantity) +
                              " -total_cost=" + std::to_string(total_cost);
    int log_id = LogManager::getInstance().addOperationLog(
        util::getTimestamp(), current_userid, usr_mgr.getUserByUserid(current_userid).privilege,
        util::toArray<Log::OPERATION_T>(command_str));
    if (usr_mgr.getUserByUserid(current_userid).privilege < 3) {
        throw ExecutionException("import error: privilege not enough to operate.");
    }
    if (!current_bookid) {
        throw ExecutionException("modify error: selected book is empty");
    }
    Book book_data = bk_mgr.getBookById(current_bookid);
    bk_mgr.importBook(book_data.ISBN, quantity);
    log_mgr.addFinanceLog(util::getTimestamp(), current_userid, -total_cost);

    LogManager::getInstance().markOperationSuccess(log_id);
}
ImportCommand::ImportCommand(int _quantity, long long _total_cost)
    : quantity(_quantity), total_cost(_total_cost) {}