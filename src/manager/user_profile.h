/**
 * @file user_profile.h
 * @brief User Profile Management System
 * 
 * This file defines the UserProfile class which manages user-specific information
 * in the diet manager application. It maintains personal data like age, weight, height,
 * gender, and activity level, which are used for calorie calculations.
 * 
 * Key features:
 * - Singleton pattern implementation for global access
 * - Access to user attributes and settings
 * - Calculation of target calorie intake based on user characteristics
 * - Persistence of user data to/from JSON files
 * - Methods to update user attributes and settings
 * 
 * The UserProfile class serves as the central repository for all user-specific
 * information used throughout the application.
 */

#ifndef USER_PROFILE_H
#define USER_PROFILE_H

#include <string>
#include <memory>
#include <limits>
#include "../models/user.h"

using namespace std;

/**
 * UserProfile Class
 * This class manages the user profile and calorie calculations.
 */
class UserProfile {
public:
    static UserProfile& getInstance();
    
    // User profile methods
    User& getUser();
    void saveUser(const string& filepath = "");
    void loadUser(const string& filepath = "");
    void setUserAttribute(const string& attribute, const string& value);
    
    // Calorie calculation
    float calculateTargetCalories() const;
    
private:
    UserProfile();
    ~UserProfile();
    UserProfile(const UserProfile&) = delete;
    UserProfile& operator=(const UserProfile&) = delete;
    
    // Methods for profile initialization
    void initializeUserProfile();
    void ensureInitialized();
    
    User user;
    string defaultFilepath;
    bool isInitialized;
};

#endif // USER_PROFILE_H
