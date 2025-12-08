#include "UserBackend.hpp"
#include <random>

bool UserBackend::registerUser(const std::string& name,
                               int age,
                               double weightKg,
                               double heightM,
                               const std::string& password) {
    if (usersByName.count(name) > 0) {
        return false; // 已存在
    }
    usersByName.emplace(name, User(name, age, weightKg, heightM, password));
    return true;
}

std::string UserBackend::generateToken() const {
    static const char chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, static_cast<int>(sizeof(chars) - 2));

    std::string token;
    token.reserve(24);
    for (int i = 0; i < 24; ++i) {
        token.push_back(chars[dist(rng)]);
    }
    return token;
}

std::string UserBackend::login(const std::string& name,
                               const std::string& password) {
    auto it = usersByName.find(name);
    if (it == usersByName.end()) return "INVALID";

    if (it->second.getPassword() != password) {
        return "INVALID";
    }

    std::string token = generateToken();
    tokenToUserName[token] = name;
    return token;
}

bool UserBackend::updateUser(const std::string& name,
                             int newAge,
                             double newWeightKg,
                             double newHeightM,
                             const std::string& newPassword) {
    auto it = usersByName.find(name);
    if (it == usersByName.end()) return false;

    it->second.setAge(newAge);
    it->second.setWeight(newWeightKg);
    it->second.setHeight(newHeightM);
    it->second.setPassword(newPassword);
    return true;
}

bool UserBackend::verifyToken(const std::string& token) const {
    return tokenToUserName.count(token) > 0;
}

std::string UserBackend::getUserNameByToken(const std::string& token) const {
    auto it = tokenToUserName.find(token);
    if (it == tokenToUserName.end()) return "";
    return it->second;
}

double UserBackend::getUserBMI(const std::string& token) const {
    auto it = tokenToUserName.find(token);
    if (it == tokenToUserName.end()) return 0.0;

    const std::string& name = it->second;
    auto itUser = usersByName.find(name);
    if (itUser == usersByName.end()) return 0.0;

    return itUser->second.getBMI();
}
