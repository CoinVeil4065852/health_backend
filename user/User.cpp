#include "User.hpp"
#include <sstream>

User::User(const std::string& name,
           int age,
           double weightKg,
           double heightM,
           const std::string& password)
    : name(name),
      age(age),
      weightKg(weightKg),
      heightM(heightM),
      password(password) {}

const std::string& User::getName() const {
    return name;
}

int User::getAge() const {
    return age;
}

double User::getWeight() const {
    return weightKg;
}

double User::getHeight() const {
    return heightM;
}

const std::string& User::getPassword() const {
    return password;
}

void User::setAge(int a) {
    age = a;
}

void User::setWeight(double w) {
    weightKg = w;
}

void User::setHeight(double h) {
    heightM = h;
}

void User::setPassword(const std::string& pw) {
    password = pw;
}

double User::getBMI() const {
    if (heightM <= 0.0) return 0.0;
    return weightKg / (heightM * heightM);
}

std::string User::toJSON() const {
    std::ostringstream os;
    os << "{"
       << "\"name\":\"" << name << "\","
       << "\"age\":" << age << ","
       << "\"weightKg\":" << weightKg << ","
       << "\"heightM\":" << heightM
       << "}";
    return os.str();
}
