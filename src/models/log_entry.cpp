#include "log_entry.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <random>

using json = nlohmann::json;

LogEntry::LogEntry() : date(std::chrono::system_clock::now()) {
    id = generateId();
}

LogEntry::LogEntry(const std::string& id, const std::chrono::system_clock::time_point& date)
    : id(id), date(date) {
}

std::string LogEntry::getId() const {
    return id;
}

std::chrono::system_clock::time_point LogEntry::getDate() const {
    return date;
}

std::vector<std::pair<std::shared_ptr<Food>, double>> LogEntry::getConsumedFoods() const {
    return consumedFoods;
}

void LogEntry::setId(const std::string& id) {
    this->id = id;
}

void LogEntry::setDate(const std::chrono::system_clock::time_point& date) {
    this->date = date;
}

void LogEntry::addFood(std::shared_ptr<Food> food, double servings) {
    // Check if food already exists, if so, update servings
    auto it = std::find_if(consumedFoods.begin(), consumedFoods.end(),
        [&](const std::pair<std::shared_ptr<Food>, double>& entry) {
            return entry.first->getId() == food->getId();
        });
    
    if (it != consumedFoods.end()) {
        it->second += servings;
    } else {
        consumedFoods.emplace_back(food, servings);
    }
}

void LogEntry::removeFood(const std::string& foodId) {
    consumedFoods.erase(
        std::remove_if(consumedFoods.begin(), consumedFoods.end(),
            [&](const std::pair<std::shared_ptr<Food>, double>& entry) {
                return entry.first->getId() == foodId;
            }),
        consumedFoods.end());
}

double LogEntry::getTotalCalories() const {
    double total = 0.0;
    for (const auto& entry : consumedFoods) {
        total += entry.first->getCaloriesPerServing() * entry.second;
    }
    return total;
}

void LogEntry::toJson(std::ostream& os) const {
    json j;
    j["id"] = id;
    j["date"] = dateToString(date);
    
    json foodsJson = json::array();
    for (const auto& entry : consumedFoods) {
        json foodJson;
        foodJson["food_id"] = entry.first->getId();
        foodJson["servings"] = entry.second;
        foodsJson.push_back(foodJson);
    }
    j["consumed_foods"] = foodsJson;
    
    os << j.dump(4);
}

std::shared_ptr<LogEntry> LogEntry::fromJson(
    const std::string& jsonString, 
    const std::map<std::string, std::shared_ptr<Food>>& foodDatabase) {
    
    json j = json::parse(jsonString);
    
    auto logEntry = std::make_shared<LogEntry>();
    logEntry->setId(j["id"]);
    logEntry->setDate(stringToDate(j["date"]));
    
    for (const auto& foodJson : j["consumed_foods"]) {
        std::string foodId = foodJson["food_id"];
        double servings = foodJson["servings"];
        
        if (foodDatabase.find(foodId) != foodDatabase.end()) {
            logEntry->addFood(foodDatabase.at(foodId), servings);
        }
    }
    
    return logEntry;
}

std::string LogEntry::generateId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static const char* digits = "0123456789abcdef";
    
    std::string uuid;
    uuid.reserve(36);
    
    for (int i = 0; i < 36; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) {
            uuid += '-';
        } else {
            uuid += digits[dis(gen)];
        }
    }
    
    return uuid;
}

std::string LogEntry::dateToString(const std::chrono::system_clock::time_point& date) {
    auto time = std::chrono::system_clock::to_time_t(date);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d");
    return ss.str();
}

std::chrono::system_clock::time_point LogEntry::stringToDate(const std::string& dateStr) {
    std::tm tm = {};
    std::stringstream ss(dateStr);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}
