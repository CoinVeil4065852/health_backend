#pragma once

#include "httplib.h"

namespace health {
class HealthBackend;
}
namespace routes {
void registerWaterRoutes(httplib::Server &svr, health::HealthBackend &backend);
}
