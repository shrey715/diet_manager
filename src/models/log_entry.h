#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <tuple>
#include "composite_food.h"

// Meal types
enum class MealType {
    BREAKFAST,
    LUNCH,
    DINNER,
    SNACK,
    OTHER
};

class LogEntry {
private:
    std::string id;
    std::chrono::system_clock::time_point date;
    // Store food, servings, and meal type
    std::vector<std::tuple<std::shared_ptr<Food>, double, MealType>> consumedFoods;
    
public:
    LogEntry();
    LogEntry(const std::string& id, const std::chrono::system_clock::time_point& date);
    
    // Getters
    std::string getId() const;
    std::chrono::system_clock::time_point getDate() const;
    const std::vector<std::tuple<std::shared_ptr<Food>, double, MealType>>& getConsumedFoods() const;
    
    // Setters
    void setId(const std::string& id);
    void setDate(const std::chrono::system_clock::time_point& date);
    
    // Food management
    void addFood(std::shared_ptr<Food> food, double servings, MealType mealType = MealType::OTHER);
    void removeFood(const std::string& foodId);
    void updateFoodMealType(const std::string& foodId, MealType mealType);
    
    // Calculations
    double getTotalCalories() const;
    double getCaloriesForMeal(MealType mealType) const;
    std::map<MealType, double> getCaloriesByMealType() const;
    
    // Serialization
    void toJson(std::ostream& os) const;
    static std::shared_ptr<LogEntry> fromJson(
        const std::string& json, 
        const std::map<std::string, std::shared_ptr<Food>>& foodDatabase);
    
    // Utility methods
    static std::string generateId();
    static std::string dateToString(const std::chrono::system_clock::time_point& date);
    static std::chrono::system_clock::time_point stringToDate(const std::string& dateStr);
    static std::string mealTypeToString(MealType mealType);
    static MealType mealTypeFromString(const std::string& mealTypeStr);
};

#endif // LOG_ENTRY_H
