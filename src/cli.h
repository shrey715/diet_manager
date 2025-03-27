#ifndef CLI_H
#define CLI_H

#include "managers/food_database.h"
#include "managers/log_manager.h"
#include "models/user.h"
#include <memory>
#include <chrono>
#include <string>
#include <vector>
#include <functional>
#include <map>

class CLI {
private:
    std::shared_ptr<FoodDatabase> foodDatabase;
    std::shared_ptr<LogManager> logManager;
    std::shared_ptr<User> user;
    std::string userFilePath;
    bool running;
    
    // Command handling
    struct Command {
        std::string name;
        std::string description;
        std::function<void(const std::vector<std::string>&)> handler;
    };
    
    std::map<std::string, Command> commands;
    
    // Helper methods
    void registerCommands();
    std::vector<std::string> parseInput(const std::string& input) const;
    void displayHelp() const;
    void displayLogo() const;
    
    // User profile management
    void loadUserProfile();
    void saveUserProfile() const;
    void createUserProfile();
    void displayUserProfile() const;
    
    // Command handlers
    void handleQuit(const std::vector<std::string>& args);
    void handleHelp(const std::vector<std::string>& args);
    void handleAddBasicFood(const std::vector<std::string>& args);
    void handleListFoods(const std::vector<std::string>& args);
    void handleSearchFoods(const std::vector<std::string>& args);
    void handleCreateCompositeFood(const std::vector<std::string>& args);
    void handleAddFoodToLog(const std::vector<std::string>& args);
    void handleRemoveFoodFromLog(const std::vector<std::string>& args);
    void handleViewLogEntry(const std::vector<std::string>& args);
    void handleSetDate(const std::vector<std::string>& args);
    void handleUndo(const std::vector<std::string>& args);
    void handleRedo(const std::vector<std::string>& args);
    void handleUpdateProfile(const std::vector<std::string>& args);
    void handleShowCalories(const std::vector<std::string>& args);
    void handleSaveData(const std::vector<std::string>& args);
    void handleLoadData(const std::vector<std::string>& args);
    
public:
    CLI(const std::string& basicFoodFilePath, 
        const std::string& compositeFoodFilePath,
        const std::string& logFilePath,
        const std::string& userFilePath);
    
    void run();
};

#endif // CLI_H
