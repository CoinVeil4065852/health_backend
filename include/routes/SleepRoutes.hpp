#pragma once

#include "core/HealthBackend.hpp"

namespace httplib { class Server; }

void registerSleepRoutes(httplib::Server &svr, HealthBackend &backend);
