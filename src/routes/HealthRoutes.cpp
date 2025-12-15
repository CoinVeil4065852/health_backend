#include "../../include/routes/HealthRoutes.hpp"

#include "../../include/routes/Helpers.hpp"
#include "../../third_party/json.hpp"

using json = nlohmann::ordered_json;

void registerHealthRoutes(httplib::Server& svr, HealthBackend&) {
  svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
    json j;
    j["status"] = "ok";
    j["message"] = "health_backend server running";
    res.status = 200;
    res.set_content(j.dump(), "application/json");
  });
}
