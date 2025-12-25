#include <jwt-cpp/jwt.h>

#include <nlohmann/json.hpp>
#include <sstream>

#include "Commands/BookCommands.hpp"
#include "Commands/UserCommands.hpp"
#include "Commands/LogCommands.hpp"
#include "Parser/FieldParser.hpp"
#include "UsersManager.hpp"
#include "crow.h"
#include "crow/middlewares/cors.h"

const std::string SECRET_KEY = "#ILove1dentityV-Di5rengeZhenh@owan$";

using json = nlohmann::json;

struct AuthMiddleware {
    struct context {
        User::USERID_T userid;
        int selected_id;
    };
    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        CROW_LOG_INFO << "BEFORE_HANDLER_CALLED (AUTH)" << '\n';
        const auto& auth_header = req.get_header_value("Authorization");
        if (auth_header.empty()) {
            ctx.userid = util::toArray<User::USERID_T>("<GUEST>");
            ctx.selected_id = 0;
            return;
        }
        CROW_LOG_DEBUG << "auth_header: " << auth_header << '\n';
        if (auth_header.substr(0, 7) != "Bearer ") {
            res.code = 401;
            res.write("Unauthorized: Invalid authorization header.");
            res.end();
            return;
        }

        try {
            auto token = auth_header.substr(7);
            auto verifier = jwt::verify()
                                .with_issuer("linlexiao")
                                .allow_algorithm(jwt::algorithm::hs256{SECRET_KEY});
            auto decoded = jwt::decode(token);
            verifier.verify(decoded);

            ctx.userid =
                util::toArray<User::USERID_T>(decoded.get_payload_claim("userid").as_string());
            ctx.selected_id = decoded.get_payload_claim("selected_id").as_integer();
        } catch (const std::exception& e) {
            res.code = 401;
            res.write(e.what());
            res.end();
            return;
        }
    }
    void after_handle(crow::request&, crow::response&, context&) {}
};

int main() {
    crow::App<crow::CORSHandler, AuthMiddleware> app;
    // app.loglevel(crow::LogLevel::Debug);
    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors.global()
        .origin("*")
        .headers(
            "origin, x-requested-with, accept, access-control-allow-origin, authorization, "
            "content-type")
        .methods("POST"_method, "GET"_method, "PUT"_method, "DELETE"_method, "PATCH"_method,
                 "OPTIONS"_method);

    CROW_ROUTE(app, "/")([]() { return "Hello, Crow!"; });

    CROW_ROUTE(app, "/api/v1/auth/login")
        .methods("POST"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);

            CROW_LOG_INFO << "auth/login, " << "userid: " << util::toString(ctx.userid)
                          << ", select_id: " << ctx.selected_id << '\n';
            // 1. Content-Type 必须要是 json
            if (auto content_type = req.get_header_value("Content-Type");
                content_type.find("application/json") == std::string::npos) {
                json err{{"message", "Only JSON body is supported"}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
            // 2. 检查 json 编码是否正确
            json j;
            try {
                j = json::parse(req.body);
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
            try {
                auto userid = parse_userid(j["userid"]);
                auto cmd = j.contains("password") ? std::make_unique<SwitchUserCommand>(
                                                        userid, parse_password(j["password"]))
                                                  : std::make_unique<SwitchUserCommand>(userid);

                cmd->execute(ctx.userid, ctx.selected_id, std::cout);

                const auto time = jwt::date::clock::now();
                auto token =
                    jwt::create()
                        .set_issuer("linlexiao")
                        .set_payload_claim("userid", jwt::claim(j["userid"].get<std::string>()))
                        .set_payload_claim("selected_id", jwt::claim(picojson::value(int64_t{0})))
                        .set_issued_at(time)
                        .set_expires_at(time + std::chrono::hours(24))
                        .sign(jwt::algorithm::hs256{SECRET_KEY});
                // TODO(llx) store tokens locally to release expired tokens
                json j;
                j["message"] = "Success.";
                j["access_token"] = token;
                const auto& user = UsersManager::getInstance().getUserByUserid(userid);
                j["username"] = util::toString(user.username);
                j["privilege"] = user.privilege;
                res.code = 200;
                res.write(j.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });

    CROW_ROUTE(app, "/api/v1/auth/logout")
        .methods("POST"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            // TODO(llx) add this token to blacklist.
            if (util::toString(ctx.userid) == "<GUEST>") {
                res.code = 400;
                json j;
                j["message"] = "Cannot log out guest user.";
                res.write(j.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }

            try {
                LogoutCommand().execute(ctx.userid, ctx.selected_id, std::cout);
                json j;
                j["message"] = "Success.";
                res.write(j.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
            }
        });

    CROW_ROUTE(app, "/api/v1/users")
        .methods("POST"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            if (auto content_type = req.get_header_value("Content-Type");
                content_type.find("application/json") == std::string::npos) {
                json err{{"message", "Only JSON body is supported"}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
            json j;
            try {
                j = json::parse(req.body);
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }

            if (!j.contains("privilege")) {
                try {
                    auto userid = parse_userid(j["userid"]);
                    auto password = parse_password(j["password"]);
                    auto username = parse_username(j["username"]);
                    RegisterCommand(userid, password, username)
                        .execute(ctx.userid, ctx.selected_id, std::cout);
                    json j;
                    j["message"] = "Success.";
                    res.code = 200;
                    res.write(j.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                } catch (const std::exception& e) {
                    json err{{"message", e.what()}};
                    res.code = 400;
                    res.write(err.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                }
            } else {
                try {
                    auto userid = parse_userid(j["userid"]);
                    auto password = parse_password(j["password"]);
                    auto username = parse_username(j["username"]);
                    int privilege = j["privilege"].get<int>();
                    AddUserCommand(userid, password, privilege, username)
                        .execute(ctx.userid, ctx.selected_id, std::cout);
                    json j;
                    j["message"] = "Success.";
                    res.code = 200;
                    res.write(j.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                } catch (const std::exception& e) {
                    json err{{"message", e.what()}};
                    res.code = 400;
                    res.write(err.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                }
            }
        });
    CROW_ROUTE(app, "/api/v1/users/<string>/password")
        .methods("PATCH"_method)(
            [&app](const crow::request& req, crow::response& res, const std::string& userid_str) {
                auto& ctx = app.get_context<AuthMiddleware>(req);
                if (auto content_type = req.get_header_value("Content-Type");
                    content_type.find("application/json") == std::string::npos) {
                    json err{{"message", "Only JSON body is supported"}};
                    res.code = 400;
                    res.write(err.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                    return;
                }
                json j;
                try {
                    j = json::parse(req.body);
                } catch (const std::exception& e) {
                    json err{{"message", e.what()}};
                    res.code = 400;
                    res.write(err.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                    return;
                }

                try {
                    auto new_password = parse_password(j["new_password"]);
                    auto userid = parse_userid(userid_str);
                    if (j.contains("current_password")) {
                        auto current_password = parse_password(j["current_password"]);
                        ChangePasswordCommand(userid, current_password, new_password)
                            .execute(ctx.userid, ctx.selected_id, std::cout);
                    } else {
                        ChangePasswordCommand(userid, new_password)
                            .execute(ctx.userid, ctx.selected_id, std::cout);
                    }
                    json j;
                    j["message"] = "Success.";
                    res.code = 200;
                    res.write(j.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                    return;
                } catch (const std::exception& e) {
                    json err{{"message", e.what()}};
                    res.code = 400;
                    res.write(err.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                }
            });
    CROW_ROUTE(app, "/api/v1/users/<string>")
        .methods("DELETE"_method)(
            [&app](const crow::request& req, crow::response& res, const std::string& userid_str) {
                auto& ctx = app.get_context<AuthMiddleware>(req);
                try {
                    auto userid = parse_userid(userid_str);
                    DeleteUserCommand(userid).execute(ctx.userid, ctx.selected_id, std::cout);
                    json j;
                    j["message"] = "Success.";
                    res.code = 200;
                    res.write(j.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                    return;
                } catch (const std::exception& e) {
                    json err{{"message", e.what()}};
                    res.code = 400;
                    res.write(err.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                    return;
                }
            });

    CROW_ROUTE(app, "/api/v1/books")
        .methods("GET"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);

            try {
                const char* isbn_ptr = req.url_params.get("isbn");
                const char* name_ptr = req.url_params.get("name");
                const char* author_ptr = req.url_params.get("author");
                const char* keyword_ptr = req.url_params.get("keyword");

                std::string isbn = isbn_ptr ? isbn_ptr : "";
                std::string name = name_ptr ? name_ptr : "";
                std::string author = author_ptr ? author_ptr : "";
                std::string keyword = keyword_ptr ? keyword_ptr : "";

                int param_count = 0;
                if (isbn_ptr) {
                    ++param_count;
                }
                if (name_ptr) {
                    ++param_count;
                }
                if (author_ptr) {
                    ++param_count;
                }
                if (keyword_ptr) {
                    ++param_count;
                }

                if (param_count > 1) {
                    json err;
                    err["message"] = "Too many parameters.";
                    res.code = 400;
                    res.write(err.dump());
                    res.set_header("Content-Type", "application/json");
                    res.end();
                    return;
                }

                ShowCommand cmd;
                if (isbn_ptr) {
                    cmd.ISBN = parseISBN(isbn);
                }
                if (name_ptr) {
                    cmd.name = parseBookName(name);
                }
                if (author_ptr) {
                    cmd.author = parseAuthor(author);
                }
                if (keyword_ptr) {
                    cmd.keyword = parseKeyword(keyword);
                }

                std::ostringstream oss;
                cmd.execute(ctx.userid, ctx.selected_id, oss);
                std::string data = oss.str();
                auto lines = util::split(data, '\n');
                json json_res = json::array();
                for (const std::string& line : lines) {
                    if (line.empty()) {
                        continue;
                    }
                    auto fields = util::split(line, '\t');
                    try {
                        json book;
                        if (fields.size() < 6) continue;
                        book["isbn"] = fields[0];
                        book["name"] = fields[1];
                        book["author"] = fields[2];
                        book["keyword"] = fields[3];
                        book["price"] = fields[4];
                        book["quantity"] = fields[5];
                        json_res.push_back(book);
                    } catch (const std::exception& e) {
                        CROW_LOG_WARNING << "show book error: " << e.what() << '\n';
                        continue;
                    }
                }
                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.write(json_res.dump());
                res.end();
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });

    CROW_ROUTE(app, "/api/v1/users/select/<string>")
        .methods("POST"_method)([&app](const crow::request& req, crow::response& res,
                                       const std::string& isbn_str) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            try {
                const Book::ISBN_T& isbn = parseISBN(isbn_str);
                SelectCommand(isbn).execute(ctx.userid, ctx.selected_id, std::cout);
                const auto time = jwt::date::clock::now();
                auto token =
                    jwt::create()
                        .set_issuer("linlexiao")
                        .set_payload_claim("userid", jwt::claim(util::toString(ctx.userid)))
                        .set_payload_claim("selected_id",
                                           jwt::claim(picojson::value(int64_t{ctx.selected_id})))
                        .set_issued_at(time)
                        .set_expires_at(time + std::chrono::hours(24))
                        .sign(jwt::algorithm::hs256{SECRET_KEY});
                json j;
                j["access_token"] = token;
                j["selected_id"] = ctx.selected_id;
                res.code = 200;
                res.write(j.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });

    CROW_ROUTE(app, "/api/v1/books/buy")
        .methods("POST"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            if (auto content_type = req.get_header_value("Content-Type");
                content_type.find("application/json") == std::string::npos) {
                json err{{"message", "Only JSON body is supported"}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
            json j;
            try {
                j = json::parse(req.body);
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }

            try {
                auto isbn = parseISBN(j["isbn"]);
                auto quantity = parseQuantity(j["quantity"]);
                std::ostringstream oss;
                BuyCommand(isbn, quantity).execute(ctx.userid, ctx.selected_id, oss);

                res.code = 200;
                json j;
                j["message"] = oss.str();

                res.write(j.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });
    CROW_ROUTE(app, "/api/v1/books/modify")
        .methods("PATCH"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            if (auto content_type = req.get_header_value("Content-Type");
                content_type.find("application/json") == std::string::npos) {
                json err{{"message", "Only JSON body is supported"}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
            json j;
            try {
                j = json::parse(req.body);
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }

            try {
                ModifyCommand cmd;
                if (j.contains("isbn")) cmd.new_ISBN = parseISBN(j["isbn"]);
                if (j.contains("name")) cmd.new_name = parseBookName(j["name"]);
                if (j.contains("author")) cmd.new_author = parseAuthor(j["author"]);
                if (j.contains("keyword")) cmd.new_keyword = parseKeyword(j["keyword"]);
                if (j.contains("price")) cmd.new_price = parsePrice(j["price"]);

                cmd.execute(ctx.userid, ctx.selected_id, std::cout);

                res.code = 200;
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });
    CROW_ROUTE(app, "/api/v1/books/import")
        .methods("POST"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            if (auto content_type = req.get_header_value("Content-Type");
                content_type.find("application/json") == std::string::npos) {
                json err{{"message", "Only JSON body is supported"}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
            json j;
            try {
                j = json::parse(req.body);
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }

            try {
                auto quantity = parseQuantity(j["quantity"]);
                auto total_cost = parsePrice(j["total_cost"]);
                std::ostringstream oss;
                ImportCommand(quantity, total_cost).execute(ctx.userid, ctx.selected_id, std::cout);
                res.code = 200;
                json j;
                res.write(j.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });

    CROW_ROUTE(app, "/api/v1/logs/show_finance")
        .methods("GET"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            try {
                if(req.url_params.get("count")){
                    int count = parseCount(req.url_params.get("count"));
                    ShowFinanceCommand cmd(count);
                    std::ostringstream oss;
                    cmd.execute(ctx.userid, ctx.selected_id, oss);
                    res.write(oss.str());
                    res.end();
                    return;
                }
                ShowFinanceCommand cmd;
                std::ostringstream oss;
                cmd.execute(ctx.userid, ctx.selected_id, oss);
                res.write(oss.str());
                res.end();
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });

    CROW_ROUTE(app, "/api/v1/logs/report_finance")
        .methods("GET"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            try {
                ReportFinanceCommand cmd;
                std::ostringstream oss;
                cmd.execute(ctx.userid, ctx.selected_id, oss);
                res.write(oss.str());
                res.end();
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });

    CROW_ROUTE(app, "/api/v1/logs/report_employee")
        .methods("GET"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            try {
                ReportEmployeeCommand cmd;
                std::ostringstream oss;
                cmd.execute(ctx.userid, ctx.selected_id, oss);
                res.write(oss.str());
                res.end();
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });

    CROW_ROUTE(app, "/api/v1/logs/all")
        .methods("GET"_method)([&app](const crow::request& req, crow::response& res) {
            auto& ctx = app.get_context<AuthMiddleware>(req);
            try {
                LogCommand cmd;
                std::ostringstream oss;
                cmd.execute(ctx.userid, ctx.selected_id, oss);
                res.write(oss.str());
                res.end();
            } catch (const std::exception& e) {
                json err{{"message", e.what()}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }
        });

    app.port(10086).multithreaded().run();
    return 0;
}
