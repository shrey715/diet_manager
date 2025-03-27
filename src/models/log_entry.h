#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "composite_food.h"

class LogEntry {
private:
    std::string id;
    std::chrono::system_clock::time_point date;
    std::vector<std::pair<std::shared_ptr<Food>, double>> consumedFoods; // Food and servings
    
public:
    LogEntry();
    LogEntry(const std::string& id, const std::chrono::system_clock::time_point& date);
    
    // Getters
    std::string getId() const;
    std::chrono::system_clock::time_point getDate() const;
    std::vector<std::pair<std::shared_ptr<Food>, double>> getConsumedFoods() const;
    
    // Setters
    void setId(const std::string& id);
    void setDate(const std::chrono::system_clock::time_point& date);
    
    // Food management
    void addFood(std::shared_ptr<Food> food, double servings);
    void removeFood(const std::string& foodId);
    
    // Calculations
    double getTotalCalories() const;
    
    // Serialization
    void toJson(std::ostream& os) const;
    static std::shared_ptr<LogEntry> fromJson(
        const std::string& json, 
        const std::map<std::string, std::shared_ptr<Food>>& foodDatabase);
    
    // Utility methods
    static std::string generateId();
    static std::string dateToString(const std::chrono::system_clock::time_point& date);
    static std::chrono::system_clock::time_point stringToDate(const std::string& dateStr);
};

#endif // LOG_ENTRY_H
