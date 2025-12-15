#include "../../include/routes/UserRoutes.hpp"
#include "../../third_party/json.hpp"
#include "../../include/routes/Helpers.hpp"

using json = nlohmann::ordered_json;

void registerUserRoutes(httplib::Server &svr, HealthBackend &backend) {
  svr.Get("/user/profile", [&backend](const httplib::Request &req, httplib::Response &res) {
    std::string token = getTokenFromAuthHeader(req);
    if (token.empty()) { json err; err["errorMessage"] = "Missing or invalid Authorization token"; res.status = 401; res.set_content(err.dump(), "application/json"); return; }

    UserProfile profile; if (!backend.getUserProfile(token, profile)) { json err; err["errorMessage"] = "Profile not found"; res.status = 404; res.set_content(err.dump(), "application/json"); return; }

    json out; out["id"] = profile.id; out["name"] = profile.name; out["gender"] = profile.gender; out["weightKg"] = profile.weightKg; out["heightM"] = profile.heightM; out["age"] = profile.age; res.status = 200; res.set_content(out.dump(), "application/json");
  });

  svr.Get("/user/bmi", [&backend](const httplib::Request &req, httplib::Response &res) {
    std::string token = getTokenFromAuthHeader(req);
    if (token.empty()) { json err; err["errorMessage"] = "Missing or invalid Authorization token"; res.status = 401; res.set_content(err.dump(), "application/json"); return; }

    double bmi = backend.getBMI(token); if (bmi <= 0.0) { json err; err["errorMessage"] = "Profile not found"; res.status = 404; res.set_content(err.dump(), "application/json"); return; }
    json out; out["bmi"] = bmi; res.status = 200; res.set_content(out.dump(), "application/json");
  });
}
