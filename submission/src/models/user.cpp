#include "user.h"
#include <ctime>
#include <iomanip>
#include <sstream>

/**
 * User Class Constructor
 * @param name The name of the user
 * @param age The age of the user in years
 * @param gender The gender of the user
 * @param height The height of the user in cm
 * @param weight The weight of the user in kg
 * @param activity The activity level of the user
 * @param goal The weight goal of the user
 * @param calcMethod The calorie calculation method to use
 * This constructor initializes the User object with personal information.
 */
User::User(const string& name, int age, Gender gender, float height, 
           float weight, ActivityLevel activity, Goal goal, CalorieCalculationMethod calcMethod)
    : name(name), age(age), gender(gender), height(height), 
      weight(weight), activityLevel(activity), goal(goal), calorieCalcMethod(calcMethod),
      lastUpdateTime(time(nullptr)) {
    
    // Initialize with first daily record
    if (weight > 0 && age > 0) {
        updateDailyRecord();
    }
}

/**
 * getName Method
 * @return The name of the user
 */
const string& User::getName() const {
    return name;
}

/**
 * getAge Method
 * @return The age of the user
 */
int User::getAge() const {
    // Return current age or last recorded age from dailyMetrics
    if (!dailyMetrics.empty()) {
        return dailyMetrics.back().age;
    }
    return age;
}

/**
 * getGender Method
 * @return The gender of the user
 */
User::Gender User::getGender() const {
    return gender;
}

/**
 * getHeight Method
 * @return The height of the user in cm
 */
float User::getHeight() const {
    return height;
}

/**
 * getWeight Method
 * @return The weight of the user in kg
 */
float User::getWeight() const {
    // Return current weight or last recorded weight from dailyMetrics
    if (!dailyMetrics.empty()) {
        return dailyMetrics.back().weight;
    }
    return weight;
}

/**
 * getActivityLevel Method
 * @return The activity level of the user
 */
User::ActivityLevel User::getActivityLevel() const {
    // Return current activity level or last recorded level from dailyMetrics
    if (!dailyMetrics.empty()) {
        return static_cast<ActivityLevel>(dailyMetrics.back().activityLevel);
    }
    return activityLevel;
}

/**
 * getGoal Method
 * @return The weight goal of the user
 */
User::Goal User::getGoal() const {
    return goal;
}

/**
 * getCalorieCalculationMethod Method
 * @return The calorie calculation method used for this user
 */
User::CalorieCalculationMethod User::getCalorieCalculationMethod() const {
    return calorieCalcMethod;
}

/**
 * setName Method
 * @param name The new name of the user
 */
void User::setName(const string& name) {
    this->name = name;
}

/**
 * setAge Method
 * @param age The new age of the user
 */
void User::setAge(int age) {
    // Update member variable and ensure today's daily record exists with updated value
    this->age = age;
    ensureDailyRecordExists();
    dailyMetrics.back().age = age;
}

/**
 * setGender Method
 * @param gender The new gender of the user
 */
void User::setGender(Gender gender) {
    this->gender = gender;
}

/**
 * setHeight Method
 * @param height The new height of the user in cm
 */
void User::setHeight(float height) {
    this->height = height;
}

/**
 * setWeight Method
 * @param weight The new weight of the user in kg
 */
void User::setWeight(float weight) {
    // Update member variable and ensure today's daily record exists with updated value
    this->weight = weight;
    ensureDailyRecordExists();
    dailyMetrics.back().weight = weight;
}

/**
 * setActivityLevel Method
 * @param activity The new activity level of the user
 */
void User::setActivityLevel(ActivityLevel activity) {
    // Update member variable and ensure today's daily record exists with updated value
    this->activityLevel = activity;
    ensureDailyRecordExists();
    dailyMetrics.back().activityLevel = static_cast<int>(activity);
}

/**
 * setGoal Method
 * @param goal The new weight goal of the user
 */
void User::setGoal(Goal goal) {
    this->goal = goal;
}

/**
 * setCalorieCalculationMethod Method
 * @param method The new calorie calculation method to use
 */
void User::setCalorieCalculationMethod(CalorieCalculationMethod method) {
    this->calorieCalcMethod = method;
}

/**
 * calculateBMI Method
 * @return The body mass index (BMI) of the user
 * This method calculates the BMI based on weight (kg) and height (cm).
 */
float User::calculateBMI() const {
    if (height <= 0) return 0;
    float heightInMeters = height / 100.0f;
    return weight / (heightInMeters * heightInMeters);
}

/**
 * calculateBMR Method
 * @return The basal metabolic rate (BMR) of the user in calories
 * This method calculates the BMR using the appropriate method based on user preference.
 */
float User::calculateBMR() const {
    switch (calorieCalcMethod) {
        case CalorieCalculationMethod::HARRIS_BENEDICT:
            return calculateBMRHarrisBenedict();
        case CalorieCalculationMethod::WHO_EQUATION:
            return calculateBMRWHOEquation();
        case CalorieCalculationMethod::MIFFLIN_ST_JEOR:
        default:
            // Use Mifflin-St Jeor as default (already implemented)
            if (gender == Gender::MALE) {
                return (10.0f * weight) + (6.25f * height) - (5.0f * age) + 5.0f;
            } else {
                return (10.0f * weight) + (6.25f * height) - (5.0f * age) - 161.0f;
            }
    }
}

/**
 * calculateBMRHarrisBenedict Method
 * @return The BMR using the Harris-Benedict equation
 * This method calculates BMR using the revised Harris-Benedict equation (1984)
 */
float User::calculateBMRHarrisBenedict() const {
    if (gender == Gender::MALE) {
        // Male: BMR = 88.362 + (13.397 × weight in kg) + (4.799 × height in cm) - (5.677 × age in years)
        return 88.362f + (13.397f * weight) + (4.799f * height) - (5.677f * age);
    } else {
        // Female: BMR = 447.593 + (9.247 × weight in kg) + (3.098 × height in cm) - (4.330 × age in years)
        return 447.593f + (9.247f * weight) + (3.098f * height) - (4.330f * age);
    }
}

/**
 * calculateBMRWHOEquation Method
 * @return The BMR using the WHO equation
 * This method calculates BMR using the World Health Organization (WHO) equations
 */
float User::calculateBMRWHOEquation() const {
    if (gender == Gender::MALE) {
        // WHO equations for males based on age ranges
        if (age < 3) {
            return (60.9f * weight) - 54;
        } else if (age < 10) {
            return (22.7f * weight) + 495;
        } else if (age < 18) {
            return (17.5f * weight) + 651;
        } else if (age < 30) {
            return (15.3f * weight) + 679;
        } else if (age < 60) {
            return (11.6f * weight) + 879;
        } else {
            return (13.5f * weight) + 487;
        }
    } else {
        // WHO equations for females based on age ranges
        if (age < 3) {
            return (61.0f * weight) - 51;
        } else if (age < 10) {
            return (22.5f * weight) + 499;
        } else if (age < 18) {
            return (12.2f * weight) + 746;
        } else if (age < 30) {
            return (14.7f * weight) + 496;
        } else if (age < 60) {
            return (8.7f * weight) + 829;
        } else {
            return (10.5f * weight) + 596;
        }
    }
}

/**
 * getActivityMultiplier Method
 * @return The activity multiplier based on the user's activity level
 * This method returns a multiplier to adjust calorie needs based on activity level.
 * Source for multipliers: https://mohap.gov.ae/en/awareness-centre/daily-calorie-requirements-calculator#:~:text=If%20you%20are%20lightly%20active,Calorie%2DCalculation%20%3D%20BMR%20x%201.725
 */
float User::getActivityMultiplier() const {
    switch (activityLevel) {
        case ActivityLevel::SEDENTARY:
            return 1.2f;
        case ActivityLevel::LIGHT:
            return 1.375f;
        case ActivityLevel::MODERATE:
            return 1.55f;
        case ActivityLevel::ACTIVE:
            return 1.725f;
        case ActivityLevel::VERY_ACTIVE:
            return 1.9f;
        default:
            return 1.55f;
    }
}

/**
 * getGoalCalorieAdjustment Method
 * @return The calorie adjustment based on the user's weight goal
 * This method returns a calorie adjustment factor for weight goals.
 */
float User::getGoalCalorieAdjustment() const {
    switch (goal) {
        case Goal::LOSE_WEIGHT:
            return -500.0f; // Deficit for weight loss
        case Goal::GAIN_WEIGHT:
            return 500.0f;  // Surplus for weight gain
        case Goal::MAINTAIN:
        default:
            return 0.0f;    // No adjustment for maintenance
    }
}

/**
 * calculateDailyCalorieNeeds Method
 * @return The daily calorie needs of the user based on BMR and activity level
 * This method calculates the total daily energy expenditure.
 */
float User::calculateDailyCalorieNeeds() const {
    return calculateBMR() * getActivityMultiplier();
}

/**
 * calculateTargetCalories Method
 * @return The target daily calorie intake based on needs and goals
 * This method calculates the recommended calorie intake according to the user's goal.
 */
float User::calculateTargetCalories() const {
    return calculateDailyCalorieNeeds() + getGoalCalorieAdjustment();
}

/**
 * toJson Method
 * @return A JSON object representing the user
 * This method serializes the User object to JSON.
 */
json User::toJson() const {
    json j;
    j["name"] = name;
    j["age"] = age;
    j["gender"] = static_cast<int>(gender);
    j["height"] = height;
    j["weight"] = weight;
    j["activityLevel"] = static_cast<int>(activityLevel);
    j["goal"] = static_cast<int>(goal);
    j["calorieCalcMethod"] = static_cast<int>(calorieCalcMethod);
    j["lastUpdateTime"] = lastUpdateTime;
    
    // Serialize daily metrics
    json metricsArray = json::array();
    for (const auto& metric : dailyMetrics) {
        metricsArray.push_back(metric.toJson());
    }
    j["dailyMetrics"] = metricsArray;
    
    return j;
}

/**
 * fromJson Method
 * @param j A JSON object representing a user
 * @return A User object deserialized from the JSON
 * This static method deserializes a User object from JSON.
 */
User User::fromJson(const json& j) {
    User user;
    if (j.contains("name")) user.name = j["name"];
    if (j.contains("age")) user.age = j["age"];
    if (j.contains("gender")) user.gender = static_cast<Gender>(j["gender"].get<int>());
    if (j.contains("height")) user.height = j["height"];
    if (j.contains("weight")) user.weight = j["weight"];
    if (j.contains("activityLevel")) user.activityLevel = static_cast<ActivityLevel>(j["activityLevel"].get<int>());
    if (j.contains("goal")) user.goal = static_cast<Goal>(j["goal"].get<int>());
    if (j.contains("calorieCalcMethod")) user.calorieCalcMethod = static_cast<CalorieCalculationMethod>(j["calorieCalcMethod"].get<int>());
    
    // Load lastUpdateTime if it exists
    if (j.contains("lastUpdateTime")) {
        user.lastUpdateTime = j["lastUpdateTime"];
    }
    
    // Load daily metrics if they exist
    if (j.contains("dailyMetrics") && j["dailyMetrics"].is_array()) {
        for (const auto& metricJson : j["dailyMetrics"]) {
            user.dailyMetrics.push_back(DailyMetric::fromJson(metricJson));
        }
    }
    
    return user;
}

/**
 * activityLevelToString Method
 * @param level The activity level enum
 * @return String representation of the activity level
 */
string User::activityLevelToString(ActivityLevel level) {
    switch (level) {
        case ActivityLevel::SEDENTARY: return "Sedentary";
        case ActivityLevel::LIGHT: return "Lightly Active";
        case ActivityLevel::MODERATE: return "Moderately Active";
        case ActivityLevel::ACTIVE: return "Active";
        case ActivityLevel::VERY_ACTIVE: return "Very Active";
        default: return "Unknown";
    }
}

/**
 * goalToString Method
 * @param goal The goal enum
 * @return String representation of the goal
 */
string User::goalToString(Goal goal) {
    switch (goal) {
        case Goal::LOSE_WEIGHT: return "Lose Weight";
        case Goal::MAINTAIN: return "Maintain Weight";
        case Goal::GAIN_WEIGHT: return "Gain Weight";
        default: return "Unknown";
    }
}

/**
 * genderToString Method
 * @param gender The gender enum
 * @return String representation of the gender
 */
string User::genderToString(Gender gender) {
    switch (gender) {
        case Gender::MALE: return "Male";
        case Gender::FEMALE: return "Female";
        case Gender::OTHER: return "Other";
        default: return "Unknown";
    }
}

/**
 * calorieMethodToString Method
 * @param method The calorie calculation method enum
 * @return String representation of the method
 */
string User::calorieMethodToString(CalorieCalculationMethod method) {
    switch (method) {
        case CalorieCalculationMethod::MIFFLIN_ST_JEOR: return "Mifflin-St Jeor";
        case CalorieCalculationMethod::HARRIS_BENEDICT: return "Harris-Benedict";
        case CalorieCalculationMethod::WHO_EQUATION: return "WHO Equation";
        default: return "Unknown";
    }
}

/**
 * stringToActivityLevel Method
 * @param str The string representation
 * @return The corresponding ActivityLevel enum
 */
User::ActivityLevel User::stringToActivityLevel(const string& str) {
    if (str == "Sedentary") return ActivityLevel::SEDENTARY;
    if (str == "Lightly Active") return ActivityLevel::LIGHT;
    if (str == "Moderately Active") return ActivityLevel::MODERATE;
    if (str == "Active") return ActivityLevel::ACTIVE;
    if (str == "Very Active") return ActivityLevel::VERY_ACTIVE;
    return ActivityLevel::MODERATE; // Default
}

/**
 * stringToGoal Method
 * @param str The string representation
 * @return The corresponding Goal enum
 */
User::Goal User::stringToGoal(const string& str) {
    if (str == "Lose Weight") return Goal::LOSE_WEIGHT;
    if (str == "Maintain Weight") return Goal::MAINTAIN;
    if (str == "Gain Weight") return Goal::GAIN_WEIGHT;
    return Goal::MAINTAIN; // Default
}

/**
 * stringToGender Method
 * @param str The string representation
 * @return The corresponding Gender enum
 */
User::Gender User::stringToGender(const string& str) {
    if (str == "Male") return Gender::MALE;
    if (str == "Female") return Gender::FEMALE;
    return Gender::OTHER; // Default
}

/**
 * stringToCalorieMethod Method
 * @param str The string representation
 * @return The corresponding CalorieCalculationMethod enum
 */
User::CalorieCalculationMethod User::stringToCalorieMethod(const string& str) {
    if (str == "Harris-Benedict") return CalorieCalculationMethod::HARRIS_BENEDICT;
    if (str == "WHO Equation") return CalorieCalculationMethod::WHO_EQUATION;
    return CalorieCalculationMethod::MIFFLIN_ST_JEOR; // Default
}

/**
 * needsDailyUpdate Method
 * @return True if the last update was on a different day
 */
bool User::needsDailyUpdate() const {
    // Check if dailyMetrics is empty or if the last record is from a different day
    if (dailyMetrics.empty()) {
        return true;
    }
    
    time_t currentDay = getCurrentDay();
    time_t lastRecordDay = dailyMetrics.back().timestamp / DAY_LENGTH * DAY_LENGTH;
    
    return currentDay > lastRecordDay;
}

/**
 * ensureDailyRecordExists Method
 * Makes sure a record exists for today
 */
void User::ensureDailyRecordExists() {
    if (needsDailyUpdate()) {
        updateDailyRecord();
    }
}

/**
 * getLastUpdateTime Method
 * @return The timestamp of the last update
 */
time_t User::getLastUpdateTime() const {
    return lastUpdateTime;
}

/**
 * updateDailyRecord Method
 * Creates or updates the daily record with current user values
 */
void User::updateDailyRecord() {
    time_t now = time(nullptr);
    DailyMetric metric;
    metric.timestamp = now;
    metric.weight = weight;
    metric.age = age;
    metric.activityLevel = static_cast<int>(activityLevel);
    
    // Check if there's already a record for today
    if (!dailyMetrics.empty()) {
        time_t currentDay = getCurrentDay();
        time_t lastRecordDay = dailyMetrics.back().timestamp / DAY_LENGTH * DAY_LENGTH;
        
        if (currentDay == lastRecordDay) {
            // Update existing record for today
            dailyMetrics.back() = metric;
        } else {
            // Add new record for today
            dailyMetrics.push_back(metric);
        }
    } else {
        // First record
        dailyMetrics.push_back(metric);
    }
    
    lastUpdateTime = now;
}

/**
 * getDailyMetrics Method
 * @return The vector of daily metrics
 */
const vector<DailyMetric>& User::getDailyMetrics() const {
    return dailyMetrics;
}

/**
 * getFormattedDate Method
 * @param timestamp The timestamp to format
 * @return A formatted date string
 */
string User::getFormattedDate(time_t timestamp) const {
    struct tm* timeinfo = localtime(&timestamp);
    stringstream ss;
    ss << put_time(timeinfo, "%Y-%m-%d %H:%M");
    return ss.str();
}

/**
 * getCurrentDay Method
 * @return The timestamp for the start of the current day
 */
time_t User::getCurrentDay() const {
    time_t now = time(nullptr);
    // Round down to the start of the day (assuming DAY_LENGTH is in seconds)
    return (now / DAY_LENGTH) * DAY_LENGTH;
}
