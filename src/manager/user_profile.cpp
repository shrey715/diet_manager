#include "user_profile.h"
#include <fstream>
#include <iostream>
#include "../utils/terminal_colors.h"

/**
 * UserProfile getInstance Method
 * @return The singleton instance of UserProfile
 * This method implements the singleton pattern.
 */
UserProfile& UserProfile::getInstance() {
    static UserProfile instance;
    return instance;
}

/**
 * UserProfile Constructor
 * Initializes the UserProfile with default values.
 */
UserProfile::UserProfile() : defaultFilepath("data/user.json") {
    // Create empty user - will be initialized on first load if needed
    user = User("", 0, User::Gender::OTHER, 0.0f, 0.0f, 
                User::ActivityLevel::MODERATE, User::Goal::MAINTAIN,
                User::CalorieCalculationMethod::MIFFLIN_ST_JEOR);
    isInitialized = false;
}

/**
 * UserProfile Destructor
 */
UserProfile::~UserProfile() {
    try {
        saveUser();
    } catch (const std::exception& e) {
        std::cerr << "Error saving user profile: " << e.what() << std::endl;
    }
}

/**
 * initializeUserProfile Method
 * Prompts the user to enter their profile information
 * This is called when no user profile exists on disk
 */
void UserProfile::initializeUserProfile() {
    std::cout << TerminalColors::bold("\nWelcome to Diet Manager!\n");
    std::cout << "Please set up your user profile:\n\n"
              << TerminalColors::info("Note: You can change these settings later with the profile command.\n\n");
    
    std::string name;
    int age;
    std::string genderStr;
    float height, weight;
    int activityLevel;
    int goal;
    int method;
    
    std::cout << "Name: ";
    std::getline(std::cin, name);
    
    std::cout << "Age (years): ";
    std::cin >> age;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "Gender [Male/Female/Other]: ";
    std::getline(std::cin, genderStr);
    
    std::cout << "Height (cm): ";
    std::cin >> height;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::cout << "Weight (kg): ";
    std::cin >> weight;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    // Display activity level options
    std::cout << "\nActivity Levels:\n"
              << "1 - Sedentary (little or no exercise)\n"
              << "2 - Lightly Active (light exercise 1-3 days/week)\n"
              << "3 - Moderately Active (moderate exercise 3-5 days/week)\n"
              << "4 - Active (hard exercise 6-7 days/week)\n"
              << "5 - Very Active (very hard exercise, physical job or training twice a day)\n"
              << "Choose Activity Level (1-5): ";
    std::cin >> activityLevel;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    // Display goal options
    std::cout << "\nGoals:\n"
              << "1 - Lose Weight\n"
              << "2 - Maintain Weight\n"
              << "3 - Gain Weight\n"
              << "Choose Goal (1-3): ";
    std::cin >> goal;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    // Display calorie calculation method options
    std::cout << "\nCalorie Calculation Methods:\n"
              << "1 - Harris-Benedict\n"
              << "2 - Mifflin-St Jeor\n"
              << "3 - WHO Equation\n"
              << "Choose Calorie Method (1-3): ";
    std::cin >> method;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    // Set the values
    user.setName(name);
    user.setAge(age);
    user.setGender(User::stringToGender(genderStr));
    user.setHeight(height);
    user.setWeight(weight);
    
    // Adjust activity level (1-based to 0-based)
    if (activityLevel >= 1 && activityLevel <= 5) {
        user.setActivityLevel(static_cast<User::ActivityLevel>(activityLevel - 1));
    } else {
        std::cout << TerminalColors::warning("Invalid activity level. Setting to Moderately Active.") << std::endl;
        user.setActivityLevel(User::ActivityLevel::MODERATE);
    }
    
    // Adjust goal (1-based to 0-based)
    if (goal >= 1 && goal <= 3) {
        user.setGoal(static_cast<User::Goal>(goal - 1));
    } else {
        std::cout << TerminalColors::warning("Invalid goal. Setting to Maintain Weight.") << std::endl;
        user.setGoal(User::Goal::MAINTAIN);
    }
    
    // Adjust calorie method (1-based to 0-based)
    if (method >= 1 && method <= 3) {
        user.setCalorieCalculationMethod(static_cast<User::CalorieCalculationMethod>(method - 1));
    } else {
        std::cout << TerminalColors::warning("Invalid calorie method. Setting to Mifflin-St Jeor.") << std::endl;
        user.setCalorieCalculationMethod(User::CalorieCalculationMethod::MIFFLIN_ST_JEOR);
    }
    
    // Save the profile
    saveUser();
    
    std::cout << TerminalColors::success("\nProfile created successfully!\n");
    isInitialized = true;
}

/**
 * ensureInitialized Method
 * Makes sure the user profile is initialized
 */
void UserProfile::ensureInitialized() {
    if (!isInitialized) {
        // Try to load from file first
        try {
            loadUser();
        } catch (const std::exception&) {
            // If loading failed, initialize with user input
            initializeUserProfile();
        }
        
        // If the profile is still not valid, initialize with user input
        if (user.getName().empty() || user.getAge() == 0 || user.getWeight() == 0.0f || user.getHeight() == 0.0f) {
            initializeUserProfile();
        } else {
            isInitialized = true;
        }
    }
}

/**
 * getUser Method
 * @return Reference to the current user
 */
User& UserProfile::getUser() {
    ensureInitialized();
    return user;
}

/**
 * saveUser Method
 * @param filepath Optional filepath to save to (uses default if empty)
 * Saves the user profile to a JSON file.
 */
void UserProfile::saveUser(const std::string& filepath) {
    std::string path = filepath.empty() ? defaultFilepath : filepath;
    
    try {
        std::ofstream file(path);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file for writing: " + path);
        }
        
        json userJson = user.toJson();
        file << std::setw(4) << userJson << std::endl;
        file.close();
    } catch (const std::exception& e) {
        throw std::runtime_error("Error saving user profile: " + std::string(e.what()));
    }
}

/**
 * loadUser Method
 * @param filepath Optional filepath to load from (uses default if empty)
 * Loads the user profile from a JSON file.
 */
void UserProfile::loadUser(const std::string& filepath) {
    std::string path = filepath.empty() ? defaultFilepath : filepath;
    
    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            // If file doesn't exist, we'll initialize when needed
            return;
        }
        
        json userJson;
        file >> userJson;
        file.close();
        
        user = User::fromJson(userJson);
        isInitialized = !user.getName().empty();
    } catch (const std::exception& e) {
        throw std::runtime_error("Error loading user profile: " + std::string(e.what()));
    }
}

/**
 * setUserAttribute Method
 * @param attribute The attribute to set
 * @param value The new value
 * Updates a user attribute from string values.
 */
void UserProfile::setUserAttribute(const std::string& attribute, const std::string& value) {
    try {
        if (attribute == "name") {
            user.setName(value);
        } else if (attribute == "age") {
            user.setAge(std::stoi(value));
        } else if (attribute == "gender") {
            user.setGender(User::stringToGender(value));
        } else if (attribute == "height") {
            user.setHeight(std::stof(value));
        } else if (attribute == "weight") {
            user.setWeight(std::stof(value));
        } else if (attribute == "activity") {
            // For backward compatibility with string-based command
            if (value == "1" || value == "2" || value == "3" || value == "4" || value == "5") {
                int level = std::stoi(value) - 1;
                user.setActivityLevel(static_cast<User::ActivityLevel>(level));
            } else {
                user.setActivityLevel(User::stringToActivityLevel(value));
            }
        } else if (attribute == "goal") {
            // For backward compatibility with string-based command
            if (value == "1" || value == "2" || value == "3") {
                int goalVal = std::stoi(value) - 1;
                user.setGoal(static_cast<User::Goal>(goalVal));
            } else {
                user.setGoal(User::stringToGoal(value));
            }
        } else if (attribute == "calorie_method") {
            // For backward compatibility with string-based command
            if (value == "1" || value == "2" || value == "3") {
                int methodVal = std::stoi(value) - 1;
                user.setCalorieCalculationMethod(static_cast<User::CalorieCalculationMethod>(methodVal));
            } else {
                user.setCalorieCalculationMethod(User::stringToCalorieMethod(value));
            }
        } else {
            throw std::invalid_argument("Unknown attribute: " + attribute);
        }
    } catch (const std::exception& e) {
        throw std::invalid_argument("Failed to set attribute: " + std::string(e.what()));
    }
}

/**
 * calculateTargetCalories Method
 * @return The target calories for the user
 * Calculates the target calories based on the user profile.
 */
float UserProfile::calculateTargetCalories() const {
    return user.calculateTargetCalories();
}
