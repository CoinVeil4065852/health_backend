#include "../../include/core/Storage.hpp"

#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <filesystem>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

using nlohmann::json;

bool Storage::dirExists(const std::string &path) {
  struct stat st {};
  if (stat(path.c_str(), &st) != 0) return false;
  return S_ISDIR(st.st_mode);
}

void Storage::ensureStorageDirExists() const {
  auto pos = storagePath.find_last_of("/\\");
  if (pos == std::string::npos) return;
  std::string dir = storagePath.substr(0, pos);
  if (!dirExists(dir)) {
    std::filesystem::create_directories(dir);
  }
}

Storage::Storage() {
  char exePath[1024] = {0};
  std::string exeDir = ".";

#if defined(__APPLE__)
  uint32_t size = sizeof(exePath);
  if (_NSGetExecutablePath(exePath, &size) == 0) {
    exeDir = exePath;
  }
#elif defined(__linux__)
  ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
  if (len != -1) {
    exePath[len] = '\0';
    exeDir = exePath;
  }
#else
  exeDir = ".";
#endif

  auto pos = exeDir.find_last_of("/\\");
  if (pos != std::string::npos) exeDir = exeDir.substr(0, pos);

  std::string dataFolder = exeDir + "/data";
  storagePath = dataFolder + "/storage.json";
  // Force project relative at runtime for predictable dev behavior
  storagePath = "data/storage.json";
}

Storage::Storage(const std::string &path) : storagePath(path) {}

json Storage::load() const {
  std::ifstream in(storagePath);
  if (!in) return json();
  json j;
  try {
    in >> j;
  } catch (...) {
    return json();
  }
  return j;
}

bool Storage::save(const json &j) const {
  ensureStorageDirExists();
  try {
    std::ofstream out(storagePath);
    if (!out) return false;
    out << j.dump(2);
    return true;
  } catch (...) {
    return false;
  }
}
