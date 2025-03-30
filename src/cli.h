#ifndef CLI_H
#define CLI_H

#include "managers/food_database.h"
#include "managers/log_manager.h"
#include "models/user.h"
#include "utils/color.h"
#include <memory>
#include <chrono>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <deque>

/**
 * @enum ViewMode 
 * @brief Different views in the CLI interface
 */
enum class ViewMode {
    MAIN_MENU,
    FOOD_DATABASE,
    FOOD_DETAIL,
    LOG_ENTRY,
    USER_PROFILE,
    HELP,
    COMMAND_HISTORY
};

/**
 * @class CLI
 * @brief Main command-line interface for diet manager
 */
class CLI {
private:
    std::shared_ptr<FoodDatabase> foodDatabase;
    std::shared_ptr<LogManager> logManager;
    std::shared_ptr<User> user;
    std::string userFilePath;
    bool running;
    ViewMode currentView;
    std::deque<std::string> commandHistory;
    const size_t maxHistorySize = 100; 
    std::string foodDetailId;
    int termWidth = 80;
    int termHeight = 24;
    
    // Command handling
    struct Command {
        std::string name;
        std::string description;
        std::string category;
        std::string usage;
        std::function<void(const std::vector<std::string>&)> handler;
    };
    
    std::map<std::string, Command> commands;
    
    // Helper methods
    void registerCommands();
    std::vector<std::string> parseInput(const std::string& input) const;
    void clearScreen() const;
    void getTerminalSize() const;
    std::string centerText(const std::string& text, int width) const;
    std::string formatCalories(double calories) const;
    
    // UI display methods
    void displayLogo() const;
    void displayHeader(const std::string& title) const;
    void displayFooter() const;
    void displayStatusBar() const;
    void displayMenuPrompt() const;
    void displayHelp() const;
    void displayCommandHistory() const;
    void displayMainMenu() const;
    void displayFoodDatabaseView() const;
    void displayFoodDetailView(const std::string& foodId) const;
    void displayLogEntryView() const;
    void displayUserProfileView() const;
    
    // View management
    void setView(ViewMode view);
    void addToHistory(const std::string& command);
    
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
    void handleViewFood(const std::vector<std::string>& args);
    void handleHistory(const std::vector<std::string>& args);
    void handleView(const std::vector<std::string>& args);
    void handleRefresh(const std::vector<std::string>& args);
    
public:
    /**
     * @brief Constructor with file paths
     */
    CLI(const std::string& basicFoodFilePath, 
        const std::string& compositeFoodFilePath,
        const std::string& logFilePath,
        const std::string& userFilePath);
    
    /**
     * @brief Run the CLI application
     */
    void run();
};

#endif // CLI_H
