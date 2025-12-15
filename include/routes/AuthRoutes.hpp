#pragma once

#include "core/HealthBackend.hpp"

namespace httplib { class Server; }

void registerAuthRoutes(httplib::Server &svr, HealthBackend &backend);
