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
