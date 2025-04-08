#ifndef CLI_H
#define CLI_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include "models/user.h"
#include "models/food.h"
#include "models/log_entry.h"
#include "manager/food_database.h"
#include "manager/user_profile.h"
#include "utils/terminal_colors.h"

using namespace std;

/**
 * CLI Class
 * This class handles the command-line interface for the diet manager application.
 */
class CLI {
public:
    CLI();
    void run();

private:
    // Command handlers
    using CommandFunc = function<void(const vector<string>&)>;
    map<string, CommandFunc> commands;
    map<string, string> helpText;
    
    // Current date
    string currentDate;
    
    // Data managers
    FoodDatabase& foodDb;
    UserProfile& userProfile;
    LogHistory logHistory;
    
    // Initialize commands
    void registerCommands();
    
    // Helper methods
    vector<string> parseCommandLine(const string& line);
    void displayHelp(const vector<string>& args);
    bool confirmAction(const string& message);
    
    // Date helper
    string getCurrentDate() const;
    
    // Food database commands
    void addBasicFood(const vector<string>& args);
    void listFoods(const vector<string>& args);
    void searchFoods(const vector<string>& args);
    void createCompositeFood(const vector<string>& args);
    
    // Log commands
    void addFoodToLog(const vector<string>& args);
    void removeFoodFromLog(const vector<string>& args);
    void viewLog(const vector<string>& args);
    void setDate(const vector<string>& args);
    void undoCommand(const vector<string>& args);
    void redoCommand(const vector<string>& args);
    
    // User profile commands
    void viewProfile(const vector<string>& args);
    void updateProfile(const vector<string>& args);
    void viewCalories(const vector<string>& args);
    void viewDailyHistory(const vector<string>& args);  // New command
    
    // Data management commands
    void saveData(const vector<string>& args);
    void loadData(const vector<string>& args);
    void manualSave(const vector<string>& args);  // Add this
    void manualLoad(const vector<string>& args);  // Add this
    
    // UI commands
    void clearScreen(const vector<string>& args);
    
    // Exit command
    void quitProgram(const vector<string>& args);
};

#endif // CLI_H
