#pragma once

#include "core/HealthBackend.hpp"

namespace httplib {
class Server;
}

void registerUserRoutes(httplib::Server& svr, HealthBackend& backend);
