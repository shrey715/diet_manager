#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

/**
 * User Class
 * This class represents a user of the diet manager application.
 */
class User {
public:
    enum class Gender { MALE, FEMALE, OTHER };
    enum class ActivityLevel { SEDENTARY, LIGHT, MODERATE, ACTIVE, VERY_ACTIVE };
    enum class Goal { LOSE_WEIGHT, MAINTAIN, GAIN_WEIGHT };
    enum class CalorieCalculationMethod { MIFFLIN_ST_JEOR, HARRIS_BENEDICT, WHO_EQUATION };

    User(const string& name = "", int age = 0, Gender gender = Gender::OTHER,
         float height = 0.0f, float weight = 0.0f, 
         ActivityLevel activity = ActivityLevel::MODERATE,
         Goal goal = Goal::MAINTAIN,
         CalorieCalculationMethod calcMethod = CalorieCalculationMethod::MIFFLIN_ST_JEOR);

    // Getters
    const string& getName() const;
    int getAge() const;
    Gender getGender() const;
    float getHeight() const;
    float getWeight() const;
    ActivityLevel getActivityLevel() const;
    Goal getGoal() const;
    CalorieCalculationMethod getCalorieCalculationMethod() const;
    
    // Setters
    void setName(const string& name);
    void setAge(int age);
    void setGender(Gender gender);
    void setHeight(float height);
    void setWeight(float weight);
    void setActivityLevel(ActivityLevel activity);
    void setGoal(Goal goal);
    void setCalorieCalculationMethod(CalorieCalculationMethod method);
    
    // Utility methods
    float calculateBMI() const;
    float calculateBMR() const;
    float calculateBMRHarrisBenedict() const;
    float calculateBMRWHOEquation() const;
    float calculateDailyCalorieNeeds() const;
    float calculateTargetCalories() const;

    // Conversion utilities for enums
    static string activityLevelToString(ActivityLevel level);
    static string goalToString(Goal goal);
    static string genderToString(Gender gender);
    static string calorieMethodToString(CalorieCalculationMethod method);
    static ActivityLevel stringToActivityLevel(const string& str);
    static Goal stringToGoal(const string& str);
    static Gender stringToGender(const string& str);
    static CalorieCalculationMethod stringToCalorieMethod(const string& str);
    
    // Serialization
    json toJson() const;
    static User fromJson(const json& j);

private:
    string name;
    int age;
    Gender gender;
    float height; // in cm
    float weight; // in kg
    ActivityLevel activityLevel;
    Goal goal;
    CalorieCalculationMethod calorieCalcMethod;

    float getActivityMultiplier() const;
    float getGoalCalorieAdjustment() const;
};

#endif // USER_H
