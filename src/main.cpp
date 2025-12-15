// server.cpp

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "../include/core/HealthBackend.hpp"
#include "../include/utils/Logger.hpp"
#include "../third_party/httplib.h"
#include "../third_party/json.hpp"

using json = nlohmann::ordered_json;

// Routes are registered from separate module
#include "../include/routes/Routes.hpp"
// Server setup helpers
#include "../include/server/ServerSetup.hpp"

int main() {
  HealthBackend backend;
  httplib::Server svr;

  // initialize logger and server hooks
  server::initLoggerFromEnv();
  server::setupServerCommon(svr);

  // register routes from the routes module
  registerRoutes(svr, backend);

  // start (blocks)
  server::startServer(svr);

  return 0;
}