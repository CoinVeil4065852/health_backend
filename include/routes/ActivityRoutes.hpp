#pragma once

#include "core/HealthBackend.hpp"

namespace httplib { class Server; }

void registerActivityRoutes(httplib::Server &svr, HealthBackend &backend);
