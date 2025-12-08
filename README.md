# Health Backend (C++17)

A simple health tracking backend implemented in **C++17**, with:

- User management (register / login / token)
- BMI calculation
- Water intake records (daily records, weekly average, enough or not)
- Sleep records (last sleep, enough or not)
- Activity records (add & sort by duration)
- HTTP API server built on top of the core backend

本專案將「**核心健康資料邏輯**」與「**HTTP 通訊層**」分開設計：

- `main.cpp`：單純測試後端邏輯（不含 HTTP）
- `server.cpp`：提供 HTTP API，讓前端或其他 client 呼叫
- `/backend`, `/user`, `/records`, `/helpers`：核心 C++ 類別與工具

---

## Folder Structure

```text
health_backend/
├── main.cpp              # 後端核心測試程式（不開 HTTP）
├── server.cpp            # HTTP server 入口（提供 REST API）
├── httplib.h             # Header-only HTTP library (cpp-httplib)
│
├── backend/
│   ├── HealthBackend.hpp
│   └── HealthBackend.cpp
│
├── user/
│   ├── User.hpp
│   ├── User.cpp
│   ├── UserBackend.hpp
│   └── UserBackend.cpp
│
├── records/
│   ├── Water.hpp
│   ├── Water.cpp
│   ├── Sleep.hpp
│   ├── Sleep.cpp
│   ├── Activity.hpp
│   ├── Activity.cpp
│   ├── OtherCategory.hpp
│   └── OtherCategory.cpp
│
└── helpers/
    ├── json.hpp          # Very simple JSON string builder
    ├── validation.hpp    # Input validation helpers
    └── validation.cpp
