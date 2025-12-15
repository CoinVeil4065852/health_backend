#pragma once

#include "core/HealthBackend.hpp"

namespace httplib { class Server; }

void registerCategoryRoutes(httplib::Server &svr, HealthBackend &backend);
