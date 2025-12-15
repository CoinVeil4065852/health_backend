#pragma once

#include "../third_party/httplib.h"

namespace httplib { class Server; }

namespace server {

// Initialize the global logger from environment variables `LOG_FILE` and `LOG_LEVEL`.
void initLoggerFromEnv();

// Configure server-wide handlers (CORS, exception handler, logging hooks, pre/post routing hooks).
void setupServerCommon(httplib::Server &svr);

// Prompt for (or detect) port and start the server (blocks until server stops).
void startServer(httplib::Server &svr);

} // namespace server
