#pragma once

#include <string>
#include "httplib.h"

namespace server_utils {

std::string getTokenFromAuthHeader(const httplib::Request& req);
void addCORSHeaders(httplib::Response& res);
void setupCORS(httplib::Server& svr);
bool requireAuth(const httplib::Request& req, httplib::Response& res, std::string& outToken);

} // namespace server_utils
