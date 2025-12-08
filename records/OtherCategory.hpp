#ifndef OTHER_CATEGORY_HPP
#define OTHER_CATEGORY_HPP

#include <string>
#include <unordered_map>
#include <vector>

struct OtherRecord {
    std::string category;
    std::string date;
    double value;
    std::string note;
};

class OtherCategoryManager {
private:
    // userName -> (categoryName -> records)
    std::unordered_map<std::string,
        std::unordered_map<std::string, std::vector<OtherRecord>>> data;

public:
    bool addRecord(const std::string& userName,
                   const std::string& categoryName,
                   const std::string& date,
                   double value,
                   const std::string& note);

    bool deleteRecord(const std::string& userName,
                      const std::string& categoryName,
                      std::size_t index);

    std::vector<std::string> getCategories(const std::string& userName) const;

    std::vector<OtherRecord> getRecords(const std::string& userName,
                                        const std::string& categoryName) const;
};

#endif
