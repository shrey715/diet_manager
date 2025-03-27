#include "log_entry.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <random>
#include <iostream> // Added this include for std::cerr

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

const std::vector<std::tuple<std::shared_ptr<Food>, double, MealType>>& LogEntry::getConsumedFoods() const {
    return consumedFoods;
}

void LogEntry::setId(const std::string& id) {
    this->id = id;
}

void LogEntry::setDate(const std::chrono::system_clock::time_point& date) {
    this->date = date;
}

void LogEntry::addFood(std::shared_ptr<Food> food, double servings, MealType mealType) {
    // Check if food already exists, if so, update servings
    auto it = std::find_if(consumedFoods.begin(), consumedFoods.end(),
        [&](const std::tuple<std::shared_ptr<Food>, double, MealType>& entry) {
            return std::get<0>(entry)->getId() == food->getId();
        });
    
    if (it != consumedFoods.end()) {
        // Update servings
        std::get<1>(*it) += servings;
    } else {
        // Add new food entry
        consumedFoods.emplace_back(food, servings, mealType);
    }
}

void LogEntry::removeFood(const std::string& foodId) {
    consumedFoods.erase(
        std::remove_if(consumedFoods.begin(), consumedFoods.end(),
            [&](const std::tuple<std::shared_ptr<Food>, double, MealType>& entry) {
                return std::get<0>(entry)->getId() == foodId;
            }),
        consumedFoods.end());
}

double LogEntry::getTotalCalories() const {
    double total = 0.0;
    for (const auto& entry : consumedFoods) {
        total += std::get<0>(entry)->getCaloriesPerServing() * std::get<1>(entry);
    }
    return total;
}

void LogEntry::updateFoodMealType(const std::string& foodId, MealType mealType) {
    for (auto& entry : consumedFoods) {
        if (std::get<0>(entry)->getId() == foodId) {
            std::get<2>(entry) = mealType;
            break;
        }
    }
}

double LogEntry::getCaloriesForMeal(MealType mealType) const {
    double total = 0.0;
    for (const auto& entry : consumedFoods) {
        if (std::get<2>(entry) == mealType) {
            total += std::get<0>(entry)->getCaloriesPerServing() * std::get<1>(entry);
        }
    }
    return total;
}

std::map<MealType, double> LogEntry::getCaloriesByMealType() const {
    std::map<MealType, double> caloriesByMeal;
    for (const auto& entry : consumedFoods) {
        MealType mealType = std::get<2>(entry);
        double calories = std::get<0>(entry)->getCaloriesPerServing() * std::get<1>(entry);
        caloriesByMeal[mealType] += calories;
    }
    return caloriesByMeal;
}

void LogEntry::toJson(std::ostream& os) const {
    try {
        json j;
        j["id"] = id;
        j["date"] = dateToString(date);
        
        json foodsJson = json::array();
        for (const auto& entry : consumedFoods) {
            if (std::get<0>(entry)) {  // Make sure food pointer is valid
                json foodJson;
                foodJson["food_id"] = std::get<0>(entry)->getId();
                foodJson["servings"] = std::get<1>(entry);
                foodJson["meal_type"] = mealTypeToString(std::get<2>(entry));
                foodsJson.push_back(foodJson);
            }
        }
        j["consumed_foods"] = foodsJson;
        
        os << j.dump(4);
    } catch (const std::exception& e) {
        std::cerr << "Error generating JSON for LogEntry: " << e.what() << std::endl;
        // Output a minimal valid JSON object
        os << "{\"id\":\"" << id << "\",\"date\":\"" << dateToString(date) << "\",\"consumed_foods\":[]}";
    }
}

std::shared_ptr<LogEntry> LogEntry::fromJson(
    const std::string& jsonString, 
    const std::map<std::string, std::shared_ptr<Food>>& foodDatabase) {
    
    try {
        json j = json::parse(jsonString);
        
        auto logEntry = std::make_shared<LogEntry>();
        logEntry->setId(j["id"]);
        logEntry->setDate(stringToDate(j["date"]));
        
        if (j.contains("consumed_foods") && j["consumed_foods"].is_array()) {
            for (const auto& foodJson : j["consumed_foods"]) {
                if (!foodJson.contains("food_id") || !foodJson.contains("servings")) {
                    std::cerr << "Warning: Invalid food entry in log" << std::endl;
                    continue;
                }
                
                std::string foodId = foodJson["food_id"];
                double servings = foodJson["servings"];
                
                // Default to OTHER if meal_type is missing
                MealType mealType = MealType::OTHER;
                if (foodJson.contains("meal_type")) {
                    mealType = mealTypeFromString(foodJson["meal_type"]);
                }
                
                if (foodDatabase.find(foodId) != foodDatabase.end()) {
                    auto foodPtr = foodDatabase.at(foodId);
                    if (foodPtr) {
                        logEntry->addFood(foodPtr, servings, mealType);
                    }
                } else {
                    std::cerr << "Warning: Food '" << foodId << "' not found in database" << std::endl;
                }
            }
        }
        
        return logEntry;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing LogEntry from JSON: " << e.what() << std::endl;
        return std::make_shared<LogEntry>(); // Return a valid but empty log entry
    }
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

std::string LogEntry::mealTypeToString(MealType mealType) {
    switch (mealType) {
        case MealType::BREAKFAST: return "Breakfast";
        case MealType::LUNCH: return "Lunch";
        case MealType::DINNER: return "Dinner";
        case MealType::SNACK: return "Snack";
        default: return "Other";
    }
}

MealType LogEntry::mealTypeFromString(const std::string& mealTypeStr) {
    std::string lower;
    std::transform(mealTypeStr.begin(), mealTypeStr.end(), std::back_inserter(lower), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower == "breakfast") return MealType::BREAKFAST;
    if (lower == "lunch") return MealType::LUNCH;
    if (lower == "dinner") return MealType::DINNER;
    if (lower == "snack") return MealType::SNACK;
    return MealType::OTHER;
}
