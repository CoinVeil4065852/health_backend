#pragma once

#include "core/HealthBackend.hpp"

namespace httplib { class Server; }

// Register all HTTP routes on the server using provided backend.
void registerRoutes(httplib::Server &svr, HealthBackend &backend);
