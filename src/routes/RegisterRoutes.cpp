#include "../../include/routes/AuthRoutes.hpp"
#include "../../include/routes/HealthRoutes.hpp"
#include "../../include/routes/UserRoutes.hpp"
#include "../../include/routes/WaterRoutes.hpp"
#include "../../include/routes/SleepRoutes.hpp"
#include "../../include/routes/ActivityRoutes.hpp"
#include "../../include/routes/CategoryRoutes.hpp"

void registerRoutes(httplib::Server &svr, HealthBackend &backend) {
  registerHealthRoutes(svr, backend);
  registerAuthRoutes(svr, backend);
  registerUserRoutes(svr, backend);
  registerWaterRoutes(svr, backend);
  registerSleepRoutes(svr, backend);
  registerActivityRoutes(svr, backend);
  registerCategoryRoutes(svr, backend);
}
