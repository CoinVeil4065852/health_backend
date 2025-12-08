#include <iostream>
#include "backend/HealthBackend.hpp"

int main() {
    HealthBackend backend;

    // 註冊一個使用者
    bool ok = backend.registerUser("Alice", 20, 50.0, 1.60, "abc123");
    if (!ok) {
        std::cout << "Register failed\n";
        return 0;
    }

    // 登入取得 token
    std::string token = backend.login("Alice", "abc123");
    if (token == "INVALID") {
        std::cout << "Login failed\n";
        return 0;
    }

    std::cout << "Login token: " << token << "\n";

    // 查看 BMI
    double bmi = backend.getBMI(token);
    std::cout << "BMI = " << bmi << "\n";

    // 加一些水紀錄
    backend.addWater(token, "2025-12-01", 1500);
    backend.addWater(token, "2025-12-02", 1800);
    backend.addWater(token, "2025-12-03", 2000);

    double avgWater = backend.getWeeklyAverageWater(token);
    std::cout << "Weekly avg water (last up to 7 records) = "
              << avgWater << " ml/day\n";
    std::cout << "Enough water? "
              << (backend.isWaterEnough(token, 1500.0) ? "yes" : "no")
              << "\n";

    // 加睡眠紀錄
    backend.addSleep(token, "2025-12-01", 6.5);
    backend.addSleep(token, "2025-12-02", 7.5);

    std::cout << "Last sleep hours = "
              << backend.getLastSleepHours(token) << "\n";
    std::cout << "Enough sleep (>=7h)? "
              << (backend.isSleepEnough(token, 7.0) ? "yes" : "no")
              << "\n";

    // 加活動紀錄
    backend.addActivity(token, "2025-12-01", 30, "moderate");
    backend.addActivity(token, "2025-12-02", 60, "vigorous");
    backend.sortActivityByDuration(token);

    auto activities = backend.getAllActivity(token);
    std::cout << "Activities (sorted by duration):\n";
    for (const auto& a : activities) {
        std::cout << "  " << a.date << " - "
                  << a.minutes << " min - "
                  << a.intensity << "\n";
    }

    return 0;
}
