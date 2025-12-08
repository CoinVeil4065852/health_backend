#include "Water.hpp"

bool WaterManager::addRecord(const std::string& userName,
                             const std::string& date,
                             double amountMl) {
    data[userName].push_back({date, amountMl});
    return true;
}

bool WaterManager::updateRecord(const std::string& userName,
                                std::size_t index,
                                const std::string& newDate,
                                double newAmountMl) {
    auto it = data.find(userName);
    if (it == data.end()) return false;
    if (index >= it->second.size()) return false;

    it->second[index].date = newDate;
    it->second[index].amountMl = newAmountMl;
    return true;
}

bool WaterManager::deleteRecord(const std::string& userName,
                                std::size_t index) {
    auto it = data.find(userName);
    if (it == data.end()) return false;
    if (index >= it->second.size()) return false;

    auto& vec = it->second;
    vec.erase(vec.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

std::vector<WaterRecord> WaterManager::getAll(const std::string& userName) const {
    auto it = data.find(userName);
    if (it == data.end()) return {};
    return it->second;
}

double WaterManager::getWeeklyAverage(const std::string& userName) const {
    auto it = data.find(userName);
    if (it == data.end()) return 0.0;

    const auto& vec = it->second;
    if (vec.empty()) return 0.0;

    std::size_t count = vec.size();
    if (count > 7) count = 7;

    double sum = 0.0;
    for (std::size_t i = vec.size() - count; i < vec.size(); ++i) {
        sum += vec[i].amountMl;
    }

    return sum / static_cast<double>(count);
}

bool WaterManager::isEnoughForWeek(const std::string& userName,
                                   double dailyGoalMl) const {
    double avg = getWeeklyAverage(userName);
    return avg >= dailyGoalMl;
}
