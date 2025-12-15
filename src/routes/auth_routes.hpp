#pragma once

#include "httplib.h"

namespace health {
class HealthBackend;
}

namespace routes {
void registerAuthRoutes(httplib::Server& svr, health::HealthBackend& backend);
} // namespace routes
