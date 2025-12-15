#include "../../include/server/ServerSetup.hpp"
#include "../../third_party/httplib.h"
#include "../../third_party/json.hpp"
#include "../../include/utils/Logger.hpp"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>

using json = nlohmann::ordered_json;

namespace server {

static std::mutex g_req_mtx;
static std::unordered_map<const httplib::Request*, std::chrono::steady_clock::time_point> g_req_start;

void initLoggerFromEnv() {
  const char* logFileEnv = std::getenv("LOG_FILE");
  std::string logFilePath = logFileEnv ? logFileEnv : "logs/server.log";
  const char* logLevelEnv = std::getenv("LOG_LEVEL");
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
}

void setupServerCommon(httplib::Server &svr) {
  // CORS preflight
  svr.Options(R"(.*)", [](const httplib::Request& req, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PATCH, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    res.set_header("Access-Control-Max-Age", "3600");
    res.status = 204;
  });

  // Exception handler
  svr.set_exception_handler([](const httplib::Request& req, httplib::Response& /*res*/, std::exception_ptr ep) {
    std::string origin = req.has_header("Origin") ? req.get_header_value("Origin") : "-";
    try {
      if (ep) std::rethrow_exception(ep);
    } catch (const std::exception& e) {
      util::Logger::error(std::string("Unhandled exception handling request: ") + req.method + " " + req.path +
                          " Origin:" + origin + " error: " + e.what());
    } catch (...) {
      util::Logger::error(std::string("Unhandled exception handling request: ") + req.method + " " + req.path +
                          " Origin:" + origin + " error: unknown");
    }
  });

  // Pre-routing: record start time and basic request info
  svr.set_pre_routing_handler([&](const httplib::Request& req, httplib::Response& /*res*/) -> httplib::Server::HandlerResponse {
    std::lock_guard<std::mutex> lk(g_req_mtx);
    g_req_start[&req] = std::chrono::steady_clock::now();
    auto origin = req.has_header("Origin") ? req.get_header_value("Origin") : "-";
    util::Logger::info(req.method + std::string(" ") + req.path + " Origin:" + origin);
    return httplib::Server::HandlerResponse::Unhandled;
  });

  // Post-routing: add CORS headers if missing and log duration
  svr.set_post_routing_handler([&](const httplib::Request& req, httplib::Response& res) {
    if (res.get_header_value("Access-Control-Allow-Origin").empty()) {
      res.set_header("Access-Control-Allow-Origin", "*");
    }
    if (res.get_header_value("Access-Control-Allow-Headers").empty()) {
      res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
    if (res.get_header_value("Access-Control-Allow-Methods").empty()) {
      res.set_header("Access-Control-Allow-Methods", "GET, POST, PATCH, DELETE, OPTIONS");
    }

    std::chrono::steady_clock::time_point start;
    {
      std::lock_guard<std::mutex> lk(g_req_mtx);
      auto it = g_req_start.find(&req);
      if (it != g_req_start.end()) {
        start = it->second;
        g_req_start.erase(it);
      }
    }
    if (start.time_since_epoch().count() > 0) {
      auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();
      util::Logger::info(req.method + std::string(" ") + req.path + " -> " + std::to_string(res.status) + " (" + std::to_string(dur) + " ms)");
    }
  });

  // httplib logging -> our Logger
  svr.set_logger([](const httplib::Request& req, const httplib::Response& /*res*/) {
    auto origin = req.has_header("Origin") ? req.get_header_value("Origin") : "-";
    util::Logger::debug(std::string("httplib log: ") + req.method + " " + req.path + " Origin:" + origin);
  });

  svr.set_error_logger([](const httplib::Error& err, const httplib::Request* req) {
    std::string path = req ? req->path : "-";
    util::Logger::warn(std::string("httplib error: ") + std::to_string(static_cast<int>(err)) + " path:" + path);
  });
}

void startServer(httplib::Server &svr) {
  std::cout << "--- Health Backend Server ---" << std::endl;
  std::cout << "Enter custom port (or 0 for default 8080): ";
  int portInput = 0;
  if (std::cin >> portInput && portInput > 0) {
    std::cout << "Starting on port " << portInput << "..." << std::endl;
    svr.listen("0.0.0.0", portInput);
  } else {
    std::cout << "Starting on default port 8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);
  }
  util::Logger::shutdown();
}

} // namespace server
