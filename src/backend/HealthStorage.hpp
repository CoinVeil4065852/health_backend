#pragma once

#include <string>

namespace health {
class HealthBackend;

class HealthStorage {
public:
    static void initStoragePath(HealthBackend& backend);
    static void ensureStorageDirExists(const HealthBackend& backend);
    static void loadFromFile(HealthBackend& backend);
    static void saveToFile(const HealthBackend& backend);
};

} // namespace health
