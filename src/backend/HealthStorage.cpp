#include "backend/HealthStorage.hpp"
#include "backend/HealthBackend.hpp"
#include "json.hpp"
#include "helpers/Logger.hpp"

#include <unistd.h>     // readlink
#ifdef __APPLE__
#include <mach-o/dyld.h> // _NSGetExecutablePath
#endif

#include <sys/stat.h>   // stat, mkdir
#include <sys/types.h>

#include <fstream>

using nlohmann::json;

namespace health {

static bool dirExists(const std::string &path) {
    struct stat st {};
    if (stat(path.c_str(), &st) != 0) return false;
    return S_ISDIR(st.st_mode);
}

void HealthStorage::initStoragePath(HealthBackend& backend) {
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
    if (pos != std::string::npos) {
        exeDir = exeDir.substr(0, pos);
    }

    std::string dataFolder = exeDir + "/data";
    backend.storagePath = dataFolder + "/storage.json";
    // force relative path
    backend.storagePath = "data/storage.json";
}

void HealthStorage::ensureStorageDirExists(const HealthBackend& backend) {
    auto pos = backend.storagePath.find_last_of("/\\");
    if (pos == std::string::npos) return;
    std::string dir = backend.storagePath.substr(0, pos);
    if (!dirExists(dir)) {
        mkdir(dir.c_str(), 0755);
    }
}

void HealthStorage::loadFromFile(HealthBackend& backend) {
    std::ifstream in(backend.storagePath);
    if (!in) return;

    json j;
    try {
        in >> j;
    } catch (...) {
        util::Logger::error(std::string("Failed to parse ") + backend.storagePath + ", starting empty.");
        return;
    }

    if (!j.contains("users") || !j["users"].is_array()) return;

    for (const auto& ju : j["users"]) {
        if (!ju.contains("name")) continue;
        std::string name = ju.value("name", "");

        HealthBackend::UserData data;
        data.profile.id       = ju.value("id", name);
        data.profile.name     = name;
        data.profile.age      = ju.value("age", 0);
        data.profile.weightKg = ju.value("weightKg", 0.0);
        data.profile.heightM  = ju.value("heightM", 0.0);
        data.profile.gender   = ju.value("gender", std::string("other"));
        data.password         = ju.value("password", std::string(""));

        if (ju.contains("waters") && ju["waters"].is_array()) {
            for (const auto& jw : ju["waters"]) {
                WaterRecord w;
                w.datetime = jw.value("datetime", std::string(""));
                w.amountMl = jw.value("amountMl", 0.0);
                data.waters.push_back(w);
            }
        }

        if (ju.contains("sleeps") && ju["sleeps"].is_array()) {
            for (const auto& js : ju["sleeps"]) {
                SleepRecord s;
                s.datetime = js.value("datetime", std::string(""));
                s.hours    = js.value("hours", 0.0);
                data.sleeps.push_back(s);
            }
        }

        if (ju.contains("activities") && ju["activities"].is_array()) {
            for (const auto& ja : ju["activities"]) {
                ActivityRecord a;
                a.datetime  = ja.value("datetime", std::string(""));
                a.minutes   = ja.value("minutes", 0);
                a.intensity = ja.value("intensity", std::string(""));
                data.activities.push_back(a);
            }
        }

        if (ju.contains("categories") && ju["categories"].is_object()) {
            for (auto it = ju["categories"].begin(); it != ju["categories"].end(); ++it) {
                const std::string catName = it.key();
                const auto& arr = it.value();
                if (!arr.is_array()) continue;

                std::vector<CategoryItem> items;
                for (const auto& ji : arr) {
                    CategoryItem item;
                    item.datetime = ji.value("datetime", std::string(""));
                    item.note     = ji.value("note", std::string(""));
                    item.value    = ji.value("value", 0.0);
                    items.push_back(item);
                }
                data.categories[catName] = std::move(items);
            }
        }

        backend.usersByName[name] = std::move(data);
    }
}

void HealthStorage::saveToFile(const HealthBackend& backend) {
    ensureStorageDirExists(backend);

    json j;
    j["users"] = json::array();

    for (const auto& [name, data] : backend.usersByName) {
        json ju;
        ju["id"]       = data.profile.id;
        ju["name"]     = data.profile.name;
        ju["age"]      = data.profile.age;
        ju["weightKg"] = data.profile.weightKg;
        ju["heightM"]  = data.profile.heightM;
        ju["gender"]   = data.profile.gender;

        ju["password"] = data.password;

        ju["waters"] = json::array();
        for (const auto& w : data.waters) {
            json jw; jw["datetime"] = w.datetime; jw["amountMl"] = w.amountMl; ju["waters"].push_back(jw);
        }

        ju["sleeps"] = json::array();
        for (const auto& s : data.sleeps) { json js; js["datetime"] = s.datetime; js["hours"] = s.hours; ju["sleeps"].push_back(js); }

        ju["activities"] = json::array();
        for (const auto& a : data.activities) { json ja; ja["datetime"] = a.datetime; ja["minutes"] = a.minutes; ja["intensity"] = a.intensity; ju["activities"].push_back(ja); }

        ju["categories"] = json::object();
        for (const auto& [catName, items] : data.categories) {
            json arr = json::array();
            for (const auto& item : items) { json ji; ji["datetime"] = item.datetime; ji["note"] = item.note; ji["value"] = item.value; arr.push_back(ji); }
            ju["categories"][catName] = arr;
        }

        j["users"].push_back(ju);
    }

    std::ofstream out(backend.storagePath);
    if (!out) {
        util::Logger::error(std::string("Failed to open ") + backend.storagePath + " for writing.");
        return;
    }
    out << j.dump(2);
}

} // namespace health
