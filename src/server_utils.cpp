#include "server_utils.hpp"
#include "helpers/Logger.hpp"
#include "json.hpp"

using json = nlohmann::ordered_json;

namespace server_utils {

std::string getTokenFromAuthHeader(const httplib::Request &req) {
    auto it = req.headers.find("Authorization");
    if (it == req.headers.end())
        return "";
    const std::string &auth = it->second;
    const std::string prefix = "Bearer ";
    if (auth.size() >= prefix.size() && auth.compare(0, prefix.size(), prefix) == 0) {
        return auth.substr(prefix.size());
    }
    return "";
}

void addCORSHeaders(httplib::Response &res) {
    if (res.get_header_value("Access-Control-Allow-Origin").empty()) {
        res.set_header("Access-Control-Allow-Origin", "*");
    }
    if (res.get_header_value("Access-Control-Allow-Headers").empty()) {
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
    if (res.get_header_value("Access-Control-Allow-Methods").empty()) {
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PATCH, DELETE, OPTIONS");
    }
}

void setupCORS(httplib::Server &svr) {
    svr.Options(R"(.*)", [](const httplib::Request & /*req*/, httplib::Response &res) {
        addCORSHeaders(res);
        res.set_header("Access-Control-Max-Age", "3600");
        res.status = 204;
    });
}

bool requireAuth(const httplib::Request &req, httplib::Response &res, std::string &outToken) {
    outToken = getTokenFromAuthHeader(req);
    if (outToken.empty()) {
        json err;
        err["errorMessage"] = "Missing or invalid Authorization token";
        res.status = 401;
        res.set_content(err.dump(), "application/json");
        return false;
    }
    return true;
}

} // namespace server_utils
