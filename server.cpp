#include <iostream>
#include <string>
#include <sstream>

#include "httplib.h"                 // 第三方 HTTP 函式庫（header-only）
#include "backend/HealthBackend.hpp" // 你的後端核心
#include "helpers/json.hpp"          // 簡單 JSON 工具

using namespace std;

// 小工具：從 header 抓 token
static string getTokenFromHeader(const httplib::Request& req) {
    if (req.has_header("X-Auth-Token")) {
        return req.get_header_value("X-Auth-Token");
    }
    return "";
}

int main() {
    httplib::Server svr;
    HealthBackend backend;  // 所有路由共用同一個 backend 物件

    // ========== 1. 註冊 ==========
    // POST /register
    // body: name, age, weightKg, heightM, password (x-www-form-urlencoded)
    svr.Post("/register", [&](const httplib::Request& req, httplib::Response& res) {
        string name      = req.get_param_value("name");
        string ageStr    = req.get_param_value("age");
        string weightStr = req.get_param_value("weightKg");
        string heightStr = req.get_param_value("heightM");
        string password  = req.get_param_value("password");

        if (name.empty() || ageStr.empty() || weightStr.empty() ||
            heightStr.empty() || password.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing parameters"),
                            "application/json");
            return;
        }

        int age = 0;
        double weight = 0.0;
        double height = 0.0;

        try {
            age    = stoi(ageStr);
            weight = stod(weightStr);
            height = stod(heightStr);
        } catch (...) {
            res.set_content(SimpleJson::makeMessage("error", "Invalid number format"),
                            "application/json");
            return;
        }

        bool ok = backend.registerUser(name, age, weight, height, password);
        if (!ok) {
            res.set_content(SimpleJson::makeMessage("error", "Invalid input or user already exists"),
                            "application/json");
            return;
        }

        res.set_content(SimpleJson::makeMessage("ok", "Registered successfully"),
                        "application/json");
    });

    // ========== 2. 登入 ==========
    // POST /login
    // body: name, password
    svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res) {
        string name = req.get_param_value("name");
        string pw   = req.get_param_value("password");

        if (name.empty() || pw.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing name or password"),
                            "application/json");
            return;
        }

        string token = backend.login(name, pw);
        if (token == "INVALID") {
            res.set_content(SimpleJson::makeMessage("error", "Invalid name or password"),
                            "application/json");
            return;
        }

        ostringstream os;
        os << "{"
           << "\"status\":\"ok\","
           << "\"token\":\"" << SimpleJson::escape(token) << "\""
           << "}";

        res.set_content(os.str(), "application/json");
    });

    // ========== 3. BMI ==========
    // GET /user/bmi  (header: X-Auth-Token)
    svr.Get("/user/bmi", [&](const httplib::Request& req, httplib::Response& res) {
        string token = getTokenFromHeader(req);
        if (token.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing X-Auth-Token header"),
                            "application/json");
            return;
        }

        double bmi = backend.getBMI(token);
        if (bmi <= 0.0) {
            res.set_content(SimpleJson::makeMessage("error", "Invalid token or user not found"),
                            "application/json");
            return;
        }

        ostringstream os;
        os << "{"
           << "\"status\":\"ok\","
           << "\"bmi\":" << bmi
           << "}";

        res.set_content(os.str(), "application/json");
    });

    // ========== 4. 新增水紀錄 ==========
    // POST /water/add (header: X-Auth-Token, body: date, amountMl)
    svr.Post("/water/add", [&](const httplib::Request& req, httplib::Response& res) {
        string token = getTokenFromHeader(req);
        if (token.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing X-Auth-Token header"),
                            "application/json");
            return;
        }

        string date      = req.get_param_value("date");
        string amountStr = req.get_param_value("amountMl");

        if (date.empty() || amountStr.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing date or amountMl"),
                            "application/json");
            return;
        }

        double amount = 0.0;
        try {
            amount = stod(amountStr);
        } catch (...) {
            res.set_content(SimpleJson::makeMessage("error", "Invalid number format"),
                            "application/json");
            return;
        }

        bool ok = backend.addWater(token, date, amount);
        if (!ok) {
            res.set_content(SimpleJson::makeMessage("error", "Invalid input or token"),
                            "application/json");
            return;
        }

        res.set_content(SimpleJson::makeMessage("ok", "Water record added"),
                        "application/json");
    });

    // ========== 5. 水紀錄列表 ==========
    // GET /water/list (header: X-Auth-Token)
    svr.Get("/water/list", [&](const httplib::Request& req, httplib::Response& res) {
        string token = getTokenFromHeader(req);
        if (token.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing X-Auth-Token header"),
                            "application/json");
            return;
        }

        vector<WaterRecord> records = backend.getAllWater(token);

        ostringstream os;
        os << "{"
           << "\"status\":\"ok\","
           << "\"records\":[";
        for (size_t i = 0; i < records.size(); ++i) {
            if (i > 0) os << ",";
            os << "{"
               << "\"date\":\"" << SimpleJson::escape(records[i].date) << "\","
               << "\"amountMl\":" << records[i].amountMl
               << "}";
        }
        os << "]}";

        res.set_content(os.str(), "application/json");
    });

    // ========== 6. 每日平均飲水 ==========
    // GET /water/weekly?goalMl=1500 (header: X-Auth-Token)
    svr.Get("/water/weekly", [&](const httplib::Request& req, httplib::Response& res) {
        string token = getTokenFromHeader(req);
        if (token.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing X-Auth-Token header"),
                            "application/json");
            return;
        }

        double avg = backend.getWeeklyAverageWater(token);

        bool hasGoal = req.has_param("goalMl");
        double goal = 0.0;
        if (hasGoal) {
            string goalStr = req.get_param_value("goalMl");
            try {
                goal = stod(goalStr);
            } catch (...) {
                res.set_content(SimpleJson::makeMessage("error", "Invalid goalMl format"),
                                "application/json");
                return;
            }
        }

        ostringstream os;
        os << "{"
           << "\"status\":\"ok\","
           << "\"averageMl\":" << avg;
        if (hasGoal) {
            bool enough = backend.isWaterEnough(token, goal);
            os << ",\"goalMl\":" << goal
               << ",\"enough\":" << (enough ? "true" : "false");
        }
        os << "}";

        res.set_content(os.str(), "application/json");
    });

    // ========== 7. 新增睡眠 ==========
    // POST /sleep/add (header: X-Auth-Token, body: date, hours)
    svr.Post("/sleep/add", [&](const httplib::Request& req, httplib::Response& res) {
        string token = getTokenFromHeader(req);
        if (token.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing X-Auth-Token header"),
                            "application/json");
            return;
        }

        string date     = req.get_param_value("date");
        string hoursStr = req.get_param_value("hours");

        if (date.empty() || hoursStr.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing date or hours"),
                            "application/json");
            return;
        }

        double hours = 0.0;
        try {
            hours = stod(hoursStr);
        } catch (...) {
            res.set_content(SimpleJson::makeMessage("error", "Invalid hours format"),
                            "application/json");
            return;
        }

        bool ok = backend.addSleep(token, date, hours);
        if (!ok) {
            res.set_content(SimpleJson::makeMessage("error", "Invalid input or token"),
                            "application/json");
            return;
        }

        res.set_content(SimpleJson::makeMessage("ok", "Sleep record added"),
                        "application/json");
    });

    // ========== 8. 最後一次睡眠 ==========
    // GET /sleep/last?minHrs=7 (header: X-Auth-Token)
    svr.Get("/sleep/last", [&](const httplib::Request& req, httplib::Response& res) {
        string token = getTokenFromHeader(req);
        if (token.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing X-Auth-Token header"),
                            "application/json");
            return;
        }

        double last = backend.getLastSleepHours(token);
        if (last <= 0.0) {
            res.set_content(SimpleJson::makeMessage("error", "No sleep record or invalid token"),
                            "application/json");
            return;
        }

        bool hasMin = req.has_param("minHrs");
        double minHrs = 0.0;
        if (hasMin) {
            string minStr = req.get_param_value("minHrs");
            try {
                minHrs = stod(minStr);
            } catch (...) {
                res.set_content(SimpleJson::makeMessage("error", "Invalid minHrs format"),
                                "application/json");
                return;
            }
        }

        ostringstream os;
        os << "{"
           << "\"status\":\"ok\","
           << "\"lastHours\":" << last;
        if (hasMin) {
            bool enough = backend.isSleepEnough(token, minHrs);
            os << ",\"minHours\":" << minHrs
               << ",\"enough\":" << (enough ? "true" : "false");
        }
        os << "}";

        res.set_content(os.str(), "application/json");
    });

    // ========== 9. 新增活動 ==========
    // POST /activity/add (header: X-Auth-Token, body: date, minutes, intensity)
    svr.Post("/activity/add", [&](const httplib::Request& req, httplib::Response& res) {
        string token = getTokenFromHeader(req);
        if (token.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing X-Auth-Token header"),
                            "application/json");
            return;
        }

        string date      = req.get_param_value("date");
        string minStr    = req.get_param_value("minutes");
        string intensity = req.get_param_value("intensity");

        if (date.empty() || minStr.empty() || intensity.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing parameters"),
                            "application/json");
            return;
        }

        int minutes = 0;
        try {
            minutes = stoi(minStr);
        } catch (...) {
            res.set_content(SimpleJson::makeMessage("error", "Invalid minutes format"),
                            "application/json");
            return;
        }

        bool ok = backend.addActivity(token, date, minutes, intensity);
        if (!ok) {
            res.set_content(SimpleJson::makeMessage("error", "Invalid input or token"),
                            "application/json");
            return;
        }

        res.set_content(SimpleJson::makeMessage("ok", "Activity added"),
                        "application/json");
    });

    // ========== 10. 活動列表 ==========
    // GET /activity/list?sortBy=duration (header: X-Auth-Token)
    svr.Get("/activity/list", [&](const httplib::Request& req, httplib::Response& res) {
        string token = getTokenFromHeader(req);
        if (token.empty()) {
            res.set_content(SimpleJson::makeMessage("error", "Missing X-Auth-Token header"),
                            "application/json");
            return;
        }

        if (req.has_param("sortBy") && req.get_param_value("sortBy") == "duration") {
            backend.sortActivityByDuration(token);
        }

        vector<ActivityRecord> acts = backend.getAllActivity(token);

        ostringstream os;
        os << "{"
           << "\"status\":\"ok\","
           << "\"records\":[";
        for (size_t i = 0; i < acts.size(); ++i) {
            if (i > 0) os << ",";
            os << "{"
               << "\"date\":\"" << SimpleJson::escape(acts[i].date) << "\","
               << "\"minutes\":" << acts[i].minutes << ","
               << "\"intensity\":\"" << SimpleJson::escape(acts[i].intensity) << "\""
               << "}";
        }
        os << "]}";

        res.set_content(os.str(), "application/json");
    });

    cout << "Server started at http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);  // 開在 8080 port
}
