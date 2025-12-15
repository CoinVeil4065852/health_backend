#pragma once

#include "core/HealthBackend.hpp"

namespace httplib { class Server; }

void registerWaterRoutes(httplib::Server &svr, HealthBackend &backend);
