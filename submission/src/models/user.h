#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

// Config variable for controlling how long a day lasts (in seconds)
const unsigned int DAY_LENGTH = 10; // 24 hours = 86400 seconds

/**
 * DailyMetric struct
 * Represents a daily record of user metrics
 */
struct DailyMetric {
    time_t timestamp;
    float weight;
    int age;
    int activityLevel; // Stored as an integer equivalent of ActivityLevel enum
    
    json toJson() const {
        json j;
        j["timestamp"] = timestamp;
        j["weight"] = weight;
        j["age"] = age;
        j["activityLevel"] = activityLevel;
        return j;
    }
    
    static DailyMetric fromJson(const json& j) {
        DailyMetric metric;
        metric.timestamp = j.value("timestamp", time(nullptr));
        metric.weight = j.value("weight", 0.0f);
        metric.age = j.value("age", 0);
        metric.activityLevel = j.value("activityLevel", 2); // Default to MODERATE
        return metric;
    }
};

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
    
    // Daily tracking methods
    bool needsDailyUpdate() const;
    void ensureDailyRecordExists();
    time_t getLastUpdateTime() const;
    void updateDailyRecord();
    const vector<DailyMetric>& getDailyMetrics() const;
    string getFormattedDate(time_t timestamp) const;
    time_t getCurrentDay() const;
    
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
    
    // Time-series data for daily metrics
    vector<DailyMetric> dailyMetrics;
    time_t lastUpdateTime;
    
    float getActivityMultiplier() const;
    float getGoalCalorieAdjustment() const;
};

#endif // USER_H
