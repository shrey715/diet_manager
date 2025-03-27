#ifndef USER_H
#define USER_H

#include <string>
#include <functional>
#include <map>

enum class Gender {
    MALE,
    FEMALE,
    OTHER
};

enum class ActivityLevel {
    SEDENTARY,
    LIGHT,
    MODERATE,
    ACTIVE,
    VERY_ACTIVE
};

enum class CalorieCalculationMethod {
    HARRIS_BENEDICT,
    MIFFLIN_ST_JEOR
};

class User {
private:
    Gender gender;
    double heightCm;
    int age;
    double weightKg;
    ActivityLevel activityLevel;
    CalorieCalculationMethod calorieMethod;
    
    // Map of calculation methods
    using CalorieCalculationFunc = std::function<double(const User&)>;
    static std::map<CalorieCalculationMethod, CalorieCalculationFunc> calculationMethods;
    
    // Calculation methods
    static double calculateHarrisBenedict(const User& user);
    static double calculateMifflinStJeor(const User& user);
    
public:
    User();
    User(Gender gender, double heightCm, int age, double weightKg, 
         ActivityLevel activityLevel, CalorieCalculationMethod method);
    
    // Getters
    Gender getGender() const;
    double getHeightCm() const;
    int getAge() const;
    double getWeightKg() const;
    ActivityLevel getActivityLevel() const;
    CalorieCalculationMethod getCalorieMethod() const;
    
    // Setters
    void setGender(Gender gender);
    void setHeightCm(double heightCm);
    void setAge(int age);
    void setWeightKg(double weightKg);
    void setActivityLevel(ActivityLevel level);
    void setCalorieMethod(CalorieCalculationMethod method);
    
    // Calculate target calorie intake
    double calculateTargetCalories() const;
    
    // Serialization
    void toJson(std::ostream& os) const;
    static User fromJson(const std::string& json);
    
    // Utility methods
    static std::string genderToString(Gender gender);
    static Gender genderFromString(const std::string& genderStr);
    static std::string activityLevelToString(ActivityLevel level);
    static ActivityLevel activityLevelFromString(const std::string& levelStr);
    static std::string calorieMethodToString(CalorieCalculationMethod method);
    static CalorieCalculationMethod calorieMethodFromString(const std::string& methodStr);
};

#endif // USER_H