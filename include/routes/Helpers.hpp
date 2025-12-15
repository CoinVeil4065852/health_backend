#pragma once

#include <string>

#include "../third_party/httplib.h"

inline std::string getTokenFromAuthHeader(const httplib::Request& req) {
  auto it = req.headers.find("Authorization");
  if (it == req.headers.end()) {
    return "";
  }
  const std::string& auth = it->second;
  const std::string prefix = "Bearer ";
  if (auth.size() >= prefix.size() && auth.compare(0, prefix.size(), prefix) == 0) {
    return auth.substr(prefix.size());
  }
  return "";
}
