#pragma once

#include "httplib.h"
namespace health { class HealthBackend; }
namespace routes { void registerCategoryRoutes(httplib::Server& svr, health::HealthBackend& backend); }
