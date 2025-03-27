#include "user.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cmath> // for rounding calories

using json = nlohmann::json;

// Static map of calorie calculation methods
// Different equations give different results, so user can choose
std::map<CalorieCalculationMethod, User::CalorieCalculationFunc> User::calculationMethods = {
    {CalorieCalculationMethod::HARRIS_BENEDICT, &User::calculateHarrisBenedict},
    {CalorieCalculationMethod::MIFFLIN_ST_JEOR, &User::calculateMifflinStJeor}
};

// Default constructor with some reasonable values
// Nobody will ever use these values but we need defaults
User::User() 
    : gender(Gender::OTHER), 
      heightCm(170.0),  // average human height, more or less
      age(30),          // not too old, not too young
      weightKg(70.0),   // average weight, I think?
      activityLevel(ActivityLevel::MODERATE), // most people are moderate, right?
      calorieMethod(CalorieCalculationMethod::HARRIS_BENEDICT) {
    // nothing else to initialize
}

// Constructor with all parameters for when we know all the details
User::User(Gender gender, double heightCm, int age, double weightKg, 
           ActivityLevel activityLevel, CalorieCalculationMethod method)
    : gender(gender), 
      heightCm(heightCm), 
      age(age), 
      weightKg(weightKg), 
      activityLevel(activityLevel),
      calorieMethod(method) {
    
    // Sanity check values
    if (heightCm <= 0) {
        std::cerr << "Invalid height, using default 170cm" << std::endl;
        this->heightCm = 170.0;
    }
    
    if (age <= 0 || age > 120) {
        std::cerr << "Invalid age, using default 30" << std::endl;
        this->age = 30;
    }
    
    if (weightKg <= 0) {
        std::cerr << "Invalid weight, using default 70kg" << std::endl;
        this->weightKg = 70.0;
    }
}

// Getters - nothing exciting here
Gender User::getGender() const {
    return gender;
}

double User::getHeightCm() const {
    return heightCm;
}

int User::getAge() const {
    return age;
}

double User::getWeightKg() const {
    return weightKg;
}

ActivityLevel User::getActivityLevel() const {
    return activityLevel;
}

CalorieCalculationMethod User::getCalorieMethod() const {
    return calorieMethod;
}

// Setters with validation
void User::setGender(Gender gender) {
    this->gender = gender;
}

void User::setHeightCm(double heightCm) {
    if (heightCm <= 0) {
        std::cerr << "Warning: Invalid height " << heightCm << ". Using previous value." << std::endl;
        return;
    }
    this->heightCm = heightCm;
}

void User::setAge(int age) {
    if (age <= 0 || age > 120) {
        std::cerr << "Warning: Invalid age " << age << ". Using previous value." << std::endl;
        return;
    }
    this->age = age;
}

void User::setWeightKg(double weightKg) {
    if (weightKg <= 0) {
        std::cerr << "Warning: Invalid weight " << weightKg << ". Using previous value." << std::endl;
        return;
    }
    this->weightKg = weightKg;
}

void User::setActivityLevel(ActivityLevel level) {
    this->activityLevel = level;
}

void User::setCalorieMethod(CalorieCalculationMethod method) {
    this->calorieMethod = method;
}

// Calculate target calories based on the selected method
double User::calculateTargetCalories() const {
    if (calculationMethods.find(calorieMethod) != calculationMethods.end()) {
        // Round to nearest whole calorie because decimal calories are stupid
        double result = calculationMethods[calorieMethod](*this);
        return std::round(result);
    }
    
    // Default to Harris-Benedict if method not found (shouldn't happen)
    std::cerr << "Warning: Unknown calorie calculation method. Using Harris-Benedict." << std::endl;
    return std::round(calculateHarrisBenedict(*this));
}

// Implementation of Harris-Benedict equation
// Looked this up on Wikipedia at 2am, hope it's right
double User::calculateHarrisBenedict(const User& user) {
    double bmr = 0.0;
    
    if (user.gender == Gender::MALE) {
        bmr = 88.362 + (13.397 * user.weightKg) + (4.799 * user.heightCm) - (5.677 * user.age);
    } else {
        // female and other both use female equation
        bmr = 447.593 + (9.247 * user.weightKg) + (3.098 * user.heightCm) - (4.330 * user.age);
    }
    
    // Apply activity multiplier
    double activityMultiplier = 1.2; // Default sedentary
    switch (user.activityLevel) {
        case ActivityLevel::SEDENTARY: activityMultiplier = 1.2; break;
        case ActivityLevel::LIGHT: activityMultiplier = 1.375; break;
        case ActivityLevel::MODERATE: activityMultiplier = 1.55; break;
        case ActivityLevel::ACTIVE: activityMultiplier = 1.725; break;
        case ActivityLevel::VERY_ACTIVE: activityMultiplier = 1.9; break;
        default: 
            std::cerr << "Unknown activity level, using sedentary" << std::endl;
            activityMultiplier = 1.2;
    }
    
    return bmr * activityMultiplier;
}

// Implementation of Mifflin-St Jeor equation
// Another equation that does basically the same thing
double User::calculateMifflinStJeor(const User& user) {
    double bmr = 0.0;
    
    if (user.gender == Gender::MALE) {
        bmr = (10 * user.weightKg) + (6.25 * user.heightCm) - (5 * user.age) + 5;
    } else {
        // female and other both use female equation
        bmr = (10 * user.weightKg) + (6.25 * user.heightCm) - (5 * user.age) - 161;
    }
    
    // Apply activity multiplier
    double activityMultiplier = 1.2; // Default sedentary
    switch (user.activityLevel) {
        case ActivityLevel::SEDENTARY: activityMultiplier = 1.2; break;
        case ActivityLevel::LIGHT: activityMultiplier = 1.375; break;
        case ActivityLevel::MODERATE: activityMultiplier = 1.55; break;
        case ActivityLevel::ACTIVE: activityMultiplier = 1.725; break;
        case ActivityLevel::VERY_ACTIVE: activityMultiplier = 1.9; break;
        default:
            std::cerr << "Unknown activity level, using sedentary" << std::endl;
            activityMultiplier = 1.2;
    }
    
    return bmr * activityMultiplier;
}

// Serialize to JSON
void User::toJson(std::ostream& os) const {
    json j;
    j["gender"] = genderToString(gender);
    j["height_cm"] = heightCm;
    j["age"] = age;
    j["weight_kg"] = weightKg;
    j["activity_level"] = activityLevelToString(activityLevel);
    j["calorie_method"] = calorieMethodToString(calorieMethod);
    
    os << j.dump(4);
}

// Create User from JSON string
User User::fromJson(const std::string& jsonString) {
    try {
        json j = json::parse(jsonString);
        
        User user;
        
        // Check for required fields
        if (j.contains("gender")) {
            user.setGender(genderFromString(j["gender"]));
        }
        
        if (j.contains("height_cm")) {
            user.setHeightCm(j["height_cm"]);
        }
        
        if (j.contains("age")) {
            user.setAge(j["age"]);
        }
        
        if (j.contains("weight_kg")) {
            user.setWeightKg(j["weight_kg"]);
        }
        
        if (j.contains("activity_level")) {
            user.setActivityLevel(activityLevelFromString(j["activity_level"]));
        }
        
        if (j.contains("calorie_method")) {
            user.setCalorieMethod(calorieMethodFromString(j["calorie_method"]));
        }
        
        return user;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing user from JSON: " << e.what() << std::endl;
        std::cerr << "Creating default user instead" << std::endl;
        return User(); // Return default user on error
    }
}

// Convert enum to string for serialization
std::string User::genderToString(Gender gender) {
    switch (gender) {
        case Gender::MALE: return "male";
        case Gender::FEMALE: return "female";
        case Gender::OTHER: return "other";
        default: return "unknown";
    }
}

// Parse string to gender enum
Gender User::genderFromString(const std::string& genderStr) {
    std::string lowercase = genderStr;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), 
                   [](unsigned char c){ return std::tolower(c); });
    
    if (lowercase == "male" || lowercase == "m") return Gender::MALE;
    if (lowercase == "female" || lowercase == "f") return Gender::FEMALE;
    return Gender::OTHER;
}

// Convert activity level to string
std::string User::activityLevelToString(ActivityLevel level) {
    switch (level) {
        case ActivityLevel::SEDENTARY: return "sedentary";
        case ActivityLevel::LIGHT: return "light";
        case ActivityLevel::MODERATE: return "moderate";
        case ActivityLevel::ACTIVE: return "active";
        case ActivityLevel::VERY_ACTIVE: return "very_active";
        default: return "unknown";
    }
}

// Parse string to activity level enum
ActivityLevel User::activityLevelFromString(const std::string& levelStr) {
    std::string lowercase = levelStr;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), 
                   [](unsigned char c){ return std::tolower(c); });
    
    if (lowercase == "sedentary" || lowercase == "none") return ActivityLevel::SEDENTARY;
    if (lowercase == "light" || lowercase == "low") return ActivityLevel::LIGHT;
    if (lowercase == "moderate" || lowercase == "medium" || lowercase == "mod") return ActivityLevel::MODERATE;
    if (lowercase == "active" || lowercase == "high") return ActivityLevel::ACTIVE;
    if (lowercase == "very_active" || lowercase == "very active" || lowercase == "intense") return ActivityLevel::VERY_ACTIVE;
    
    std::cerr << "Unknown activity level '" << levelStr << "', using moderate" << std::endl;
    return ActivityLevel::MODERATE; // Default if not recognized
}

// Convert calorie method to string
std::string User::calorieMethodToString(CalorieCalculationMethod method) {
    switch (method) {
        case CalorieCalculationMethod::HARRIS_BENEDICT: return "harris_benedict";
        case CalorieCalculationMethod::MIFFLIN_ST_JEOR: return "mifflin_st_jeor";
        default: return "unknown";
    }
}

// Parse string to calorie method enum
CalorieCalculationMethod User::calorieMethodFromString(const std::string& methodStr) {
    std::string lowercase = methodStr;
    std::transform(lowercase.begin(), lowercase.end(), lowercase.begin(), 
                   [](unsigned char c){ return std::tolower(c); });
    
    if (lowercase == "harris_benedict" || lowercase == "harris" || lowercase == "benedict") 
        return CalorieCalculationMethod::HARRIS_BENEDICT;
    if (lowercase == "mifflin_st_jeor" || lowercase == "mifflin" || lowercase == "st_jeor") 
        return CalorieCalculationMethod::MIFFLIN_ST_JEOR;
    
    std::cerr << "Unknown calorie method '" << methodStr << "', using Harris-Benedict" << std::endl;
    return CalorieCalculationMethod::HARRIS_BENEDICT; // Default if not recognized
}
