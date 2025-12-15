#pragma once

#include "httplib.h"
namespace health {
class HealthBackend;
}
namespace routes {
void registerSleepRoutes(httplib::Server &svr, health::HealthBackend &backend);
}
