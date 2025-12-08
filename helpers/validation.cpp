#include "validation.hpp"

namespace Validation {

bool isValidName(const std::string& name) {
    return !name.empty();
}

bool isValidAge(int age) {
    return age > 0 && age < 150;
}

bool isValidWeight(double weightKg) {
    return weightKg > 0.0 && weightKg < 500.0;
}

bool isValidHeight(double heightM) {
    return heightM > 0.0 && heightM < 3.0;
}

bool isValidPassword(const std::string& pw) {
    return pw.size() >= 4;
}

bool isValidDate(const std::string& date) {
    // 超級簡單的格式檢查：YYYY-MM-DD，沒檢查月份天數合理性
    if (date.size() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    for (std::size_t i = 0; i < date.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (date[i] < '0' || date[i] > '9') return false;
    }
    return true;
}

bool isNonNegative(double value) {
    return value >= 0.0;
}

} // namespace Validation
