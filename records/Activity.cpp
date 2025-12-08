#include "Activity.hpp"
#include <algorithm>

bool ActivityManager::addRecord(const std::string& userName,
                                const std::string& date,
                                int minutes,
                                const std::string& intensity) {
    data[userName].push_back({date, minutes, intensity});
    return true;
}

bool ActivityManager::updateRecord(const std::string& userName,
                                   std::size_t index,
                                   const std::string& newDate,
                                   int newMinutes,
                                   const std::string& newIntensity) {
    auto it = data.find(userName);
    if (it == data.end()) return false;
    if (index >= it->second.size()) return false;

    it->second[index].date = newDate;
    it->second[index].minutes = newMinutes;
    it->second[index].intensity = newIntensity;
    return true;
}

bool ActivityManager::deleteRecord(const std::string& userName,
                                   std::size_t index) {
    auto it = data.find(userName);
    if (it == data.end()) return false;
    if (index >= it->second.size()) return false;

    auto& vec = it->second;
    vec.erase(vec.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

std::vector<ActivityRecord> ActivityManager::getAll(const std::string& userName) const {
    auto it = data.find(userName);
    if (it == data.end()) return {};
    return it->second;
}

void ActivityManager::sortByDuration(const std::string& userName) {
    auto it = data.find(userName);
    if (it == data.end()) return;

    auto& vec = it->second;
    std::sort(vec.begin(), vec.end(),
              [](const ActivityRecord& a, const ActivityRecord& b) {
                  return a.minutes > b.minutes; // 由大到小
              });
}
