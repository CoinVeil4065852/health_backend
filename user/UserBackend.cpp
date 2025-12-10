#include "UserBackend.hpp"
#include <random>
#include <chrono>

// ===== 產生隨機 token =====
std::string UserBackend::generateToken() const {
    static const char kChars[] =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::mt19937_64 rng(
        static_cast<unsigned long>(
            std::chrono::high_resolution_clock::now().time_since_epoch().count()
        )
    );
    std::uniform_int_distribution<int> dist(0, static_cast<int>(sizeof(kChars) - 2));

    std::string token;
    token.reserve(24);
    for (int i = 0; i < 24; ++i) {
        token.push_back(kChars[dist(rng)]);
    }
    return token;
}

// ===== 註冊 =====
bool UserBackend::registerUser(const std::string& name,
                               int age,
                               double weightKg,
                               double heightM,
                               const std::string& password,
                               const std::string& gender) {
    // 不允許同名 user
    if (users.find(name) != users.end()) {
        return false;
    }

    User u{name, age, weightKg, heightM, password, gender};

    // 一開始就給他一個 token（也可以等登入時給）
    std::string token = generateToken();
    u.setToken(token);

    users[name] = u;
    tokenToName[token] = name;
    return true;
}

// ===== 登入 =====
std::string UserBackend::login(const std::string& name,
                               const std::string& password) {
    auto it = users.find(name);
    if (it == users.end()) {
        return "INVALID";
    }
    if (it->second.getPassword() != password) {
        return "INVALID";
    }

    // 有舊 token 就沿用，沒有就產生新 token
    if (!it->second.getToken().empty()) {
        return it->second.getToken();
    }

    std::string token = generateToken();
    it->second.setToken(token);
    tokenToName[token] = name;
    return token;
}

// ===== 更新使用者（用 name 當 key）=====
bool UserBackend::updateUser(const std::string& name,
                             int newAge,
                             double newWeightKg,
                             double newHeightM,
                             const std::string& newPassword,
                             const std::string& newGender) {
    auto it = users.find(name);
    if (it == users.end()) return false;

    it->second.setAge(newAge);
    it->second.setWeightKg(newWeightKg);
    it->second.setHeightM(newHeightM);
    it->second.setPassword(newPassword);
    it->second.setGender(newGender);
    return true;
}

// ===== 刪除使用者 =====
bool UserBackend::deleteUser(const std::string& name) {
    auto it = users.find(name);
    if (it == users.end()) return false;

    // 清掉對應的 token
    std::string token = it->second.getToken();
    if (!token.empty()) {
        tokenToName.erase(token);
    }

    users.erase(it);
    return true;
}

// ===== 用 token 算 BMI =====
double UserBackend::getUserBMI(const std::string& token) const {
    std::string name = getUserNameByToken(token);
    if (name.empty()) return 0.0;

    auto it = users.find(name);
    if (it == users.end()) return 0.0;

    return it->second.getBMI();
}

// ===== 用 token 找 userName =====
std::string UserBackend::getUserNameByToken(const std::string& token) const {
    auto it = tokenToName.find(token);
    if (it == tokenToName.end()) return "";
    return it->second;
}

// ===== 用 name 找 User（for profile 用）=====
const User* UserBackend::findUserByName(const std::string& name) const {
    auto it = users.find(name);
    if (it == users.end()) return nullptr;
    return &it->second;
}

// ===== 存成 JSON =====
json UserBackend::toJson() const {
    json arr = json::array();

    for (const auto& [name, u] : users) {
        json ju;
        ju["name"]      = u.getName();
        ju["age"]       = u.getAge();
        ju["weightKg"]  = u.getWeightKg();
        ju["heightM"]   = u.getHeightM();
        ju["password"]  = u.getPassword();
        ju["gender"]    = u.getGender();     // ★ 新增：gender
        ju["token"]     = u.getToken();      // token 也一併存起來（可以不存，看你作業需求）

        arr.push_back(ju);
    }

    return arr;
}

// ===== 從 JSON 載入 =====
void UserBackend::fromJson(const json& j) {
    users.clear();
    tokenToName.clear();

    if (!j.is_array()) return;

    for (const auto& ju : j) {
        std::string name     = ju.value("name", "");
        int         age      = ju.value("age", 0);
        double      weightKg = ju.value("weightKg", 0.0);
        double      heightM  = ju.value("heightM", 0.0);
        std::string password = ju.value("password", "");
        std::string gender   = ju.value("gender", "other");
        std::string token    = ju.value("token", "");

        if (name.empty()) continue;

        User u{name, age, weightKg, heightM, password, gender};
        u.setToken(token);

        users[name] = u;

        if (!token.empty()) {
            tokenToName[token] = name;
        }
    }
}