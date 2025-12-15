#include "routes/sleeps_routes.hpp"
#include "backend/HealthBackend.hpp"
#include "helpers/Logger.hpp"
#include "json.hpp"
#include "server_utils.hpp"

using json = nlohmann::ordered_json;
using namespace health;

namespace routes {

void registerSleepRoutes(httplib::Server &svr, HealthBackend &backend) {
    svr.Post("/sleeps", [&backend](const httplib::Request &req, httplib::Response &res) {
        std::string token;
        if (!server_utils::requireAuth(req, res, token))
            return;
        try {
            json j = json::parse(req.body);
            if (!j.contains("datetime") || !j.contains("hours")) {
                json err;
                err["errorMessage"] = "Missing datetime or hours";
                res.status = 400;
                res.set_content(err.dump(), "application/json");
                return;
            }
            std::string datetime = j["datetime"].get<std::string>();
            double hours = j["hours"].get<double>();
            bool ok = backend.addSleep(token, datetime, hours);
            if (!ok) {
                json err;
                err["errorMessage"] = "Failed to add sleep record";
                res.status = 400;
                res.set_content(err.dump(), "application/json");
                util::Logger::warn(std::string("POST /sleeps failed: token=") + token +
                                   " hours=" + std::to_string(hours));
                return;
            }
            auto records = backend.getAllSleep(token);
            if (records.empty()) {
                json err;
                err["errorMessage"] = "Internal error: no sleep records after add";
                res.status = 500;
                res.set_content(err.dump(), "application/json");
                return;
            }
            std::size_t idx = records.size() - 1;
            const auto &r = records[idx];
            json out;
            out["id"] = std::to_string(idx);
            out["datetime"] = r.datetime;
            out["hours"] = r.hours;
            res.status = 201;
            res.set_content(out.dump(), "application/json");
        } catch (const std::exception &e) {
            json err;
            err["errorMessage"] = std::string("Invalid JSON: ") + e.what();
            res.status = 400;
            res.set_content(err.dump(), "application/json");
        }
    });

    svr.Get("/sleeps", [&backend](const httplib::Request &req, httplib::Response &res) {
        std::string token;
        if (!server_utils::requireAuth(req, res, token))
            return;
        auto records = backend.getAllSleep(token);
        json arr = json::array();
        for (std::size_t i = 0; i < records.size(); ++i) {
            const auto &r = records[i];
            json jr;
            jr["id"] = std::to_string(i);
            jr["datetime"] = r.datetime;
            jr["hours"] = r.hours;
            arr.push_back(jr);
        }
        res.status = 200;
        res.set_content(arr.dump(), "application/json");
    });

    svr.Patch(R"(/sleeps/(\d+))", [&backend](const httplib::Request &req, httplib::Response &res) {
        std::string token;
        if (!server_utils::requireAuth(req, res, token))
            return;
        std::string idStr = req.matches[1];
        std::size_t index = 0;
        try {
            index = static_cast<std::size_t>(std::stoul(idStr));
        } catch (...) {
            json err;
            err["errorMessage"] = "Invalid sleep id";
            res.status = 400;
            res.set_content(err.dump(), "application/json");
            return;
        }
        try {
            json j = json::parse(req.body);
            auto records = backend.getAllSleep(token);
            if (index >= records.size()) {
                json err;
                err["errorMessage"] = "Record not found";
                res.status = 404;
                res.set_content(err.dump(), "application/json");
                return;
            }
            std::string newDatetime = records[index].datetime;
            double newHours = records[index].hours;
            if (j.contains("datetime"))
                newDatetime = j["datetime"].get<std::string>();
            if (j.contains("hours"))
                newHours = j["hours"].get<double>();
            bool ok = backend.updateSleep(token, index, newDatetime, newHours);
            if (!ok) {
                json err;
                err["errorMessage"] = "Failed to update sleep record";
                res.status = 400;
                res.set_content(err.dump(), "application/json");
                return;
            }
            json out;
            out["id"] = idStr;
            out["datetime"] = newDatetime;
            out["hours"] = newHours;
            res.status = 200;
            res.set_content(out.dump(), "application/json");
        } catch (const std::exception &e) {
            json err;
            err["errorMessage"] = std::string("Invalid JSON: ") + e.what();
            res.status = 400;
            res.set_content(err.dump(), "application/json");
        }
    });

    svr.Delete(R"(/sleeps/(\d+))", [&backend](const httplib::Request &req, httplib::Response &res) {
        std::string token;
        if (!server_utils::requireAuth(req, res, token))
            return;
        std::string idStr = req.matches[1];
        std::size_t index = 0;
        try {
            index = static_cast<std::size_t>(std::stoul(idStr));
        } catch (...) {
            json err;
            err["errorMessage"] = "Invalid sleep id";
            res.status = 400;
            res.set_content(err.dump(), "application/json");
            return;
        }
        bool ok = backend.deleteSleep(token, index);
        if (!ok) {
            json err;
            err["errorMessage"] = "Record not found";
            res.status = 404;
            res.set_content(err.dump(), "application/json");
            return;
        }
        res.status = 204;
        res.set_content("", "application/json");
    });
}

} // namespace routes
