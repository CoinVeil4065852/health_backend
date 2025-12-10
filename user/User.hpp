#ifndef USER_HPP
#define USER_HPP

#include <string>

class User {
private:
    std::string name;       // 使用者名稱（當作 id）
    int         age{};      // 年齡
    double      weightKg{}; // 體重 (kg)
    double      heightM{};  // 身高 (m)
    std::string password;   // 密碼（純文字，作業等級先這樣）
    std::string gender;     // "male" / "female" / "other"
    std::string token;      // 目前登入用 token（可空字串）

public:
    User() = default;

    User(const std::string& name,
         int age,
         double weightKg,
         double heightM,
         const std::string& password,
         const std::string& gender)
        : name(name),
          age(age),
          weightKg(weightKg),
          heightM(heightM),
          password(password),
          gender(gender) {}

    // ===== getters =====
    const std::string& getName()     const { return name; }
    int                getAge()      const { return age; }
    double             getWeightKg() const { return weightKg; }
    double             getHeightM()  const { return heightM; }
    const std::string& getPassword() const { return password; }
    const std::string& getGender()   const { return gender; }
    const std::string& getToken()    const { return token; }

    // ===== setters =====
    void setAge(int a)                     { age = a; }
    void setWeightKg(double w)             { weightKg = w; }
    void setHeightM(double h)              { heightM = h; }
    void setPassword(const std::string& pw){ password = pw; }
    void setGender(const std::string& g)   { gender = g; }
    void setToken(const std::string& t)    { token = t; }

    // ===== BMI 計算 =====
    double getBMI() const {
        if (heightM <= 0.0) return 0.0;
        return weightKg / (heightM * heightM);
    }
};

#endif // USER_HPP