#include "Sleep.hpp"

bool SleepManager::addRecord(const std::string& userName,
                             const std::string& date,
                             double hours) {
    data[userName].push_back({date, hours});
    return true;
}

bool SleepManager::updateRecord(const std::string& userName,
                                std::size_t index,
                                const std::string& newDate,
                                double newHours) {
    auto it = data.find(userName);
    if (it == data.end()) return false;
    if (index >= it->second.size()) return false;

    it->second[index].date = newDate;
    it->second[index].hours = newHours;
    return true;
}

bool SleepManager::deleteRecord(const std::string& userName,
                                std::size_t index) {
    auto it = data.find(userName);
    if (it == data.end()) return false;
    if (index >= it->second.size()) return false;

    auto& vec = it->second;
    vec.erase(vec.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

std::vector<SleepRecord> SleepManager::getAll(const std::string& userName) const {
    auto it = data.find(userName);
    if (it == data.end()) return {};
    return it->second;
}

double SleepManager::getLastSleepHours(const std::string& userName) const {
    auto it = data.find(userName);
    if (it == data.end()) return 0.0;
    const auto& vec = it->second;
    if (vec.empty()) return 0.0;
    return vec.back().hours;
}

bool SleepManager::isSleepEnough(const std::string& userName,
                                 double minHours) const {
    return getLastSleepHours(userName) >= minHours;
}
