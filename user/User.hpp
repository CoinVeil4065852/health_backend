#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {
private:
    std::string name;
    int age;
    double weightKg;
    double heightM;
    std::string password;  // 目前純文字

public:
    User() = default;

    User(const std::string& name,
         int age,
         double weightKg,
         double heightM,
         const std::string& password);

    const std::string& getName() const;
    int getAge() const;
    double getWeight() const;
    double getHeight() const;
    const std::string& getPassword() const;

    void setAge(int age);
    void setWeight(double weightKg);
    void setHeight(double heightM);
    void setPassword(const std::string& pw);

    double getBMI() const;

    std::string toJSON() const;
};

#endif
