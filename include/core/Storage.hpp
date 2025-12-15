#pragma once

#include "../../third_party/json.hpp"
#include <string>

class Storage {
 public:
  Storage();
  explicit Storage(const std::string &path);

  std::string path() const { return storagePath; }

  // Load JSON from storage; returns empty json object on missing or parse error
  nlohmann::json load() const;

  // Save JSON to storage; returns true on success
  bool save(const nlohmann::json &j) const;

 private:
  std::string storagePath;

  static bool dirExists(const std::string &path);
  void ensureStorageDirExists() const;
};
