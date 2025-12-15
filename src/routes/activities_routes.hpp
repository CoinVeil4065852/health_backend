#pragma once

#include "httplib.h"
namespace health { class HealthBackend; }
namespace routes { void registerActivityRoutes(httplib::Server& svr, health::HealthBackend& backend); }
