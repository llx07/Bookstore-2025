#include <jwt-cpp/jwt.h>

#include <nlohmann/json.hpp>

#include "Commands/UserCommands.hpp"
#include "Parser/FieldParser.hpp"
#include "UsersManager.hpp"
#include "crow.h"
#include "crow/middlewares/cors.h"

const std::string SECRET_KEY = "#ILove1dentityV-Di5rengeZhenh@owan$";

using json = nlohmann::json;

// 自己实现一个符合 Crow 中间件规范的 CORSHandler
struct CORSHandler {
    struct context {};  // 每个中间件都需要一个 context struct，即使是空的

    // 在处理请求之前被调用
    void before_handle(crow::request& req, crow::response& res, context& /*ctx*/) {
        CROW_LOG_INFO << "BEFORE_HANDLER_CALLED" << '\n';
        // 1. 设置允许跨源请求的来源
        //    在开发环境中，可以使用 "*" 允许所有来源
        //    在生产环境中，为了安全，应指定你的前端应用的 URL，例如 "http://localhost:5173"
        res.add_header("Access-Control-Allow-Origin", "*");

        // 2. 设置允许的 HTTP 方法
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");

        // 3. 设置允许的自定义请求头
        //    确保这里包含了前端发送的所有自定义头部，如 Content-Type 和 Authorization
        res.add_header("Access-Control-Allow-Headers",
                       "Content-Type, Authorization, Accept, X-Requested-With");
        // 4. (关键步骤) 处理 OPTIONS 预检请求
        //    如果请求方法是 OPTIONS，说明是预检请求，我们直接返回一个成功的空响应即可。
        //    浏览器收到这个响应后，就会发送真正的 POST 请求。
        CROW_LOG_INFO << "TYPE = " << method_name(req.method) << '\n';
        if (req.method == "OPTIONS"_method) {
            CROW_LOG_INFO << "RETURN 200 HERE" << '\n';
            res.code = 200;
            res.end();
        }
    }

    // 在处理请求之后被调用 (这里我们不需要做什么)
    void after_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/) {
        // No action needed
    }
};

int main() {
    crow::App<CORSHandler> app;
    CROW_ROUTE(app, "/")([]() { return "hello"; });
    app.port(10086).multithreaded().run();
    return 0;
}
