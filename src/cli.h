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
#include <deque>

// Terminal colors using ANSI escape codes - modified for better readability
namespace Color {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string UNDERLINE = "\033[4m";
    const std::string BLINK = "\033[5m";
    const std::string REVERSE = "\033[7m";
    const std::string HIDDEN = "\033[8m";
    
    // Foreground colors - adjusted for better contrast
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[91m";          // Brighter red
    const std::string GREEN = "\033[92m";        // Brighter green
    const std::string YELLOW = "\033[93m";       // Brighter yellow
    const std::string BLUE = "\033[94m";         // Brighter blue
    const std::string MAGENTA = "\033[95m";      // Brighter magenta
    const std::string CYAN = "\033[96m";         // Brighter cyan
    const std::string WHITE = "\033[97m";        // Brighter white
    
    // Background colors - using darker variants for text readability
    const std::string BG_BLACK = "\033[40m";
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN = "\033[46m";
    const std::string BG_WHITE = "\033[47m";
    
    // Additional color combinations for better UI
    const std::string HEADER = "\033[97;44m";    // White text on blue background
    const std::string SUCCESS = "\033[92m";      // Bright green
    const std::string WARNING = "\033[93m";      // Bright yellow
    const std::string ERROR = "\033[91m";        // Bright red
    const std::string INFO = "\033[96m";         // Bright cyan
    const std::string HIGHLIGHT = "\033[97;45m"; // White text on magenta background
}

// View mode enumeration
enum class ViewMode {
    MAIN_MENU,
    FOOD_DATABASE,
    FOOD_DETAIL,
    LOG_ENTRY,
    USER_PROFILE,
    HELP,
    COMMAND_HISTORY
};

class CLI {
private:
    std::shared_ptr<FoodDatabase> foodDatabase;
    std::shared_ptr<LogManager> logManager;
    std::shared_ptr<User> user;
    std::string userFilePath;
    bool running;
    ViewMode currentView;
    std::deque<std::string> commandHistory;
    const size_t maxHistorySize = 100;  // Maximum number of commands to store
    std::string foodDetailId;  // For FOOD_DETAIL view
    int termWidth = 80;  // Default terminal width
    int termHeight = 24;  // Default terminal height
    
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
    void displayHelp() const;
    void displayLogo() const;
    void clearScreen() const;
    void getTerminalSize() const; // Changed to const
    void displayStatusBar() const;
    void displayHeader(const std::string& title) const;
    void displayFooter() const;
    void displayCommandHistory() const;
    void displayMainMenu() const;
    void displayFoodDatabaseView() const;
    void displayFoodDetailView(const std::string& foodId) const;
    void displayLogEntryView() const;
    void displayUserProfileView() const;
    void displayMenuPrompt() const;
    void setView(ViewMode view);
    void addToHistory(const std::string& command);
    std::string centerText(const std::string& text, int width) const;
    std::string formatCalories(double calories) const;
    
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
    CLI(const std::string& basicFoodFilePath, 
        const std::string& compositeFoodFilePath,
        const std::string& logFilePath,
        const std::string& userFilePath);
    
    void run();
};

#endif // CLI_H
