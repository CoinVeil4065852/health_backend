#include "routes/auth_routes.hpp"
#include "json.hpp"
#include "helpers/Logger.hpp"
#include "server_utils.hpp"
#include "backend/HealthBackend.hpp"

using json = nlohmann::ordered_json;
using namespace health;

namespace routes {

void registerAuthRoutes(httplib::Server& svr, HealthBackend& backend) {
  svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
    json j;
    j["status"] = "ok";
    j["message"] = "health_backend server running";
    res.status = 200;
    res.set_content(j.dump(), "application/json");
  });

  svr.Post("/register", [&backend](const httplib::Request& req, httplib::Response& res) {
    try {
      json j = json::parse(req.body);
      if (!j.contains("name") || !j.contains("password") || !j.contains("age") || !j.contains("weightKg") ||
          !j.contains("heightM") || !j.contains("gender")) {
        json err;
        err["errorMessage"] = "Missing or invalid fields";
        res.status = 400;
        res.set_content(err.dump(), "application/json");
        return;
      }

      std::string name = j["name"].get<std::string>();
      std::string password = j["password"].get<std::string>();
      int age = j["age"].get<int>();
      double weightKg = j["weightKg"].get<double>();
      double heightM = j["heightM"].get<double>();
      std::string gender = j["gender"].get<std::string>();

      bool ok = backend.registerUser(name, age, weightKg, heightM, password, gender);
      if (!ok) {
        json err;
        err["errorMessage"] = "User already exists";
        res.status = 409;
        res.set_content(err.dump(), "application/json");
        return;
      }

      std::string token = backend.login(name, password);
      if (token == "INVALID") {
        json err;
        err["errorMessage"] = "Internal error when generating token";
        res.status = 500;
        res.set_content(err.dump(), "application/json");
        return;
      }

      json out;
      out["token"] = token;
      res.status = 201;
      res.set_content(out.dump(), "application/json");
      util::Logger::info(std::string("POST /register: user=") + name + " token=" + token);
    } catch (const std::exception& e) {
      json err;
      err["errorMessage"] = std::string("Invalid JSON: ") + e.what();
      res.status = 400;
      res.set_content(err.dump(), "application/json");
    }
  });

  svr.Post("/login", [&backend](const httplib::Request& req, httplib::Response& res) {
    try {
      json j = json::parse(req.body);
      if (!j.contains("name") || !j.contains("password")) {
        json err;
        err["errorMessage"] = "Missing name or password";
        res.status = 400;
        res.set_content(err.dump(), "application/json");
        return;
      }

      std::string name = j["name"].get<std::string>();
      std::string password = j["password"].get<std::string>();

      std::string token = backend.login(name, password);
      if (token == "INVALID") {
        json err;
        err["errorMessage"] = "Invalid name or password";
        res.status = 401;
        res.set_content(err.dump(), "application/json");
        util::Logger::warn(std::string("POST /login failed: user=") + name);
        return;
      }

      json out;
      out["token"] = token;
      res.status = 200;
      res.set_content(out.dump(), "application/json");
      util::Logger::info(std::string("POST /login: user=") + name + " token=" + token);
    } catch (const std::exception& e) {
      json err;
      err["errorMessage"] = std::string("Invalid JSON: ") + e.what();
      res.status = 400;
      res.set_content(err.dump(), "application/json");
    }
  });

  svr.Get("/user/profile", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token;
    if (!server_utils::requireAuth(req, res, token)) return;

    UserProfile profile;
    if (!backend.getUserProfile(token, profile)) {
      json err;
      err["errorMessage"] = "Profile not found";
      res.status = 404;
      res.set_content(err.dump(), "application/json");
      return;
    }

    json out;
    out["id"] = profile.id;
    out["name"] = profile.name;
    out["gender"] = profile.gender;
    out["weightKg"] = profile.weightKg;
    out["heightM"] = profile.heightM;
    out["age"] = profile.age;

    res.status = 200;
    res.set_content(out.dump(), "application/json");
  });

  svr.Get("/user/bmi", [&backend](const httplib::Request& req, httplib::Response& res) {
    std::string token;
    if (!server_utils::requireAuth(req, res, token)) return;

    double bmi = backend.getBMI(token);
    if (bmi <= 0.0) {
      json err;
      err["errorMessage"] = "Profile not found";
      res.status = 404;
      res.set_content(err.dump(), "application/json");
      return;
    }

    json out;
    out["bmi"] = bmi;
    res.status = 200;
    res.set_content(out.dump(), "application/json");
  });
}

} // namespace routes
