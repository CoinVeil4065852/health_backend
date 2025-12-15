#include <iostream>
#include <string>

#include "backend/HealthBackend.hpp"

// 簡單的後端核心測試程式（不開 HTTP）
// 這個版本「不呼叫 registerUser」，只用既有使用者做測試
int main() {
    HealthBackend backend;

    std::cout << "=== HealthBackend core test (no HTTP) ===\n";

    // 1. 嘗試登入一個既有的使用者
    //    ⚠️ 請確認這個 user 已經用 HTTP /register 建立過，或改成你自己的帳號
    std::string name     = "testUser";
    std::string password = "abc123";

    std::string token = backend.login(name, password);
    if (token == "INVALID") {
        std::cout << "[LOGIN] failed for user: " << name << "\n";
        std::cout << "請先用 HTTP /register 建立這個使用者再來跑 main_app\n";
        return 0;
    }

    std::cout << "[LOGIN] success, token = " << token << "\n";

    // 2. 測試 BMI
    double bmi = backend.getBMI(token);
    std::cout << "[BMI] = " << bmi << "\n";

    // =========================
    // 3. 測試 Water records
    // =========================
    std::cout << "\n=== Water records test ===\n";

    // 新增幾筆測試資料（如果你不想改資料，也可以註解掉）
    backend.addWater(token, "2025-12-10T08:00:00Z", 250.0);
    backend.addWater(token, "2025-12-10T12:00:00Z", 500.0);
    backend.addWater(token, "2025-12-11T09:30:00Z", 300.0);

    auto waters = backend.getAllWater(token);
    std::cout << "Current water records:\n";
    for (std::size_t i = 0; i < waters.size(); ++i) {
        const auto &w = waters[i];
        std::cout << "  [" << i << "] "
                  << w.datetime << " -> " << w.amountMl << " ml\n";
    }

    // =========================
    // 4. 測試 Sleep records
    // =========================
    std::cout << "\n=== Sleep records test ===\n";

    backend.addSleep(token, "2025-12-09T23:00:00Z", 7.5);
    backend.addSleep(token, "2025-12-10T23:30:00Z", 6.0);

    auto sleeps = backend.getAllSleep(token);
    std::cout << "Current sleep records:\n";
    for (std::size_t i = 0; i < sleeps.size(); ++i) {
        const auto &s = sleeps[i];
        std::cout << "  [" << i << "] "
                  << s.datetime << " -> " << s.hours << " hours\n";
    }

    // =========================
    // 5. 測試 Activity records
    // =========================
    std::cout << "\n=== Activity records test ===\n";

    backend.addActivity(token, "2025-12-10T18:00:00Z", 30, "moderate");
    backend.addActivity(token, "2025-12-11T07:30:00Z", 45, "high");

    auto acts = backend.getAllActivity(token);
    std::cout << "Current activity records:\n";
    for (std::size_t i = 0; i < acts.size(); ++i) {
        const auto &a = acts[i];
        std::cout << "  [" << i << "] "
                  << a.datetime << " -> "
                  << a.minutes << " min, intensity = " << a.intensity << "\n";
    }

    // =========================
    // 6. 測試 Other Category
    // =========================
    std::cout << "\n=== Other category test ===\n";

    backend.addOtherRecord(
        token,
        "Eating",                         // categoryName
        "2025-12-10T12:00:00Z",           // datetime
        0.0,                              // value 先放 0.0（前端看不到）
        "lunch burger"                    // note
    );

    auto cats = backend.getOtherCategories(token);
    std::cout << "Categories:\n";
    for (const auto &c : cats) {
        std::cout << "  - " << c << "\n";
    }

    auto eatingItems = backend.getOtherRecords(token, "Eating");
    std::cout << "Items in category 'Eating':\n";
    for (std::size_t i = 0; i < eatingItems.size(); ++i) {
        const auto &item = eatingItems[i];
        std::cout << "  [" << i << "] "
                  << item.datetime
                  << " note = " << item.note
                  << " (value = " << item.value << ")\n";
    }

    std::cout << "\n=== Core test finished ===\n";
    return 0;
}