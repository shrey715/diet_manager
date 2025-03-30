#ifndef FOOD_ENTRY_UTILS_H
#define FOOD_ENTRY_UTILS_H

#include "../models/log_entry.h"

/**
 * @namespace FoodEntryUtils
 * @brief Helper functions for working with food entries
 */
namespace FoodEntryUtils {
    /**
     * @brief Get the meal type of a food entry
     * @param entry Food entry to get meal type from
     * @return Meal type
     */
    inline MealType getMealType(const LogEntry::FoodEntry& entry) {
        return std::get<2>(entry);
    }
    
    /**
     * @brief Get the servings of a food entry
     * @param entry Food entry to get servings from
     * @return Number of servings
     */
    inline double getServings(const LogEntry::FoodEntry& entry) {
        return std::get<1>(entry);
    }
    
    /**
     * @brief Get the food from a food entry
     * @param entry Food entry to get food from
     * @return Food pointer
     */
    inline std::shared_ptr<Food> getFood(const LogEntry::FoodEntry& entry) {
        return std::get<0>(entry);
    }
    
    /**
     * @brief Calculate calories in a food entry
     * @param entry Food entry to calculate calories for
     * @return Total calories
     */
    inline double getCalories(const LogEntry::FoodEntry& entry) {
        return getFood(entry)->getCaloriesPerServing() * getServings(entry);
    }
}

#endif // FOOD_ENTRY_UTILS_H
