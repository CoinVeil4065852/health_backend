#ifndef USER_BACKEND_HPP
#define USER_BACKEND_HPP

#include <string>
#include <unordered_map>
#include "User.hpp"

class UserBackend {
private:
    std::unordered_map<std::string, User> usersByName;
    std::unordered_map<std::string, std::string> tokenToUserName;

    std::string generateToken() const;

public:
    UserBackend() = default;

    bool registerUser(const std::string& name,
                      int age,
                      double weightKg,
                      double heightM,
                      const std::string& password);

    std::string login(const std::string& name,
                      const std::string& password);

    bool updateUser(const std::string& name,
                    int newAge,
                    double newWeightKg,
                    double newHeightM,
                    const std::string& newPassword);

    bool verifyToken(const std::string& token) const;

    std::string getUserNameByToken(const std::string& token) const;

    double getUserBMI(const std::string& token) const;
};

#endif
