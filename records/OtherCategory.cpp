#include "OtherCategory.hpp"

bool OtherCategoryManager::addRecord(const std::string& userName,
                                     const std::string& categoryName,
                                     const std::string& date,
                                     double value,
                                     const std::string& note) {
    auto& categoryMap = data[userName];
    auto& vec = categoryMap[categoryName];
    vec.push_back({categoryName, date, value, note});
    return true;
}

bool OtherCategoryManager::deleteRecord(const std::string& userName,
                                        const std::string& categoryName,
                                        std::size_t index) {
    auto itUser = data.find(userName);
    if (itUser == data.end()) return false;

    auto& categoryMap = itUser->second;
    auto itCat = categoryMap.find(categoryName);
    if (itCat == categoryMap.end()) return false;

    auto& vec = itCat->second;
    if (index >= vec.size()) return false;

    vec.erase(vec.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

std::vector<std::string> OtherCategoryManager::getCategories(const std::string& userName) const {
    std::vector<std::string> result;
    auto itUser = data.find(userName);
    if (itUser == data.end()) return result;

    for (const auto& pair : itUser->second) {
        result.push_back(pair.first);
    }
    return result;
}

std::vector<OtherRecord> OtherCategoryManager::getRecords(
    const std::string& userName,
    const std::string& categoryName) const {
    auto itUser = data.find(userName);
    if (itUser == data.end()) return {};

    auto itCat = itUser->second.find(categoryName);
    if (itCat == itUser->second.end()) return {};

    return itCat->second;
}
