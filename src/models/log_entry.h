#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <tuple>
#include "composite_food.h"

/**
 * @enum MealType
 * @brief Categories of meals for food log entries
 */
enum class MealType {
    BREAKFAST,
    LUNCH,
    DINNER,
    SNACK,
    OTHER
};

/**
 * @class LogEntry
 * @brief Represents a single day's food log
 */
class LogEntry {
public:
    /**
     * @brief Type definition for a food entry
     */
    using FoodEntry = std::tuple<std::shared_ptr<Food>, double, MealType>;
    
private:
    std::string id;
    std::chrono::system_clock::time_point date;
    std::vector<FoodEntry> consumedFoods;
    
public:
    /**
     * @brief Default constructor - creates log for current date
     */
    LogEntry();
    
    /**
     * @brief Constructor with ID and date
     */
    LogEntry(const std::string& id, const std::chrono::system_clock::time_point& date);
    
    // Basic getters & setters
    std::string getId() const;
    std::chrono::system_clock::time_point getDate() const;
    const std::vector<FoodEntry>& getConsumedFoods() const;
    void setId(const std::string& id);
    void setDate(const std::chrono::system_clock::time_point& date);
    
    /**
     * @brief Add food to this log entry
     */
    void addFood(std::shared_ptr<Food> food, double servings, MealType mealType = MealType::OTHER);
    
    /**
     * @brief Remove food from this log entry by ID
     */
    void removeFood(const std::string& foodId);
    
    /**
     * @brief Update meal type for a specific food
     */
    void updateFoodMealType(const std::string& foodId, MealType mealType);
    
    /**
     * @brief Get total calories in this log entry
     */
    double getTotalCalories() const;
    
    /**
     * @brief Get calories for a specific meal type
     */
    double getCaloriesForMeal(MealType mealType) const;
    
    /**
     * @brief Get calories organized by meal type
     */
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
