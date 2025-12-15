#pragma once

#include "core/HealthBackend.hpp"

namespace httplib {
class Server;
}

void registerHealthRoutes(httplib::Server& svr, HealthBackend& backend);
