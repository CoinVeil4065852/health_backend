#include "HealthBackend.hpp"
#include "../helpers/validation.hpp"

std::string HealthBackend::getUserNameFromToken(const std::string& token) const {
    return userBackend.getUserNameByToken(token);
}

// 使用者相關 ---------------------------------------------------

bool HealthBackend::registerUser(const std::string& name,
                                 int age,
                                 double weightKg,
                                 double heightM,
                                 const std::string& password) {
    if (!Validation::isValidName(name)) return false;
    if (!Validation::isValidAge(age)) return false;
    if (!Validation::isValidWeight(weightKg)) return false;
    if (!Validation::isValidHeight(heightM)) return false;
    if (!Validation::isValidPassword(password)) return false;

    return userBackend.registerUser(name, age, weightKg, heightM, password);
}

std::string HealthBackend::login(const std::string& name,
                                 const std::string& password) {
    return userBackend.login(name, password);
}

bool HealthBackend::updateUser(const std::string& token,
                               int newAge,
                               double newWeightKg,
                               double newHeightM,
                               const std::string& newPassword) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;

    if (!Validation::isValidAge(newAge)) return false;
    if (!Validation::isValidWeight(newWeightKg)) return false;
    if (!Validation::isValidHeight(newHeightM)) return false;
    if (!Validation::isValidPassword(newPassword)) return false;

    return userBackend.updateUser(userName,
                                  newAge,
                                  newWeightKg,
                                  newHeightM,
                                  newPassword);
}

double HealthBackend::getBMI(const std::string& token) const {
    return userBackend.getUserBMI(token);
}

// 水紀錄 -------------------------------------------------------

bool HealthBackend::addWater(const std::string& token,
                             const std::string& date,
                             double amountMl) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    if (!Validation::isValidDate(date)) return false;
    if (!Validation::isNonNegative(amountMl)) return false;

    return waterManager.addRecord(userName, date, amountMl);
}

bool HealthBackend::updateWater(const std::string& token,
                                std::size_t index,
                                const std::string& newDate,
                                double newAmountMl) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    if (!Validation::isValidDate(newDate)) return false;
    if (!Validation::isNonNegative(newAmountMl)) return false;

    return waterManager.updateRecord(userName, index, newDate, newAmountMl);
}

bool HealthBackend::deleteWater(const std::string& token, std::size_t index) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    return waterManager.deleteRecord(userName, index);
}

std::vector<WaterRecord> HealthBackend::getAllWater(const std::string& token) const {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return {};
    return waterManager.getAll(userName);
}

double HealthBackend::getWeeklyAverageWater(const std::string& token) const {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return 0.0;
    return waterManager.getWeeklyAverage(userName);
}

bool HealthBackend::isWaterEnough(const std::string& token,
                                  double dailyGoalMl) const {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    return waterManager.isEnoughForWeek(userName, dailyGoalMl);
}

// 睡眠紀錄 -----------------------------------------------------

bool HealthBackend::addSleep(const std::string& token,
                             const std::string& date,
                             double hours) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    if (!Validation::isValidDate(date)) return false;
    if (!Validation::isNonNegative(hours)) return false;

    return sleepManager.addRecord(userName, date, hours);
}

bool HealthBackend::updateSleep(const std::string& token,
                                std::size_t index,
                                const std::string& newDate,
                                double newHours) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    if (!Validation::isValidDate(newDate)) return false;
    if (!Validation::isNonNegative(newHours)) return false;

    return sleepManager.updateRecord(userName, index, newDate, newHours);
}

bool HealthBackend::deleteSleep(const std::string& token, std::size_t index) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    return sleepManager.deleteRecord(userName, index);
}

std::vector<SleepRecord> HealthBackend::getAllSleep(const std::string& token) const {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return {};
    return sleepManager.getAll(userName);
}

double HealthBackend::getLastSleepHours(const std::string& token) const {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return 0.0;
    return sleepManager.getLastSleepHours(userName);
}

bool HealthBackend::isSleepEnough(const std::string& token,
                                  double minHours) const {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    return sleepManager.isSleepEnough(userName, minHours);
}

// 活動紀錄 -----------------------------------------------------

bool HealthBackend::addActivity(const std::string& token,
                                const std::string& date,
                                int minutes,
                                const std::string& intensity) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    if (!Validation::isValidDate(date)) return false;
    if (minutes < 0) return false;

    return activityManager.addRecord(userName, date, minutes, intensity);
}

bool HealthBackend::updateActivity(const std::string& token,
                                   std::size_t index,
                                   const std::string& newDate,
                                   int newMinutes,
                                   const std::string& newIntensity) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    if (!Validation::isValidDate(newDate)) return false;
    if (newMinutes < 0) return false;

    return activityManager.updateRecord(userName,
                                        index,
                                        newDate,
                                        newMinutes,
                                        newIntensity);
}

bool HealthBackend::deleteActivity(const std::string& token, std::size_t index) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    return activityManager.deleteRecord(userName, index);
}

std::vector<ActivityRecord> HealthBackend::getAllActivity(const std::string& token) const {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return {};
    return activityManager.getAll(userName);
}

void HealthBackend::sortActivityByDuration(const std::string& token) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return;
    activityManager.sortByDuration(userName);
}

// 其他分類 -----------------------------------------------------

bool HealthBackend::addOtherRecord(const std::string& token,
                                   const std::string& categoryName,
                                   const std::string& date,
                                   double value,
                                   const std::string& note) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    if (!Validation::isValidName(categoryName)) return false;
    if (!Validation::isValidDate(date)) return false;

    return otherManager.addRecord(userName,
                                  categoryName,
                                  date,
                                  value,
                                  note);
}

bool HealthBackend::deleteOtherRecord(const std::string& token,
                                      const std::string& categoryName,
                                      std::size_t index) {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return false;
    return otherManager.deleteRecord(userName, categoryName, index);
}

std::vector<std::string> HealthBackend::getOtherCategories(const std::string& token) const {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return {};
    return otherManager.getCategories(userName);
}

std::vector<OtherRecord> HealthBackend::getOtherRecords(const std::string& token,
                                                        const std::string& categoryName) const {
    std::string userName = getUserNameFromToken(token);
    if (userName.empty()) return {};
    return otherManager.getRecords(userName, categoryName);
}
