#include "../../include/routes/WaterRoutes.hpp"
#include "../../third_party/json.hpp"
#include "../../include/routes/Helpers.hpp"

using json = nlohmann::ordered_json;

void registerWaterRoutes(httplib::Server &svr, HealthBackend &backend) {
  svr.Post("/waters", [&backend](const httplib::Request &req, httplib::Response &res) {
    std::string token = getTokenFromAuthHeader(req); if (token.empty()) { json err; err["errorMessage"] = "Missing or invalid Authorization token"; res.status = 401; res.set_content(err.dump(), "application/json"); return; }
    try {
      json j = json::parse(req.body); if (!j.contains("datetime") || !j.contains("amountMl")) { json err; err["errorMessage"] = "Missing datetime or amountMl"; res.status = 400; res.set_content(err.dump(), "application/json"); return; }
      std::string datetime = j["datetime"].get<std::string>(); double amount = j["amountMl"].get<double>();
      bool ok = backend.addWater(token, datetime, amount); if (!ok) { json err; err["errorMessage"] = "Failed to add water record"; res.status = 400; res.set_content(err.dump(), "application/json"); return; }
      auto records = backend.getAllWater(token); if (records.empty()) { json err; err["errorMessage"] = "Internal error: no records after add"; res.status = 500; res.set_content(err.dump(), "application/json"); return; }
      std::size_t idx = records.size() - 1; const auto &r = records[idx]; json out; out["id"] = std::to_string(idx); out["datetime"] = r.datetime; out["amountMl"] = r.amountMl; res.status = 201; res.set_content(out.dump(), "application/json");
    } catch (const std::exception &e) { json err; err["errorMessage"] = std::string("Invalid JSON: ") + e.what(); res.status = 400; res.set_content(err.dump(), "application/json"); }
  });

  svr.Get("/waters", [&backend](const httplib::Request &req, httplib::Response &res) {
    std::string token = getTokenFromAuthHeader(req); if (token.empty()) { json err; err["errorMessage"] = "Missing or invalid Authorization token"; res.status = 401; res.set_content(err.dump(), "application/json"); return; }
    auto records = backend.getAllWater(token); json arr = json::array(); for (std::size_t i = 0; i < records.size(); ++i) { const auto &r = records[i]; json jr; jr["id"] = std::to_string(i); jr["datetime"] = r.datetime; jr["amountMl"] = r.amountMl; arr.push_back(jr); }
    res.status = 200; res.set_content(arr.dump(), "application/json");
  });

  svr.Patch(R"(/waters/(\d+))", [&backend](const httplib::Request &req, httplib::Response &res) {
    std::string token = getTokenFromAuthHeader(req); if (token.empty()) { json err; err["errorMessage"] = "Missing or invalid Authorization token"; res.status = 401; res.set_content(err.dump(), "application/json"); return; }
    std::string idStr = req.matches[1]; std::size_t index = 0; try { index = static_cast<std::size_t>(std::stoul(idStr)); } catch (...) { json err; err["errorMessage"] = "Invalid water id"; res.status = 400; res.set_content(err.dump(), "application/json"); return; }
    try {
      json j = json::parse(req.body);
      auto records = backend.getAllWater(token); if (index >= records.size()) { json err; err["errorMessage"] = "Record not found"; res.status = 404; res.set_content(err.dump(), "application/json"); return; }
      std::string newDatetime = records[index].datetime; double newAmount = records[index].amountMl; if (j.contains("datetime")) newDatetime = j["datetime"].get<std::string>(); if (j.contains("amountMl")) newAmount = j["amountMl"].get<double>();
      bool ok = backend.updateWater(token, index, newDatetime, newAmount); if (!ok) { json err; err["errorMessage"] = "Failed to update water record"; res.status = 400; res.set_content(err.dump(), "application/json"); return; }
      json out; out["id"] = idStr; out["datetime"] = newDatetime; out["amountMl"] = newAmount; res.status = 200; res.set_content(out.dump(), "application/json");
    } catch (const std::exception &e) { json err; err["errorMessage"] = std::string("Invalid JSON: ") + e.what(); res.status = 400; res.set_content(err.dump(), "application/json"); }
  });

  svr.Delete(R"(/waters/(\d+))", [&backend](const httplib::Request &req, httplib::Response &res) {
    std::string token = getTokenFromAuthHeader(req); if (token.empty()) { json err; err["errorMessage"] = "Missing or invalid Authorization token"; res.status = 401; res.set_content(err.dump(), "application/json"); return; }
    std::string idStr = req.matches[1]; std::size_t index = 0; try { index = static_cast<std::size_t>(std::stoul(idStr)); } catch (...) { json err; err["errorMessage"] = "Invalid water id"; res.status = 400; res.set_content(err.dump(), "application/json"); return; }
    bool ok = backend.deleteWater(token, index); if (!ok) { json err; err["errorMessage"] = "Record not found"; res.status = 404; res.set_content(err.dump(), "application/json"); return; }
    res.status = 204; res.set_content("", "application/json");
  });
}
