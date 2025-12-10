# Health Backend (C++17)

A simple health tracking backend implemented in C++17, with:
	•	User management (register / login / token)
	•	BMI calculation
	•	Water intake records (daily records, weekly average, enough or not)
	•	Sleep records (last sleep, enough or not)
	•	Activity records (add, edit, delete, sort)
	•	Custom categories (create, add record, edit, delete)
	•	HTTP API server built on top of the core backend
	•	Full persistent storage using storage.json

本專案將「核心健康資料邏輯」與「HTTP 通訊層」分開設計：
	•	main.cpp：單純測試後端邏輯（不含 HTTP）
	•	server.cpp：提供 HTTP API，讓前端或其他 client 呼叫
	•	/backend, /user, /records, /helpers：核心 C++ 類別與工具
	•	/data/storage.json：所有資料持久化存檔（自動讀寫）

---

## Folder Structure

health_backend/
├── main.cpp                     # Core backend testing (no HTTP)
├── server.cpp                   # HTTP server entry point (REST API)
├── httplib.h                    # cpp-httplib (header-only HTTP library)
│
├── external/
│   └── json.hpp                 # nlohmann JSON header-only library
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
├── helpers/
│   ├── validation.hpp
│   ├── validation.cpp
│   ├── json.hpp                 # (replaced by nlohmann/json)
│
└── data/
├── storage.json             # Auto-generated persistent storage
└── storage.example.json     # Example layout (no real data)

---

## Build Instructions

# Make sure you are inside the project folder:

cd health_backend

# Compile the HTTP server:

g++ -std=c++17 \
  server.cpp \
  backend/HealthBackend.cpp \
  user/User.cpp \
  user/UserBackend.cpp \
  records/Water.cpp \
  records/Sleep.cpp \
  records/Activity.cpp \
  records/OtherCategory.cpp \
  helpers/validation.cpp \
  -o server_app


---

## Run the Server

./server_app

預期輸出：

Server started at http://0.0.0.0:8080

你可在瀏覽器開：

http://localhost:8080

若要同 Wi-Fi 前端電腦訪問：

http://<你的IP>:8080


---

## Persistent Storage

所有資料會自動存入：

/data/storage.json

	•	伺服器啟動：自動讀取
	•	有任何新增／修改／刪除：自動寫回
	•	刪除檔案 → 重置所有資料

---

## API Authentication

登入取得 token。

所有 API（除了 register/login）都必須加上：

Token: <token>

---

Notes
	•	main.cpp 用於純後端邏輯測試，不提供 HTTP。
	•	server.cpp 才是提供 REST API 的入口。
	•	資料儲存位置：data/storage.json。
	•	JSON parser 使用 nlohmann/json（header-only）。



