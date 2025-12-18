#include <jwt-cpp/jwt.h>

#include <nlohmann/json.hpp>

#include "Commands/UserCommands.hpp"
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
                json err{{"message", "Invalid Argument"}};
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
                json err{{"message", "Invalid Argument"}};
                res.code = 400;
                res.write(err.dump());
                res.set_header("Content-Type", "application/json");
                res.end();
                return;
            }

            if (!j.contains("userid")) {
                json err{{"message", "Invalid Argument"}};
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
            if (util::toString(ctx.userid) == "<GUEST>") {
                res.code = 400;
                json j;
                j["message"] = "";
            }

            LogoutCommand().execute(ctx.userid, ctx.selected_id, std::cout);
        });
    app.port(10086).multithreaded().run();
    return 0;
}
