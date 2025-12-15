// server.cpp

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "backend/HealthBackend.hpp"
#include "helpers/Logger.hpp"
#include "httplib.h"
#include "json.hpp"
#include "routes/activities_routes.hpp"
#include "routes/auth_routes.hpp"
#include "routes/categories_routes.hpp"
#include "routes/sleeps_routes.hpp"
#include "routes/waters_routes.hpp"
#include "server_utils.hpp"

using json = nlohmann::ordered_json;
using namespace health;

// Route registration is moved to smaller modules in src/routes/

int main() {
    HealthBackend backend;
    httplib::Server svr;

    // Initialize logger -------------------------------
    const char *logFileEnv = std::getenv("LOG_FILE");
    std::string logFilePath = logFileEnv ? logFileEnv : "logs/server.log";
    const char *logLevelEnv = std::getenv("LOG_LEVEL");
    util::LogLevel level = util::LogLevel::Info;
    if (logLevelEnv) {
        std::string s = logLevelEnv;
        if (s == "DEBUG")
            level = util::LogLevel::Debug;
        else if (s == "WARN")
            level = util::LogLevel::Warning;
        else if (s == "ERROR")
            level = util::LogLevel::Error;
        else
            level = util::LogLevel::Info;
    }
    util::Logger::init(logFilePath, level);
    // --------------------------------------------------

    // ===== NEW: CORS 設定（前端在別的 Port/Domain 時也能用） =====
    server_utils::setupCORS(svr);

    // Log exceptions
    svr.set_exception_handler([](const httplib::Request &req, httplib::Response &res,
                                 std::exception_ptr ep) {
        (void)res; // we don't modify response here
        std::string origin = req.has_header("Origin") ? req.get_header_value("Origin") : "-";
        try {
            if (ep)
                std::rethrow_exception(ep);
        } catch (const std::exception &e) {
            util::Logger::error(std::string("Unhandled exception handling request: ") + req.method +
                                " " + req.path + " Origin:" + origin + " error: " + e.what());
        } catch (...) {
            util::Logger::error(std::string("Unhandled exception handling request: ") + req.method +
                                " " + req.path + " Origin:" + origin + " error: unknown");
        }
    });

    // Inject CORS headers and log request info via a post-routing hook
    svr.set_post_routing_handler([&](const httplib::Request &req, httplib::Response &res) {
        // Inject standard CORS headers
        server_utils::addCORSHeaders(res);
        // Log basic request info (method, path, status)
        util::Logger::info(req.method + std::string(" ") + req.path + " -> " +
                           std::to_string(res.status));
    });

    // Set httplib server-level logging to route through our Logger
    svr.set_logger([](const httplib::Request &req, const httplib::Response &res) {
        (void)res;
        auto origin = req.has_header("Origin") ? req.get_header_value("Origin") : "-";
        util::Logger::debug(std::string("httplib log: ") + req.method + " " + req.path +
                            " Origin:" + origin);
    });
    svr.set_error_logger([](const httplib::Error &err, const httplib::Request *req) {
        std::string path = req ? req->path : "-";
        util::Logger::warn(std::string("httplib error: ") + std::to_string(static_cast<int>(err)) +
                           " path:" + path);
    });

    // Register routes from modules
    routes::registerAuthRoutes(svr, backend);
    routes::registerWaterRoutes(svr, backend);
    routes::registerSleepRoutes(svr, backend);
    routes::registerActivityRoutes(svr, backend);
    routes::registerCategoryRoutes(svr, backend);

    util::Logger::info("Server started at http://0.0.0.0:8080");
    svr.listen("0.0.0.0", 8080);

    util::Logger::shutdown();

    return 0;
}
