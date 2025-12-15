#include "../../include/routes/CategoryRoutes.hpp"

#include "../../include/routes/Helpers.hpp"
#include "../../third_party/json.hpp"

using json = nlohmann::ordered_json;

void registerCategoryRoutes(httplib::Server& svr, HealthBackend& backend) {
  svr.Get("/category/list", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token = getTokenFromAuthHeader(req);
    if (token.empty()) {
      json err;
      err["errorMessage"] = "Missing or invalid Authorization token";
      res.status = 401;
      res.set_content(err.dump(), "application/json");
      return;
    }
    auto cats = backend.getOtherCategories(token);
    json arr = json::array();
    for (const auto& name : cats) {
      json jc;
      jc["id"] = name;
      jc["categoryName"] = name;
      arr.push_back(jc);
    }
    res.status = 200;
    res.set_content(arr.dump(), "application/json");
  });

  svr.Post("/category/create", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token = getTokenFromAuthHeader(req);
    if (token.empty()) {
      json err;
      err["errorMessage"] = "Missing or invalid Authorization token";
      res.status = 401;
      res.set_content(err.dump(), "application/json");
      return;
    }
    try {
      json j = json::parse(req.body);
      if (!j.contains("categoryName")) {
        json err;
        err["errorMessage"] = "Missing categoryName";
        res.status = 400;
        res.set_content(err.dump(), "application/json");
        return;
      }
      std::string name = j["categoryName"].get<std::string>();
      bool ok = backend.createCategory(token, name);
      if (!ok) {
        json err;
        err["errorMessage"] = "Category already exists or invalid name";
        res.status = 400;
        res.set_content(err.dump(), "application/json");
        return;
      }
      json out;
      out["id"] = name;
      out["categoryName"] = name;
      res.status = 201;
      res.set_content(out.dump(), "application/json");
    } catch (const std::exception& e) {
      json err;
      err["errorMessage"] = std::string("Invalid JSON: ") + e.what();
      res.status = 400;
      res.set_content(err.dump(), "application/json");
    }
  });

  svr.Delete(R"(/category/([^/]+)$)", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token = getTokenFromAuthHeader(req);
    if (token.empty()) {
      json err;
      err["errorMessage"] = "Missing or invalid Authorization token";
      res.status = 401;
      res.set_content(err.dump(), "application/json");
      return;
    }
    std::string categoryId = req.matches[1];
    bool ok = backend.deleteCategory(token, categoryId);
    if (!ok) {
      json err;
      err["errorMessage"] = "Category not found";
      res.status = 404;
      res.set_content(err.dump(), "application/json");
      return;
    }
    res.status = 204;
    res.set_content("", "application/json");
  });

  svr.Get(R"(/category/([^/]+)/list)", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token = getTokenFromAuthHeader(req);
    if (token.empty()) {
      json err;
      err["errorMessage"] = "Missing or invalid Authorization token";
      res.status = 401;
      res.set_content(err.dump(), "application/json");
      return;
    }
    std::string categoryId = req.matches[1];
    auto records = backend.getOtherRecords(token, categoryId);
    if (records.empty()) {
      json err;
      err["errorMessage"] = "Category not found or no items";
      res.status = 404;
      res.set_content(err.dump(), "application/json");
      return;
    }
    json arr = json::array();
    for (std::size_t i = 0; i < records.size(); ++i) {
      const auto& r = records[i];
      json jr;
      jr["id"] = std::to_string(i);
      jr["datetime"] = r.datetime;
      jr["note"] = r.note;
      arr.push_back(jr);
    }
    res.status = 200;
    res.set_content(arr.dump(), "application/json");
  });

  svr.Post(R"(/category/([^/]+)/add)", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token = getTokenFromAuthHeader(req);
    if (token.empty()) {
      json err;
      err["errorMessage"] = "Missing or invalid Authorization token";
      res.status = 401;
      res.set_content(err.dump(), "application/json");
      return;
    }
    std::string categoryId = req.matches[1];
    try {
      json j = json::parse(req.body);
      if (!j.contains("datetime") || !j.contains("note")) {
        json err;
        err["errorMessage"] = "Missing datetime or note";
        res.status = 400;
        res.set_content(err.dump(), "application/json");
        return;
      }
      std::string datetime = j["datetime"].get<std::string>();
      std::string note = j["note"].get<std::string>();
      bool ok = backend.addOtherRecord(token, categoryId, datetime, 0.0, note);
      if (!ok) {
        json err;
        err["errorMessage"] = "Category not found or invalid data";
        res.status = 400;
        res.set_content(err.dump(), "application/json");
        return;
      }
      auto records = backend.getOtherRecords(token, categoryId);
      std::size_t idx = records.size() - 1;
      const auto& r = records[idx];
      json out;
      out["id"] = std::to_string(idx);
      out["categoryId"] = categoryId;
      out["datetime"] = r.datetime;
      out["note"] = r.note;
      res.status = 201;
      res.set_content(out.dump(), "application/json");
    } catch (const std::exception& e) {
      json err;
      err["errorMessage"] = std::string("Invalid JSON: ") + e.what();
      res.status = 400;
      res.set_content(err.dump(), "application/json");
    }
  });

  svr.Patch(R"(/category/([^/]+)/([^/]+))", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token = getTokenFromAuthHeader(req);
    if (token.empty()) {
      json err;
      err["errorMessage"] = "Missing or invalid Authorization token";
      res.status = 401;
      res.set_content(err.dump(), "application/json");
      return;
    }
    std::string categoryId = req.matches[1];
    std::string itemIdStr = req.matches[2];
    std::size_t index = 0;
    try {
      json j = json::parse(req.body);
      auto records = backend.getOtherRecords(token, categoryId);
      if (index >= records.size()) {
        json err;
        err["errorMessage"] = "Category or item not found";
        res.status = 404;
        res.set_content(err.dump(), "application/json");
        return;
      }
      std::string newDatetime = records[index].datetime;
      std::string newNote = records[index].note;
      double value = records[index].value;
      if (j.contains("datetime")) newDatetime = j["datetime"].get<std::string>();
      if (j.contains("note")) newNote = j["note"].get<std::string>();
      bool ok = backend.updateOtherRecord(token, categoryId, index, newDatetime, value, newNote);
      if (!ok) {
        json err;
        err["errorMessage"] = "Failed to update category item";
        res.status = 400;
        res.set_content(err.dump(), "application/json");
        return;
      }
      json out;
      out["id"] = std::to_string(index);
      out["categoryId"] = categoryId;
      out["datetime"] = newDatetime;
      out["note"] = newNote;
      res.status = 200;
      res.set_content(out.dump(), "application/json");
    } catch (const std::exception& e) {
      json err;
      err["errorMessage"] = std::string("Invalid JSON: ") + e.what();
      res.status = 400;
      res.set_content(err.dump(), "application/json");
    }
  });

  svr.Delete(R"(/category/([^/]+)/([^/]+))", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token = getTokenFromAuthHeader(req);
    if (token.empty()) {
      json err;
      err["errorMessage"] = "Missing or invalid Authorization token";
      res.status = 401;
      res.set_content(err.dump(), "application/json");
      return;
    }
    std::string categoryId = req.matches[1];
    std::string itemIdStr = req.matches[2];
    std::size_t index = 0;
    bool ok = backend.deleteOtherRecord(token, categoryId, index);
    if (!ok) {
      json err;
      err["errorMessage"] = "Category or item not found";
      res.status = 404;
      res.set_content(err.dump(), "application/json");
      return;
    }
    res.status = 204;
    res.set_content("", "application/json");
  });
}
