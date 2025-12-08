#ifndef VALIDATION_HPP
#define VALIDATION_HPP

#include <string>

namespace Validation {

bool isValidName(const std::string& name);
bool isValidAge(int age);
bool isValidWeight(double weightKg);
bool isValidHeight(double heightM);
bool isValidPassword(const std::string& pw);
bool isValidDate(const std::string& date); // YYYY-MM-DD 簡單檢查
bool isNonNegative(double value);

}

#endif
