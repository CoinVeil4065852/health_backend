#include "HealthBackend.hpp"
#include "json.hpp"
#include "HealthStorage.hpp"

#include <fstream>
#include <random>
#include <iostream>
#include "../helpers/Logger.hpp"

// 使用 nlohmann::json 方便寫成 json
using nlohmann::json;

namespace health {

// ----------------------
// 初始化：delegates to HealthStorage
// ----------------------

void HealthBackend::initStoragePath() {
    HealthStorage::initStoragePath(*this);
}

// 建立 data/ 資料夾（如果不存在）
void HealthBackend::ensureStorageDirExists() const {
    HealthStorage::ensureStorageDirExists(*this);
}

// ----------------------
// 建構 / 解構：處理載入 / 儲存
// ----------------------

HealthBackend::HealthBackend() {
    initStoragePath();        // ⭐ 依照執行檔位置決定 data/storage.json
    ensureStorageDirExists(); // ⭐ 確保 data/ 存在
    loadFromFile();           // ⭐ 嘗試載入舊有資料
}

HealthBackend::~HealthBackend() {
    try {
        saveToFile();
    } catch (...) {
        // 不讓 destructor 拋例外
    }
}

// ----------------------
// Helper：產生 token
// ----------------------

std::string HealthBackend::generateToken() const {
    static const char chars[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static constexpr std::size_t N = sizeof(chars) - 1;

    std::random_device rd;
    std::mt19937_64 rng(rd());
    std::uniform_int_distribution<std::size_t> dist(0, N - 1);

    std::string token;
    token.reserve(32);
    for (int i = 0; i < 32; ++i) {
        token.push_back(chars[dist(rng)]);
    }
    return token;
}

// ----------------------
// 檔案 I/O：load / save
// ----------------------

void HealthBackend::loadFromFile() {
    HealthStorage::loadFromFile(*this);
}

void HealthBackend::saveToFile() const {
    HealthStorage::saveToFile(*this);
}

// ----------------------
// Token → UserData
// ----------------------

HealthBackend::UserData* HealthBackend::getUserByToken(const std::string& token) {
    auto itTok = tokenToName.find(token);
    if (itTok == tokenToName.end()) return nullptr;
    auto itUser = usersByName.find(itTok->second);
    if (itUser == usersByName.end()) return nullptr;
    return &itUser->second;
}

const HealthBackend::UserData* HealthBackend::getUserByToken(const std::string& token) const {
    auto itTok = tokenToName.find(token);
    if (itTok == tokenToName.end()) return nullptr;
    auto itUser = usersByName.find(itTok->second);
    if (itUser == usersByName.end()) return nullptr;
    return &itUser->second;
}

bool HealthBackend::hasUserForToken(const std::string& token) const {
    return getUserByToken(token) != nullptr;
}

// ----------------------
// User / Auth
// ----------------------

bool HealthBackend::registerUser(const std::string& name,
                                 int                age,
                                 double             weightKg,
                                 double             heightM,
                                 const std::string& password,
                                 const std::string& gender) {
    if (name.empty() || password.empty()) return false;
    if (age <= 0 || weightKg <= 0.0 || heightM <= 0.0) return false;

    if (usersByName.find(name) != usersByName.end()) {
        // User already exists
        return false;
    }

    UserData data;
    data.profile.id       = name; // 簡單用 name 當 id
    data.profile.name     = name;
    data.profile.age      = age;
    data.profile.weightKg = weightKg;
    data.profile.heightM  = heightM;
    data.profile.gender   = gender;
    data.password         = password;

    usersByName[name] = std::move(data);
    saveToFile();
    util::Logger::info(std::string("registerUser: created user: ") + name);
    return true;
}

std::string HealthBackend::login(const std::string& name,
                                 const std::string& password) {
    auto it = usersByName.find(name);
    if (it == usersByName.end()) {
        util::Logger::warn(std::string("login: user not found: ") + name);
        return "INVALID";
    }
    if (it->second.password != password) {
        util::Logger::warn(std::string("login: bad password for user: ") + name);
        return "INVALID";
    }

    // 產生新的 token
    std::string token = generateToken();
    tokenToName[token] = name;
    util::Logger::info(std::string("login: user= ") + name + " token=" + token);
    return token;
}

bool HealthBackend::getUserProfile(const std::string& token,
                                   UserProfile&       outProfile) const {
    const UserData* user = getUserByToken(token);
    if (!user) return false;
    outProfile = user->profile;
    return true;
}

double HealthBackend::getBMI(const std::string& token) const {
    const UserData* user = getUserByToken(token);
    if (!user) return 0.0;
    if (user->profile.heightM <= 0.0) return 0.0;
    if (user->profile.weightKg <= 0.0) return 0.0;

    return user->profile.weightKg / (user->profile.heightM * user->profile.heightM);
}

// ----------------------
// Waters
// ----------------------

bool HealthBackend::addWater(const std::string& token,
                             const std::string& datetime,
                             double             amountMl) {
    if (amountMl <= 0.0) return false;
    UserData* user = getUserByToken(token);
    if (!user) return false;

    WaterRecord w;
    w.datetime = datetime;
    w.amountMl = amountMl;
    user->waters.push_back(w);
    saveToFile();
    return true;
}

std::vector<WaterRecord> HealthBackend::getAllWater(const std::string& token) const {
    const UserData* user = getUserByToken(token);
    if (!user) return {};
    return user->waters;
}

bool HealthBackend::updateWater(const std::string& token,
                                std::size_t       index,
                                const std::string& newDatetime,
                                double             newAmountMl) {
    if (newAmountMl <= 0.0) return false;
    UserData* user = getUserByToken(token);
    if (!user) return false;
    if (index >= user->waters.size()) return false;

    user->waters[index].datetime = newDatetime;
    user->waters[index].amountMl = newAmountMl;
    saveToFile();
    return true;
}

bool HealthBackend::deleteWater(const std::string& token,
                                std::size_t       index) {
    UserData* user = getUserByToken(token);
    if (!user) return false;
    if (index >= user->waters.size()) return false;

    user->waters.erase(user->waters.begin() + static_cast<long>(index));
    saveToFile();
    return true;
}

// ----------------------
// Sleeps
// ----------------------

bool HealthBackend::addSleep(const std::string& token,
                             const std::string& datetime,
                             double             hours) {
    if (hours < 0.0) {
        util::Logger::warn(std::string("addSleep: invalid hours: ") + std::to_string(hours));
        return false;
    }
    UserData* user = getUserByToken(token);
    if (!user) return false;

    SleepRecord s;
    s.datetime = datetime;
    s.hours    = hours;
    user->sleeps.push_back(s);
    saveToFile();
    util::Logger::info(std::string("addSleep: user token found, added sleep for token: ") + token);
    return true;
}

std::vector<SleepRecord> HealthBackend::getAllSleep(const std::string& token) const {
    const UserData* user = getUserByToken(token);
    if (!user) return {};
    return user->sleeps;
}

bool HealthBackend::updateSleep(const std::string& token,
                                std::size_t       index,
                                const std::string& newDatetime,
                                double             newHours) {
    if (newHours < 0.0) return false;
    UserData* user = getUserByToken(token);
    if (!user) return false;
    if (index >= user->sleeps.size()) return false;

    user->sleeps[index].datetime = newDatetime;
    user->sleeps[index].hours    = newHours;
    saveToFile();
    return true;
}

bool HealthBackend::deleteSleep(const std::string& token,
                                std::size_t       index) {
    UserData* user = getUserByToken(token);
    if (!user) return false;
    if (index >= user->sleeps.size()) return false;

    user->sleeps.erase(user->sleeps.begin() + static_cast<long>(index));
    saveToFile();
    return true;
}

// ----------------------
// Activities
// ----------------------

bool HealthBackend::addActivity(const std::string& token,
                                const std::string& datetime,
                                int                minutes,
                                const std::string& intensity) {
    if (minutes <= 0) return false;
    UserData* user = getUserByToken(token);
    if (!user) return false;

    ActivityRecord a;
    a.datetime  = datetime;
    a.minutes   = minutes;
    a.intensity = intensity;
    user->activities.push_back(a);
    saveToFile();
    return true;
}

std::vector<ActivityRecord> HealthBackend::getAllActivity(const std::string& token) const {
    const UserData* user = getUserByToken(token);
    if (!user) return {};
    return user->activities;
}

bool HealthBackend::updateActivity(const std::string& token,
                                   std::size_t       index,
                                   const std::string& newDatetime,
                                   int                newMinutes,
                                   const std::string& newIntensity) {
    if (newMinutes <= 0) return false;
    UserData* user = getUserByToken(token);
    if (!user) return false;
    if (index >= user->activities.size()) return false;

    user->activities[index].datetime  = newDatetime;
    user->activities[index].minutes   = newMinutes;
    user->activities[index].intensity = newIntensity;
    saveToFile();
    return true;
}

bool HealthBackend::deleteActivity(const std::string& token,
                                   std::size_t       index) {
    UserData* user = getUserByToken(token);
    if (!user) return false;
    if (index >= user->activities.size()) return false;

    user->activities.erase(user->activities.begin() + static_cast<long>(index));
    saveToFile();
    return true;
}

std::vector<std::string> HealthBackend::getOtherCategories(const std::string& token) const {
    const UserData* user = getUserByToken(token);
    if (!user) return {};

    std::vector<std::string> cats;
    for (const auto& [name, _vec] : user->categories) {
        cats.push_back(name);
    }
    return cats;
}

bool HealthBackend::createCategory(const std::string& token,
                                   const std::string& name)
{
    if (name.empty()) return false;
    UserData* user = getUserByToken(token);
    if (!user) return false;

    if (user->categories.find(name) != user->categories.end())
        return false; // 已存在

    user->categories[name] = {};  // 建立空 category
    saveToFile();
    return true;
}

// ⚠️ 不再自動建立 category
bool HealthBackend::addOtherRecord(const std::string& token,
                                   const std::string& categoryName,
                                   const std::string& datetime,
                                   double             value,
                                   const std::string& note)
{
    UserData* user = getUserByToken(token);
    if (!user) return false;

    auto it = user->categories.find(categoryName);
    if (it == user->categories.end())
        return false;              // ❌ category 不存在 → 回傳 false

    CategoryItem item;
    item.datetime = datetime;
    item.note     = note;
    item.value    = value;

    it->second.push_back(item);
    saveToFile();
    return true;
}

std::vector<CategoryItem> HealthBackend::getOtherRecords(const std::string& token,
                                                         const std::string& categoryName) const {
    const UserData* user = getUserByToken(token);
    if (!user) return {};
    auto it = user->categories.find(categoryName);
    if (it == user->categories.end()) return {};
    return it->second;
}

bool HealthBackend::updateOtherRecord(const std::string& token,
                                      const std::string& categoryName,
                                      std::size_t       index,
                                      const std::string& newDatetime,
                                      double             newValue,
                                      const std::string& newNote) {
    UserData* user = getUserByToken(token);
    if (!user) return false;
    auto it = user->categories.find(categoryName);
    if (it == user->categories.end()) return false;
    auto& vec = it->second;
    if (index >= vec.size()) return false;

    vec[index].datetime = newDatetime;
    vec[index].note     = newNote;
    vec[index].value    = newValue;
    saveToFile();
    return true;
}

bool HealthBackend::deleteOtherRecord(const std::string& token,
                                      const std::string& categoryName,
                                      std::size_t       index) {
    UserData* user = getUserByToken(token);
    if (!user) return false;
    auto it = user->categories.find(categoryName);
    if (it == user->categories.end()) return false;

    auto& vec = it->second;
    if (index >= vec.size()) return false;

    vec.erase(vec.begin() + static_cast<long>(index));
    saveToFile();
    return true;
}

// 刪掉整個 category，不管裡面有沒有 item
bool HealthBackend::deleteCategory(const std::string& token,
                                   const std::string& categoryName) {
    UserData* user = getUserByToken(token);
    if (!user) return false;
    auto it = user->categories.find(categoryName);
    if (it == user->categories.end()) return false;

    user->categories.erase(it);   // 直接整個刪掉這個 category
    saveToFile();
    return true;
}

} // namespace health