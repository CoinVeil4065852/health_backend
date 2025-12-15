#include "routes/activities_routes.hpp"
#include "json.hpp"
#include "helpers/Logger.hpp"
#include "server_utils.hpp"
#include "backend/HealthBackend.hpp"

using json = nlohmann::ordered_json;
using namespace health;

namespace routes {

void registerActivityRoutes(httplib::Server& svr, HealthBackend& backend) {
  svr.Post("/activities", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token; if (!server_utils::requireAuth(req, res, token)) return;
    try {
      json j = json::parse(req.body);
      if (!j.contains("datetime") || !j.contains("minutes") || !j.contains("intensity")) { json err; err["errorMessage"] = "Missing fields"; res.status = 400; res.set_content(err.dump(), "application/json"); return; }
      std::string datetime = j["datetime"].get<std::string>(); int minutes = j["minutes"].get<int>(); std::string intensity = j["intensity"].get<std::string>();
      bool ok = backend.addActivity(token, datetime, minutes, intensity); if (!ok) { json err; err["errorMessage"] = "Failed to add activity record"; res.status = 400; res.set_content(err.dump(), "application/json"); return; }
      auto records = backend.getAllActivity(token); if (records.empty()) { json err; err["errorMessage"] = "Internal error: no activity records after add"; res.status = 500; res.set_content(err.dump(), "application/json"); return; }
      std::size_t idx = records.size() - 1; const auto& a = records[idx]; json out; out["id"] = std::to_string(idx); out["datetime"] = a.datetime; out["minutes"] = a.minutes; out["intensity"] = a.intensity; res.status = 201; res.set_content(out.dump(), "application/json");
    } catch (const std::exception& e) { json err; err["errorMessage"] = std::string("Invalid JSON: ") + e.what(); res.status = 400; res.set_content(err.dump(), "application/json"); }
  });

  svr.Get("/activities", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token; if (!server_utils::requireAuth(req, res, token)) return; auto records = backend.getAllActivity(token); json arr = json::array(); for (std::size_t i = 0; i < records.size(); ++i) { const auto& a = records[i]; json ja; ja["id"] = std::to_string(i); ja["datetime"] = a.datetime; ja["minutes"] = a.minutes; ja["intensity"] = a.intensity; arr.push_back(ja); } res.status = 200; res.set_content(arr.dump(), "application/json");
  });

  svr.Patch(R"(/activities/(\d+))", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token; if (!server_utils::requireAuth(req, res, token)) return; std::string idStr = req.matches[1]; std::size_t index = 0; try { index = static_cast<std::size_t>(std::stoul(idStr)); } catch (...) { json err; err["errorMessage"] = "Invalid activity id"; res.status = 400; res.set_content(err.dump(), "application/json"); return; }
    try { json j = json::parse(req.body); auto records = backend.getAllActivity(token); if (index >= records.size()) { json err; err["errorMessage"] = "Record not found"; res.status = 404; res.set_content(err.dump(), "application/json"); return; } std::string newDatetime = records[index].datetime; int newMinutes = records[index].minutes; std::string newIntensity = records[index].intensity; if (j.contains("datetime")) newDatetime = j["datetime"].get<std::string>(); if (j.contains("minutes")) newMinutes = j["minutes"].get<int>(); if (j.contains("intensity")) newIntensity = j["intensity"].get<std::string>(); bool ok = backend.updateActivity(token, index, newDatetime, newMinutes, newIntensity); if (!ok) { json err; err["errorMessage"] = "Failed to update activity record"; res.status = 400; res.set_content(err.dump(), "application/json"); return; } json out; out["id"] = idStr; out["datetime"] = newDatetime; out["minutes"] = newMinutes; out["intensity"] = newIntensity; res.status = 200; res.set_content(out.dump(), "application/json"); } catch (const std::exception& e) { json err; err["errorMessage"] = std::string("Invalid JSON: ") + e.what(); res.status = 400; res.set_content(err.dump(), "application/json"); }
  });

  svr.Delete(R"(/activities/(\d+))", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token; if (!server_utils::requireAuth(req, res, token)) return; std::string idStr = req.matches[1]; std::size_t index = 0; try { index = static_cast<std::size_t>(std::stoul(idStr)); } catch (...) { json err; err["errorMessage"] = "Invalid activity id"; res.status = 400; res.set_content(err.dump(), "application/json"); return; } bool ok = backend.deleteActivity(token, index); if (!ok) { json err; err["errorMessage"] = "Record not found"; res.status = 404; res.set_content(err.dump(), "application/json"); return; } res.status = 204; res.set_content("", "application/json");
  });
}

} // namespace routes
