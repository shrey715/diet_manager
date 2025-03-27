#include "cli.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

CLI::CLI(const std::string& basicFoodFilePath, 
         const std::string& compositeFoodFilePath,
         const std::string& logFilePath,
         const std::string& userFilePath)
    : userFilePath(userFilePath), running(false), currentView(ViewMode::MAIN_MENU) {
    
    // Initialize food database
    foodDatabase = std::make_shared<FoodDatabase>(basicFoodFilePath, compositeFoodFilePath);
    
    // Load user profile or create a new one
    loadUserProfile();
    
    // Initialize log manager
    logManager = std::make_shared<LogManager>(logFilePath, user, foodDatabase);
    
    // Register available commands
    registerCommands();
    
    // Get terminal size
    getTerminalSize();
}

void CLI::run() {
    running = true;
    
    // Clear the screen and show welcome message
    clearScreen();
    displayLogo();
    std::cout << Color::GREEN << "Welcome to Diet Manager!" << Color::RESET << std::endl;
    std::cout << "Type '" << Color::CYAN << "help" << Color::RESET << "' to see available commands." << std::endl;
    
    // Load data
    foodDatabase->loadDatabase();
    logManager->loadLogs();
    
    // Show initial view
    setView(ViewMode::MAIN_MENU);
    
    // Main CLI loop
    std::string input;
    while (running) {
        displayMenuPrompt();
        std::getline(std::cin, input);
        
        if (input.empty()) {
            setView(currentView); // Refresh current view
            continue;
        }
        
        // Add command to history
        addToHistory(input);
        
        // Parse input into command and arguments
        std::vector<std::string> tokens = parseInput(input);
        if (tokens.empty()) {
            setView(currentView); // Refresh current view
            continue;
        }
        
        std::string commandName = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        // Find and execute command
        auto it = commands.find(commandName);
        if (it != commands.end()) {
            it->second.handler(args);
        } else {
            clearScreen();
            std::cout << Color::RED << "Unknown command: " << commandName << Color::RESET << std::endl;
            std::cout << "Type '" << Color::CYAN << "help" << Color::RESET << "' to see available commands." << std::endl;
            
            // Return to previous view after a short delay
            sleep(1);
            setView(currentView);
        }
    }
    
    // Save data before exiting
    saveUserProfile();
    foodDatabase->saveDatabase();
    logManager->saveLogs();
    
    clearScreen();
    std::cout << Color::GREEN << "Thank you for using Diet Manager. Goodbye!" << Color::RESET << std::endl;
}

void CLI::registerCommands() {
    // General commands
    commands["help"] = {"help", "Display available commands", "General", "help [category]",
                        [this](const auto& args) { handleHelp(args); }};
    commands["quit"] = {"quit", "Exit the program", "General", "quit",
                        [this](const auto& args) { handleQuit(args); }};
    commands["exit"] = {"exit", "Exit the program", "General", "exit",
                        [this](const auto& args) { handleQuit(args); }};
    commands["view"] = {"view", "Switch to a different view", "General", "view <main|food|log|profile|help|history>",
                        [this](const auto& args) { handleView(args); }};
    commands["refresh"] = {"refresh", "Refresh the current view", "General", "refresh",
                          [this](const auto& args) { handleRefresh(args); }};
    commands["history"] = {"history", "View command history", "General", "history [count]",
                           [this](const auto& args) { handleHistory(args); }};
    
    // Food database commands
    commands["add-basic-food"] = {"add-basic-food", "Add a new basic food item", "Food Database", 
                                 "add-basic-food <id> <calories> <keyword1> [keyword2] ...",
                                 [this](const auto& args) { handleAddBasicFood(args); }};
    commands["list-foods"] = {"list-foods", "List all available foods", "Food Database", 
                             "list-foods", 
                             [this](const auto& args) { handleListFoods(args); }};
    commands["search-foods"] = {"search-foods", "Search for foods by keywords", "Food Database", 
                              "search-foods <keyword1> [keyword2] ... [--all]",
                              [this](const auto& args) { handleSearchFoods(args); }};
    commands["create-composite"] = {"create-composite", "Create a new composite food", "Food Database",
                                  "create-composite <id> <keyword1> [keyword2] ... --components <food1> <servings1> [<food2> <servings2> ...]",
                                  [this](const auto& args) { handleCreateCompositeFood(args); }};
    commands["view-food"] = {"view-food", "View details of a specific food", "Food Database",
                            "view-food <food_id>",
                            [this](const auto& args) { handleViewFood(args); }};
    
    // Log management commands
    commands["add-food"] = {"add-food", "Add food to daily log", "Log Management",
                          "add-food <food_id> <servings>",
                          [this](const auto& args) { handleAddFoodToLog(args); }};
    commands["remove-food"] = {"remove-food", "Remove food from daily log", "Log Management",
                             "remove-food <food_id>", 
                             [this](const auto& args) { handleRemoveFoodFromLog(args); }};
    commands["view-log"] = {"view-log", "View the log for a specific date", "Log Management",
                           "view-log [date]",
                           [this](const auto& args) { handleViewLogEntry(args); }};
    commands["set-date"] = {"set-date", "Set the current working date", "Log Management",
                           "set-date <YYYY-MM-DD>",
                           [this](const auto& args) { handleSetDate(args); }};
    commands["undo"] = {"undo", "Undo the last log operation", "Log Management",
                       "undo",
                       [this](const auto& args) { handleUndo(args); }};
    commands["redo"] = {"redo", "Redo the last undone operation", "Log Management",
                       "redo",
                       [this](const auto& args) { handleRedo(args); }};
    
    // User profile commands
    commands["profile"] = {"profile", "Display or update user profile", "User Profile",
                          "profile [gender|height|age|weight|activity|method] [value]",
                          [this](const auto& args) { handleUpdateProfile(args); }};
    commands["calories"] = {"calories", "Show calorie intake and target", "User Profile",
                           "calories [date]",
                           [this](const auto& args) { handleShowCalories(args); }};
    
    // Data management commands
    commands["save"] = {"save", "Save all data", "Data Management",
                       "save",
                       [this](const auto& args) { handleSaveData(args); }};
    commands["load"] = {"load", "Load all data", "Data Management",
                       "load",
                       [this](const auto& args) { handleLoadData(args); }};
}

// New methods for advanced UI

void CLI::clearScreen() const {
    // ANSI escape code to clear screen and move cursor to home position
    std::cout << "\033[2J\033[H";
}

void CLI::getTerminalSize() const {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    // Update terminal size if we could detect it
    if (w.ws_col > 0) const_cast<CLI*>(this)->termWidth = w.ws_col;
    if (w.ws_row > 0) const_cast<CLI*>(this)->termHeight = w.ws_row;
}

std::string CLI::centerText(const std::string& text, int width) const {
    if (width <= text.length()) return text;
    
    int padding = (width - text.length()) / 2;
    return std::string(padding, ' ') + text + std::string(padding, ' ');
}

void CLI::displayHeader(const std::string& title) const {
    getTerminalSize();
    std::string centeredTitle = centerText(title, termWidth - 4);
    
    std::cout << Color::HEADER << "┌" << std::string(termWidth - 2, '-') << "┐" << Color::RESET << std::endl;
    std::cout << Color::HEADER << "│ " << std::left << std::setw(termWidth - 4) << centeredTitle << " │" << Color::RESET << std::endl;
    std::cout << Color::HEADER << "└" << std::string(termWidth - 2, '-') << "┘" << Color::RESET << std::endl;
}

void CLI::displayFooter() const {
    getTerminalSize();
    std::string dateStr = LogEntry::dateToString(logManager->getCurrentDate());
    std::string footerText = "Current date: " + dateStr + " | Press 'help' for commands | 'view' to change views";
    
    std::cout << Color::HEADER << "┌" << std::string(termWidth - 2, '-') << "┐" << Color::RESET << std::endl;
    std::cout << Color::HEADER << "│ " << std::left << std::setw(termWidth - 4) << footerText << " │" << Color::RESET << std::endl;
    std::cout << Color::HEADER << "└" << std::string(termWidth - 2, '-') << "┘" << Color::RESET << std::endl;
}

void CLI::displayStatusBar() const {
    getTerminalSize();
    
    // Calculate daily calories
    double consumed = logManager->getConsumedCalories(logManager->getCurrentDate());
    double target = logManager->getTargetCalories();
    
    // Status info
    std::string dateInfo = "Date: " + LogEntry::dateToString(logManager->getCurrentDate());
    std::string calorieInfo = "Calories: " + std::to_string(static_cast<int>(consumed)) + "/" + 
                             std::to_string(static_cast<int>(target));
    
    // Percentage of calorie target
    double percentage = target > 0 ? (consumed / target) * 100.0 : 0.0;
    std::string progressBar = "[";
    int barWidth = 20;
    int filledWidth = static_cast<int>((percentage > 100 ? 100 : percentage) * barWidth / 100);
    
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            progressBar += "█";
        } else {
            progressBar += "░";
        }
    }
    progressBar += "] " + std::to_string(static_cast<int>(percentage)) + "%";
    
    // Status bar color depends on percentage
    std::string statusColor;
    if (percentage > 100) {
        statusColor = Color::ERROR;  // Over calorie limit
    } else if (percentage > 85) {
        statusColor = Color::WARNING;  // Close to limit
    } else {
        statusColor = Color::SUCCESS;  // Good
    }
    
    // Display status bar
    std::cout << Color::BG_BLACK << Color::WHITE << " " << dateInfo << " | " 
              << calorieInfo << " " << statusColor << progressBar << Color::RESET << std::endl;
    std::cout << std::string(termWidth, '-') << std::endl;
}

void CLI::displayMenuPrompt() const {
    std::string viewName;
    switch (currentView) {
        case ViewMode::MAIN_MENU: viewName = "Main Menu"; break;
        case ViewMode::FOOD_DATABASE: viewName = "Food Database"; break;
        case ViewMode::FOOD_DETAIL: viewName = "Food Detail"; break;
        case ViewMode::LOG_ENTRY: viewName = "Log Entry"; break;
        case ViewMode::USER_PROFILE: viewName = "User Profile"; break;
        case ViewMode::HELP: viewName = "Help"; break;
        case ViewMode::COMMAND_HISTORY: viewName = "Command History"; break;
    }
    
    std::cout << std::endl << Color::CYAN << "[" << viewName << "] " << Color::RESET << "> ";
}

void CLI::setView(ViewMode view) {
    currentView = view;
    clearScreen();
    
    switch (view) {
        case ViewMode::MAIN_MENU:
            displayMainMenu();
            break;
        case ViewMode::FOOD_DATABASE:
            displayFoodDatabaseView();
            break;
        case ViewMode::FOOD_DETAIL:
            displayFoodDetailView(foodDetailId);
            break;
        case ViewMode::LOG_ENTRY:
            displayLogEntryView();
            break;
        case ViewMode::USER_PROFILE:
            displayUserProfileView();
            break;
        case ViewMode::HELP:
            displayHelp();
            break;
        case ViewMode::COMMAND_HISTORY:
            displayCommandHistory();
            break;
    }
    
    displayFooter();
}

void CLI::addToHistory(const std::string& command) {
    // Add command to history, keeping it at the max size
    commandHistory.push_front(command);
    if (commandHistory.size() > maxHistorySize) {
        commandHistory.pop_back();
    }
}

void CLI::displayMainMenu() const {
    displayHeader("DIET MANAGER - MAIN MENU");
    displayStatusBar();
    
    std::cout << Color::YELLOW << "╔══════════════════════════════╗" << Color::RESET << std::endl;
    std::cout << Color::YELLOW << "║" << Color::RESET << Color::INFO << " Diet Manager Main Menu       " << Color::RESET << Color::YELLOW << "║" << Color::RESET << std::endl;
    std::cout << Color::YELLOW << "╚══════════════════════════════╝" << Color::RESET << std::endl;
    
    std::cout << std::endl;
    std::cout << Color::SUCCESS << "Available Views:" << Color::RESET << std::endl;
    std::cout << "  " << Color::BOLD << "1." << Color::RESET << " " << Color::INFO << "Food Database" << Color::RESET << " - Manage foods" << std::endl;
    std::cout << "  " << Color::BOLD << "2." << Color::RESET << " " << Color::INFO << "Log Entry" << Color::RESET << " - Track daily food consumption" << std::endl;
    std::cout << "  " << Color::BOLD << "3." << Color::RESET << " " << Color::INFO << "User Profile" << Color::RESET << " - Update personal information" << std::endl;
    std::cout << "  " << Color::BOLD << "4." << Color::RESET << " " << Color::INFO << "Help" << Color::RESET << " - Show available commands" << std::endl;
    std::cout << "  " << Color::BOLD << "5." << Color::RESET << " " << Color::INFO << "Command History" << Color::RESET << " - View previous commands" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Use '" << Color::MAGENTA << "view <name>" << Color::RESET << "' to switch views, e.g. '" << 
               Color::MAGENTA << "view food" << Color::RESET << "'" << std::endl;
    std::cout << "Type '" << Color::ERROR << "exit" << Color::RESET << "' to quit the application" << std::endl;
    
    // Display today's summary
    std::cout << std::endl;
    std::cout << Color::GREEN << "Today's Summary:" << Color::RESET << std::endl;
    std::chrono::system_clock::time_point today = logManager->getCurrentDate();
    double consumed = logManager->getConsumedCalories(today);
    double target = logManager->getTargetCalories();
    double difference = target - consumed;
    
    std::cout << "  Date: " << Color::YELLOW << LogEntry::dateToString(today) << Color::RESET << std::endl;
    std::cout << "  Consumed calories: " << Color::CYAN << consumed << Color::RESET << std::endl;
    std::cout << "  Target calories: " << Color::CYAN << target << Color::RESET << std::endl;
    
    // Show remaining or excess calories
    if (difference >= 0) {
        std::cout << "  Remaining: " << Color::SUCCESS << difference << " calories" << Color::RESET << std::endl;
    } else {
        std::cout << "  Excess: " << Color::ERROR << -difference << " calories" << Color::RESET << std::endl;
    }
}

// More view methods...

// New command handlers for view navigation
void CLI::handleView(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: view <main|food|log|profile|help|history>" << std::endl;
        return;
    }
    
    const std::string& viewName = args[0];
    
    if (viewName == "main" || viewName == "menu") {
        setView(ViewMode::MAIN_MENU);
    } else if (viewName == "food" || viewName == "foods") {
        setView(ViewMode::FOOD_DATABASE);
    } else if (viewName == "log") {
        setView(ViewMode::LOG_ENTRY);
    } else if (viewName == "profile") {
        setView(ViewMode::USER_PROFILE);
    } else if (viewName == "help") {
        setView(ViewMode::HELP);
    } else if (viewName == "history") {
        setView(ViewMode::COMMAND_HISTORY);
    } else {
        std::cout << Color::RED << "Unknown view: " << viewName << Color::RESET << std::endl;
    }
}

void CLI::handleRefresh(const std::vector<std::string>& args) {
    setView(currentView);
}

void CLI::handleHistory(const std::vector<std::string>& args) {
    setView(ViewMode::COMMAND_HISTORY);
}

void CLI::displayCommandHistory() const {
    displayHeader("COMMAND HISTORY");
    
    if (commandHistory.empty()) {
        std::cout << Color::WARNING << "No commands in history." << Color::RESET << std::endl;
        return;
    }
    
    std::cout << Color::SUCCESS << "Recent commands:" << Color::RESET << std::endl;
    
    size_t count = 20; // Default number of commands to show
    
    size_t i = 0;
    for (const auto& cmd : commandHistory) {
        if (i >= count) break;
        
        std::cout << Color::INFO << std::setw(3) << (i + 1) << ". " << Color::RESET << cmd << std::endl;
        ++i;
    }
}

void CLI::handleViewFood(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: view-food <food_id>" << std::endl;
        return;
    }
    
    std::string foodId = args[0];
    auto food = foodDatabase->getFood(foodId);
    
    if (!food) {
        std::cout << Color::RED << "Food not found: " << foodId << Color::RESET << std::endl;
        return;
    }
    
    // Set the food ID for the detail view and switch to it
    foodDetailId = foodId;
    setView(ViewMode::FOOD_DETAIL);
}

std::string CLI::formatCalories(double calories) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << calories;
    return ss.str();
}

void CLI::displayFoodDatabaseView() const {
    displayHeader("FOOD DATABASE");
    displayStatusBar();
    
    auto foods = foodDatabase->getAllFoods();
    
    if (foods.empty()) {
        std::cout << Color::YELLOW << "No foods in the database." << Color::RESET << std::endl;
        return;
    }
    
    // Sort foods by name
    std::sort(foods.begin(), foods.end(), 
              [](const auto& a, const auto& b) { return a->getId() < b->getId(); });
    
    // Separate basic and composite foods
    std::vector<std::shared_ptr<Food>> basicFoods;
    std::vector<std::shared_ptr<Food>> compositeFoods;
    
    for (const auto& food : foods) {
        if (std::dynamic_pointer_cast<BasicFood>(food)) {
            basicFoods.push_back(food);
        } else {
            compositeFoods.push_back(food);
        }
    }
    
    // Display basic foods
    if (!basicFoods.empty()) {
        std::cout << Color::CYAN << "╔" << std::string(termWidth - 2, '=') << "╗" << Color::RESET << std::endl;
        std::cout << Color::CYAN << "║" << Color::RESET << " BASIC FOODS " << std::string(termWidth - 14, ' ') << Color::CYAN << "║" << Color::RESET << std::endl;
        std::cout << Color::CYAN << "╠" << std::string(termWidth - 2, '=') << "╣" << Color::RESET << std::endl;
        
        std::cout << Color::WHITE << std::left << std::setw(25) << "ID" 
                  << std::setw(10) << "CALORIES" << "KEYWORDS" << Color::RESET << std::endl;
        std::cout << Color::CYAN << "╠" << std::string(termWidth - 2, '=') << "╣" << Color::RESET << std::endl;
        
        for (const auto& food : basicFoods) {
            std::string keywordStr;
            for (const auto& keyword : food->getKeywords()) {
                if (!keywordStr.empty()) keywordStr += ", ";
                keywordStr += keyword;
            }
            if (keywordStr.length() > 35) {
                keywordStr = keywordStr.substr(0, 32) + "...";
            }
            
            std::cout << Color::GREEN << std::left << std::setw(25) << food->getId()
                      << Color::YELLOW << std::setw(10) << formatCalories(food->getCaloriesPerServing())
                      << Color::DIM << keywordStr << Color::RESET << std::endl;
        }
        std::cout << Color::CYAN << "╚" << std::string(termWidth - 2, '=') << "╝" << Color::RESET << std::endl;
    }
    
    // Display composite foods
    if (!compositeFoods.empty()) {
        std::cout << std::endl;
        std::cout << Color::MAGENTA << "╔" << std::string(termWidth - 2, '=') << "╗" << Color::RESET << std::endl;
        std::cout << Color::MAGENTA << "║" << Color::RESET << " COMPOSITE FOODS " << std::string(termWidth - 18, ' ') << Color::MAGENTA << "║" << Color::RESET << std::endl;
        std::cout << Color::MAGENTA << "╠" << std::string(termWidth - 2, '=') << "╣" << Color::RESET << std::endl;
        
        std::cout << Color::WHITE << std::left << std::setw(25) << "ID" 
                  << std::setw(10) << "CALORIES" << "KEYWORDS" << Color::RESET << std::endl;
        std::cout << Color::MAGENTA << "╠" << std::string(termWidth - 2, '=') << "╣" << Color::RESET << std::endl;
        
        for (const auto& food : compositeFoods) {
            std::string keywordStr;
            for (const auto& keyword : food->getKeywords()) {
                if (!keywordStr.empty()) keywordStr += ", ";
                keywordStr += keyword;
            }
            if (keywordStr.length() > 35) {
                keywordStr = keywordStr.substr(0, 32) + "...";
            }
            
            std::cout << Color::BLUE << std::left << std::setw(25) << food->getId()
                      << Color::YELLOW << std::setw(10) << formatCalories(food->getCaloriesPerServing())
                      << Color::DIM << keywordStr << Color::RESET << std::endl;
        }
        std::cout << Color::MAGENTA << "╚" << std::string(termWidth - 2, '=') << "╝" << Color::RESET << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Use '" << Color::CYAN << "view-food <id>" << Color::RESET << "' to see food details" << std::endl;
    std::cout << "Use '" << Color::CYAN << "add-basic-food" << Color::RESET << "' to add a new basic food" << std::endl;
    std::cout << "Use '" << Color::CYAN << "create-composite" << Color::RESET << "' to create a composite food" << std::endl;
    std::cout << "Use '" << Color::CYAN << "search-foods" << Color::RESET << "' to search for foods" << std::endl;
}

// More UI methods would follow...

std::vector<std::string> CLI::parseInput(const std::string& input) const {
    std::vector<std::string> tokens;
    std::istringstream iss(input);
    std::string token;
    
    // Handle quoted arguments
    bool inQuote = false;
    std::string quotedArg;
    
    while (iss >> std::quoted(token)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

void CLI::displayHelp() const {
    std::cout << "Available commands:" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    // Group commands by category
    std::map<std::string, std::vector<Command>> categories = {
        {"General", {}},
        {"Food Database", {}},
        {"Log Management", {}},
        {"User Profile", {}},
        {"Data Management", {}}
    };
    
    // Assign commands to categories
    for (const auto& pair : commands) {
        const auto& cmd = pair.second;
        
        if (cmd.name == "help" || cmd.name == "quit" || cmd.name == "exit") {
            categories["General"].push_back(cmd);
        } else if (cmd.name == "add-basic-food" || cmd.name == "list-foods" || 
                  cmd.name == "search-foods" || cmd.name == "create-composite") {
            categories["Food Database"].push_back(cmd);
        } else if (cmd.name == "add-food" || cmd.name == "remove-food" || 
                  cmd.name == "view-log" || cmd.name == "set-date" || 
                  cmd.name == "undo" || cmd.name == "redo") {
            categories["Log Management"].push_back(cmd);
        } else if (cmd.name == "profile" || cmd.name == "calories") {
            categories["User Profile"].push_back(cmd);
        } else if (cmd.name == "save" || cmd.name == "load") {
            categories["Data Management"].push_back(cmd);
        }
    }
    
    // Display commands by category
    for (const auto& category : categories) {
        std::cout << category.first << ":" << std::endl;
        
        for (const auto& cmd : category.second) {
            std::cout << "  " << std::left << std::setw(20) << cmd.name 
                      << cmd.description << std::endl;
        }
        
        std::cout << std::endl;
    }
}

void CLI::displayLogo() const {
    std::cout << R"(
  _____  _      _     __  __                                   
 |  __ \(_)    | |   |  \/  |                                  
 | |  | |_  ___| |_  | \  / | __ _ _ __   __ _  __ _  ___ _ __ 
 | |  | | |/ _ \ __| | |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
 | |__| | |  __/ |_  | |  | | (_| | | | | (_| | (_| |  __/ |   
 |_____/|_|\___|\__| |_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|   
                                                __/ |          
                                               |___/           
)" << std::endl;
}

void CLI::loadUserProfile() {
    std::ifstream file(userFilePath);
    if (!file.is_open()) {
        std::cout << "No user profile found. Creating a new profile..." << std::endl;
        createUserProfile();
        return;
    }
    
    try {
        json j;
        file >> j;
        file.close();
        
        user = std::make_shared<User>(User::fromJson(j.dump()));
        std::cout << "User profile loaded successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading user profile: " << e.what() << std::endl;
        createUserProfile();
    }
}

void CLI::saveUserProfile() const {
    if (!user) {
        return;
    }
    
    std::ofstream file(userFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << userFilePath << std::endl;
        return;
    }
    
    std::stringstream ss;
    user->toJson(ss);
    file << ss.str();
    file.close();
}

void CLI::createUserProfile() {
    user = std::make_shared<User>();
    
    std::cout << "Let's create a new user profile." << std::endl;
    
    std::string genderStr;
    std::cout << "Gender (male/female/other): ";
    std::getline(std::cin, genderStr);
    Gender gender = User::genderFromString(genderStr);
    user->setGender(gender);
    
    double height;
    std::cout << "Height (in cm): ";
    std::cin >> height;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    user->setHeightCm(height);
    
    int age;
    std::cout << "Age: ";
    std::cin >> age;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    user->setAge(age);
    
    double weight;
    std::cout << "Weight (in kg): ";
    std::cin >> weight;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    user->setWeightKg(weight);
    
    std::string activityStr;
    std::cout << "Activity level (sedentary/light/moderate/active/very_active): ";
    std::getline(std::cin, activityStr);
    ActivityLevel activity = User::activityLevelFromString(activityStr);
    user->setActivityLevel(activity);
    
    std::cout << "User profile created successfully." << std::endl;
    
    saveUserProfile();
}

void CLI::displayUserProfile() const {
    if (!user) {
        std::cout << "No user profile available." << std::endl;
        return;
    }
    
    std::cout << "User Profile:" << std::endl;
    std::cout << "Gender: " << User::genderToString(user->getGender()) << std::endl;
    std::cout << "Height: " << user->getHeightCm() << " cm" << std::endl;
    std::cout << "Age: " << user->getAge() << " years" << std::endl;
    std::cout << "Weight: " << user->getWeightKg() << " kg" << std::endl;
    std::cout << "Activity Level: " << User::activityLevelToString(user->getActivityLevel()) << std::endl;
    std::cout << "Calorie Calculation Method: " 
              << User::calorieMethodToString(user->getCalorieMethod()) << std::endl;
    std::cout << "Target Daily Calories: " << user->calculateTargetCalories() << std::endl;
}

// Command handlers
void CLI::handleQuit(const std::vector<std::string>& args) {
    running = false;
}

void CLI::handleHelp(const std::vector<std::string>& args) {
    displayHelp();
}

void CLI::handleAddBasicFood(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cout << "Usage: add-basic-food <id> <calories> <keyword1> [keyword2] ..." << std::endl;
        return;
    }
    
    std::string id = args[0];
    
    // Parse calories
    double calories;
    try {
        calories = std::stod(args[1]);
    } catch (const std::exception& e) {
        std::cout << "Invalid calories value. Please enter a valid number." << std::endl;
        return;
    }
    
    // Parse keywords
    std::vector<std::string> keywords(args.begin() + 2, args.end());
    if (keywords.empty()) {
        std::cout << "At least one keyword is required." << std::endl;
        return;
    }
    
    // Create and add the food
    auto food = std::make_shared<BasicFood>(id, keywords, calories);
    
    if (foodDatabase->addFood(food)) {
        std::cout << "Food '" << id << "' added successfully." << std::endl;
    } else {
        std::cout << "Failed to add food. Food with ID '" << id << "' might already exist." << std::endl;
    }
}

void CLI::handleListFoods(const std::vector<std::string>& args) {
    auto foods = foodDatabase->getAllFoods();
    
    if (foods.empty()) {
        std::cout << "No foods in the database." << std::endl;
        return;
    }
    
    std::cout << "Foods in the database:" << std::endl;
    std::cout << std::left << std::setw(20) << "ID" 
              << std::left << std::setw(10) << "Type"
              << std::left << std::setw(10) << "Calories"
              << "Keywords" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& food : foods) {
        std::string type;
        if (std::dynamic_pointer_cast<BasicFood>(food)) {
            type = "Basic";
        } else if (std::dynamic_pointer_cast<CompositeFood>(food)) {
            type = "Composite";
        }
        
        std::string keywordStr;
        for (const auto& keyword : food->getKeywords()) {
            if (!keywordStr.empty()) {
                keywordStr += ", ";
            }
            keywordStr += keyword;
        }
        
        std::cout << std::left << std::setw(20) << food->getId()
                  << std::left << std::setw(10) << type
                  << std::left << std::setw(10) << food->getCaloriesPerServing()
                  << keywordStr << std::endl;
    }
}

void CLI::handleSearchFoods(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: search-foods <keyword1> [keyword2] ... [--all]" << std::endl;
        return;
    }
    
    bool matchAll = false;
    std::vector<std::string> keywords;
    
    // Process arguments
    for (const auto& arg : args) {
        if (arg == "--all") {
            matchAll = true;
        } else {
            keywords.push_back(arg);
        }
    }
    
    if (keywords.empty()) {
        std::cout << "At least one keyword is required." << std::endl;
        return;
    }
    
    // Perform the search
    std::vector<std::shared_ptr<Food>> results;
    if (matchAll) {
        results = foodDatabase->searchByAllKeywords(keywords);
    } else {
        results = foodDatabase->searchByAnyKeyword(keywords);
    }
    
    if (results.empty()) {
        std::cout << "No foods found matching the search criteria." << std::endl;
        return;
    }
    
    // Display results
    std::cout << "Search results:" << std::endl;
    std::cout << std::left << std::setw(20) << "ID" 
              << std::left << std::setw(10) << "Type"
              << std::left << std::setw(10) << "Calories"
              << "Keywords" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    
    for (const auto& food : results) {
        std::string type;
        if (std::dynamic_pointer_cast<BasicFood>(food)) {
            type = "Basic";
        } else if (std::dynamic_pointer_cast<CompositeFood>(food)) {
            type = "Composite";
        }
        
        std::string keywordStr;
        for (const auto& keyword : food->getKeywords()) {
            if (!keywordStr.empty()) {
                keywordStr += ", ";
            }
            keywordStr += keyword;
        }
        
        std::cout << std::left << std::setw(20) << food->getId()
                  << std::left << std::setw(10) << type
                  << std::left << std::setw(10) << food->getCaloriesPerServing()
                  << keywordStr << std::endl;
    }
}

void CLI::handleCreateCompositeFood(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: create-composite <id> <keyword1> [keyword2] ... --components <food1> <servings1> [<food2> <servings2> ...]" << std::endl;
        return;
    }
    
    std::string id = args[0];
    
    // Find the --components marker
    auto it = std::find(args.begin() + 1, args.end(), "--components");
    if (it == args.end()) {
        std::cout << "No components specified. Use --components followed by food IDs and servings." << std::endl;
        return;
    }
    
    // Extract keywords (everything between id and --components)
    std::vector<std::string> keywords(args.begin() + 1, it);
    if (keywords.empty()) {
        std::cout << "At least one keyword is required." << std::endl;
        return;
    }
    
    // Extract components (after --components)
    std::vector<std::string> componentArgs(it + 1, args.end());
    if (componentArgs.empty() || componentArgs.size() % 2 != 0) {
        std::cout << "Components must be specified in pairs: <food_id> <servings>" << std::endl;
        return;
    }
    
    // Create the composite food
    auto compositeFood = std::make_shared<CompositeFood>(id, keywords);
    
    // Add components
    for (size_t i = 0; i < componentArgs.size(); i += 2) {
        std::string foodId = componentArgs[i];
        double servings;
        
        try {
            servings = std::stod(componentArgs[i + 1]);
        } catch (const std::exception& e) {
            std::cout << "Invalid servings value for food '" << foodId << "'." << std::endl;
            return;
        }
        
        auto food = foodDatabase->getFood(foodId);
        if (!food) {
            std::cout << "Food '" << foodId << "' not found in the database." << std::endl;
            return;
        }
        
        compositeFood->addComponent(food, servings);
    }
    
    if (foodDatabase->addFood(compositeFood)) {
        std::cout << "Composite food '" << id << "' created successfully." << std::endl;
    } else {
        std::cout << "Failed to create composite food. Food with ID '" << id << "' might already exist." << std::endl;
    }
}

void CLI::handleAddFoodToLog(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: add-food <food_id> <servings>" << std::endl;
        return;
    }
    
    std::string foodId = args[0];
    double servings;
    
    try {
        servings = std::stod(args[1]);
    } catch (const std::exception& e) {
        std::cout << "Invalid servings value. Please enter a valid number." << std::endl;
        return;
    }
    
    if (servings <= 0) {
        std::cout << "Servings must be greater than zero." << std::endl;
        return;
    }
    
    auto command = std::make_shared<AddFoodCommand>(logManager.get(), foodId, servings, logManager->getCurrentDate());
    logManager->executeCommand(command);
    
    // Confirm the operation
    auto food = foodDatabase->getFood(foodId);
    if (food) {
        double calories = food->getCaloriesPerServing() * servings;
        std::cout << "Added " << servings << " serving(s) of '" << foodId 
                 << "' (" << calories << " calories) to the log." << std::endl;
    } else {
        std::cout << "Food added to log, but could not find details." << std::endl;
    }
}

void CLI::handleRemoveFoodFromLog(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: remove-food <food_id>" << std::endl;
        return;
    }
    
    std::string foodId = args[0];
    
    auto command = std::make_shared<RemoveFoodCommand>(logManager.get(), foodId, logManager->getCurrentDate());
    logManager->executeCommand(command);
    
    std::cout << "Food '" << foodId << "' removed from the log." << std::endl;
}

void CLI::handleViewLogEntry(const std::vector<std::string>& args) {
    std::chrono::system_clock::time_point date;
    
    if (args.empty()) {
        date = logManager->getCurrentDate();
    } else {
        try {
            date = LogEntry::stringToDate(args[0]);
        } catch (const std::exception& e) {
            std::cout << "Invalid date format. Use YYYY-MM-DD." << std::endl;
            return;
        }
    }
    
    auto logEntry = logManager->getLogEntry(date);
    auto foods = logEntry->getConsumedFoods();
    
    std::cout << "Log for " << LogEntry::dateToString(date) << ":" << std::endl;
    
    if (foods.empty()) {
        std::cout << "No foods logged for this date." << std::endl;
        return;
    }
    
    double totalCalories = 0.0;
    
    std::cout << std::left << std::setw(20) << "Food ID" 
              << std::left << std::setw(10) << "Servings"
              << std::left << std::setw(10) << "Calories"
              << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    
    for (const auto& foodTuple : foods) {
        // Unpack the tuple (Food, servings, mealType)
        auto& food = std::get<0>(foodTuple);
        double servings = std::get<1>(foodTuple);
        // MealType mealType = std::get<2>(foodTuple); // If needed later
        
        double calories = food->getCaloriesPerServing() * servings;
        totalCalories += calories;
        
        std::cout << std::left << std::setw(20) << food->getId()
                 << std::left << std::setw(10) << servings
                 << std::left << std::setw(10) << calories
                 << std::endl;
    }
    
    std::cout << std::string(50, '-') << std::endl;
    std::cout << std::left << std::setw(30) << "Total Calories:" 
              << totalCalories << std::endl;
    
    double targetCalories = logManager->getTargetCalories();
    double difference = logManager->getCalorieDifference(date);
    
    std::cout << std::left << std::setw(30) << "Target Calories:" 
              << targetCalories << std::endl;
    std::cout << std::left << std::setw(30) << "Difference:" 
              << difference << " (" << (difference < 0 ? "Under" : "Over") << " target)" << std::endl;
}

void CLI::handleSetDate(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: set-date <date> (format: YYYY-MM-DD)" << std::endl;
        std::cout << "Current date: " << LogEntry::dateToString(logManager->getCurrentDate()) << std::endl;
        return;
    }
    
    try {
        auto date = LogEntry::stringToDate(args[0]);
        logManager->setCurrentDate(date);
        std::cout << "Current date set to: " << args[0] << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Invalid date format. Use YYYY-MM-DD." << std::endl;
    }
}

void CLI::handleUndo(const std::vector<std::string>& args) {
    if (logManager->canUndo()) {
        logManager->undo();
        std::cout << "Undone last operation." << std::endl;
    } else {
        std::cout << "Nothing to undo." << std::endl;
    }
}

void CLI::handleRedo(const std::vector<std::string>& args) {
    if (logManager->canRedo()) {
        logManager->redo();
        std::cout << "Redone last undone operation." << std::endl;
    } else {
        std::cout << "Nothing to redo." << std::endl;
    }
}

void CLI::handleUpdateProfile(const std::vector<std::string>& args) {
    if (args.empty()) {
        displayUserProfile();
        return;
    }
    
    if (args[0] == "gender" && args.size() > 1) {
        user->setGender(User::genderFromString(args[1]));
        std::cout << "Gender updated to " << User::genderToString(user->getGender()) << "." << std::endl;
    } else if (args[0] == "height" && args.size() > 1) {
        try {
            double height = std::stod(args[1]);
            if (height > 0) {
                user->setHeightCm(height);
                std::cout << "Height updated to " << height << " cm." << std::endl;
            } else {
                std::cout << "Height must be positive." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Invalid height value." << std::endl;
        }
    } else if (args[0] == "age" && args.size() > 1) {
        try {
            int age = std::stoi(args[1]);
            if (age > 0 && age <= 120) {
                user->setAge(age);
                std::cout << "Age updated to " << age << "." << std::endl;
            } else {
                std::cout << "Age must be between 1 and 120." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Invalid age value." << std::endl;
        }
    } else if (args[0] == "weight" && args.size() > 1) {
        try {
            double weight = std::stod(args[1]);
            if (weight > 0) {
                user->setWeightKg(weight);
                std::cout << "Weight updated to " << weight << " kg." << std::endl;
            } else {
                std::cout << "Weight must be positive." << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "Invalid weight value." << std::endl;
        }
    } else if (args[0] == "activity" && args.size() > 1) {
        ActivityLevel oldLevel = user->getActivityLevel();
        user->setActivityLevel(User::activityLevelFromString(args[1]));
        std::cout << "Activity level updated from " << User::activityLevelToString(oldLevel) 
                  << " to " << User::activityLevelToString(user->getActivityLevel()) << "." << std::endl;
        
        // Show how this affects calories
        double oldCalories = user->calculateTargetCalories();
        user->setActivityLevel(User::activityLevelFromString(args[1]));
        double newCalories = user->calculateTargetCalories();
        std::cout << "This changes your target calories from " << oldCalories 
                  << " to " << newCalories << " calories per day." << std::endl;
    } else if (args[0] == "method" && args.size() > 1) {
        CalorieCalculationMethod oldMethod = user->getCalorieMethod();
        double oldCalories = user->calculateTargetCalories();
        
        user->setCalorieMethod(User::calorieMethodFromString(args[1]));
        double newCalories = user->calculateTargetCalories();
        
        std::cout << "Calorie calculation method updated from " << User::calorieMethodToString(oldMethod)
                  << " to " << User::calorieMethodToString(user->getCalorieMethod()) << "." << std::endl;
        std::cout << "This changes your target calories from " << oldCalories 
                  << " to " << newCalories << " calories per day." << std::endl;
    } else {
        std::cout << "Usage: profile [gender|height|age|weight|activity|method] [value]" << std::endl;
        std::cout << "Example: profile weight 65" << std::endl;
        return;
    }
    
    // Save changes to profile
    saveUserProfile();
    std::cout << "Profile updated and saved." << std::endl;
}

void CLI::handleShowCalories(const std::vector<std::string>& args) {
    std::chrono::system_clock::time_point date;
    
    if (args.empty()) {
        date = logManager->getCurrentDate();
    } else {
        try {
            date = LogEntry::stringToDate(args[0]);
        } catch (const std::exception& e) {
            std::cout << "Invalid date format. Use YYYY-MM-DD." << std::endl;
            return;
        }
    }
    
    double consumed = logManager->getConsumedCalories(date);
    double target = logManager->getTargetCalories();
    double difference = logManager->getCalorieDifference(date);
    
    std::cout << "Calorie Summary for " << LogEntry::dateToString(date) << ":" << std::endl;
    std::cout << "Consumed Calories: " << consumed << std::endl;
    std::cout << "Target Calories: " << target << std::endl;
    std::cout << "Difference: " << difference << " (" 
              << (difference < 0 ? "Under" : "Over") << " target)" << std::endl;
}

void CLI::handleSaveData(const std::vector<std::string>& args) {
    saveUserProfile();
    foodDatabase->saveDatabase();
    logManager->saveLogs();
    
    std::cout << "All data saved successfully." << std::endl;
}

void CLI::handleLoadData(const std::vector<std::string>& args) {
    loadUserProfile();
    foodDatabase->loadDatabase();
    logManager->loadLogs();
    
    std::cout << "All data loaded successfully." << std::endl;
}

void CLI::displayFoodDetailView(const std::string& foodId) const {
    displayHeader("FOOD DETAILS");
    
    auto food = foodDatabase->getFood(foodId);
    if (!food) {
        std::cout << Color::RED << "Food not found with ID: " << foodId << Color::RESET << std::endl;
        return;
    }
    
    std::cout << Color::BLUE << "╔" << std::string(termWidth - 2, '=') << "╗" << Color::RESET << std::endl;
    std::cout << Color::BLUE << "║" << Color::RESET << " " << Color::BOLD << "Food Details: " << food->getId() << Color::RESET << std::string(termWidth - 16 - food->getId().length(), ' ') << Color::BLUE << "║" << Color::RESET << std::endl;
    std::cout << Color::BLUE << "╚" << std::string(termWidth - 2, '=') << "╝" << Color::RESET << std::endl;
    
    // Basic info
    std::cout << std::endl;
    std::cout << Color::CYAN << "Basic Information:" << Color::RESET << std::endl;
    std::cout << "ID: " << Color::YELLOW << food->getId() << Color::RESET << std::endl;
    std::cout << "Calories per serving: " << Color::GREEN << food->getCaloriesPerServing() << Color::RESET << std::endl;
    
    // Keywords
    std::cout << "Keywords: ";
    auto keywords = food->getKeywords();
    for (size_t i = 0; i < keywords.size(); i++) {
        std::cout << Color::MAGENTA << keywords[i] << Color::RESET;
        if (i < keywords.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    
    // If it's a composite food, show components
    auto compositeFood = std::dynamic_pointer_cast<CompositeFood>(food);
    if (compositeFood) {
        std::cout << std::endl;
        std::cout << Color::CYAN << "Components:" << Color::RESET << std::endl;
        
        auto components = compositeFood->getComponents();
        if (components.empty()) {
            std::cout << "No components (empty composite food)" << std::endl;
        } else {
            std::cout << std::left << std::setw(20) << "Food ID" 
                      << std::left << std::setw(10) << "Servings"
                      << std::left << std::setw(10) << "Calories"
                      << std::endl;
            std::cout << std::string(50, '-') << std::endl;
            
            double totalCalories = 0.0;
            
            for (const auto& component : components) {
                double componentCalories = component.first->getCaloriesPerServing() * component.second;
                totalCalories += componentCalories;
                
                std::cout << std::left << std::setw(20) << component.first->getId()
                         << std::left << std::setw(10) << component.second
                         << std::left << std::setw(10) << componentCalories
                         << std::endl;
            }
            
            std::cout << std::string(50, '-') << std::endl;
            std::cout << std::left << std::setw(30) << "Total Calories:" 
                      << totalCalories << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Use '" << Color::CYAN << "add-food " << food->getId() << " <servings>" << Color::RESET 
             << "' to add this food to your log" << std::endl;
    std::cout << "Use '" << Color::CYAN << "view food" << Color::RESET 
             << "' to return to food database" << std::endl;
}

void CLI::displayLogEntryView() const {
    displayHeader("LOG ENTRIES");
    displayStatusBar();
    
    // Current date's log
    auto date = logManager->getCurrentDate();
    auto logEntry = logManager->getLogEntry(date);
    auto foods = logEntry->getConsumedFoods();
    
    std::cout << Color::GREEN << "╔" << std::string(termWidth - 2, '=') << "╗" << Color::RESET << std::endl;
    std::cout << Color::GREEN << "║" << Color::RESET << " Log for " << LogEntry::dateToString(date) << std::string(termWidth - 20 - LogEntry::dateToString(date).length(), ' ') << Color::GREEN << "║" << Color::RESET << std::endl;
    std::cout << Color::GREEN << "╚" << std::string(termWidth - 2, '=') << "╝" << Color::RESET << std::endl;
    
    if (foods.empty()) {
        std::cout << Color::YELLOW << "No foods logged for this date." << Color::RESET << std::endl;
    } else {
        double totalCalories = 0.0;
        
        // Display by meal type if available
        std::map<MealType, std::vector<std::tuple<std::shared_ptr<Food>, double, MealType>>> foodsByMeal;
        for (const auto& foodTuple : foods) {
            MealType mealType = std::get<2>(foodTuple);
            foodsByMeal[mealType].push_back(foodTuple);
        }
        
        // Define meal type order for display
        std::vector<MealType> mealOrder = {
            MealType::BREAKFAST,
            MealType::LUNCH,
            MealType::DINNER,
            MealType::SNACK,
            MealType::OTHER
        };
        
        // Display foods grouped by meal type
        for (const auto& mealType : mealOrder) {
            if (foodsByMeal.find(mealType) != foodsByMeal.end() && !foodsByMeal[mealType].empty()) {
                std::cout << std::endl;
                std::cout << Color::CYAN << "● " << LogEntry::mealTypeToString(mealType) << ":" << Color::RESET << std::endl;
                
                // Table header
                std::cout << std::left << std::setw(20) << "Food ID" 
                          << std::left << std::setw(10) << "Servings"
                          << std::left << std::setw(10) << "Calories"
                          << std::endl;
                std::cout << std::string(40, '-') << std::endl;
                
                double mealCalories = 0.0;
                
                // Display foods for this meal
                for (const auto& foodTuple : foodsByMeal[mealType]) {
                    auto& food = std::get<0>(foodTuple);
                    double servings = std::get<1>(foodTuple);
                    
                    double calories = food->getCaloriesPerServing() * servings;
                    mealCalories += calories;
                    totalCalories += calories;
                    
                    std::cout << std::left << std::setw(20) << food->getId()
                             << std::left << std::setw(10) << servings
                             << std::left << std::setw(10) << calories
                             << std::endl;
                }
                
                std::cout << std::string(40, '-') << std::endl;
                std::cout << std::left << std::setw(30) << "Meal subtotal:" << mealCalories << std::endl;
            }
        }
        
        // Display total calories
        std::cout << std::endl;
        std::cout << std::left << std::setw(30) << "Total Calories:" 
                  << Color::BOLD << totalCalories << Color::RESET << std::endl;
        
        double targetCalories = logManager->getTargetCalories();
        double difference = targetCalories - totalCalories;
        
        std::cout << std::left << std::setw(30) << "Target Calories:" 
                  << targetCalories << std::endl;
        
        // Show remaining or excess calories
        if (difference >= 0) {
            std::cout << std::left << std::setw(30) << "Remaining:" 
                      << Color::GREEN << difference << Color::RESET << std::endl;
        } else {
            std::cout << std::left << std::setw(30) << "Excess:" 
                      << Color::RED << -difference << Color::RESET << std::endl;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Use '" << Color::CYAN << "add-food <food_id> <servings>" << Color::RESET 
             << "' to add food to this log" << std::endl;
    std::cout << "Use '" << Color::CYAN << "remove-food <food_id>" << Color::RESET 
             << "' to remove food from this log" << std::endl;
    std::cout << "Use '" << Color::CYAN << "set-date YYYY-MM-DD" << Color::RESET 
             << "' to view a different date's log" << std::endl;
}

void CLI::displayUserProfileView() const {
    displayHeader("USER PROFILE");
    
    if (!user) {
        std::cout << Color::RED << "No user profile available." << Color::RESET << std::endl;
        return;
    }
    
    std::cout << Color::YELLOW << "╔" << std::string(termWidth - 2, '=') << "╗" << Color::RESET << std::endl;
    std::cout << Color::YELLOW << "║" << Color::RESET << " " << Color::BOLD << "User Information" << Color::RESET << std::string(termWidth - 19, ' ') << Color::YELLOW << "║" << Color::RESET << std::endl;
    std::cout << Color::YELLOW << "╚" << std::string(termWidth - 2, '=') << "╝" << Color::RESET << std::endl;
    
    std::cout << std::endl;
    std::cout << std::left << std::setw(20) << "Gender:" << Color::CYAN << User::genderToString(user->getGender()) << Color::RESET << std::endl;
    std::cout << std::left << std::setw(20) << "Height:" << Color::CYAN << user->getHeightCm() << " cm" << Color::RESET << std::endl;
    std::cout << std::left << std::setw(20) << "Age:" << Color::CYAN << user->getAge() << " years" << Color::RESET << std::endl;
    std::cout << std::left << std::setw(20) << "Weight:" << Color::CYAN << user->getWeightKg() << " kg" << Color::RESET << std::endl;
    std::cout << std::left << std::setw(20) << "Activity Level:" << Color::CYAN << User::activityLevelToString(user->getActivityLevel()) << Color::RESET << std::endl;
    std::cout << std::left << std::setw(20) << "Calorie Method:" << Color::CYAN << User::calorieMethodToString(user->getCalorieMethod()) << Color::RESET << std::endl;
    
    std::cout << std::endl;
    std::cout << Color::GREEN << "╔" << std::string(termWidth - 2, '=') << "╗" << Color::RESET << std::endl;
    std::cout << Color::GREEN << "║" << Color::RESET << " " << Color::BOLD << "Calorie Information" << Color::RESET << std::string(termWidth - 22, ' ') << Color::GREEN << "║" << Color::RESET << std::endl;
    std::cout << Color::GREEN << "╚" << std::string(termWidth - 2, '=') << "╝" << Color::RESET << std::endl;
    
    double targetCalories = user->calculateTargetCalories();
    double consumed = logManager->getConsumedCalories(logManager->getCurrentDate());
    double difference = targetCalories - consumed;
    
    std::cout << std::endl;
    std::cout << std::left << std::setw(25) << "Target Daily Calories:" << Color::BOLD << targetCalories << Color::RESET << std::endl;
    std::cout << std::left << std::setw(25) << "Consumed Today:" << Color::BOLD << consumed << Color::RESET << std::endl;
    
    if (difference >= 0) {
        std::cout << std::left << std::setw(25) << "Remaining Calories:" << Color::GREEN << difference << Color::RESET << std::endl;
    } else {
        std::cout << std::left << std::setw(25) << "Excess Calories:" << Color::RED << -difference << Color::RESET << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Use '" << Color::CYAN << "profile <attribute> <value>" << Color::RESET 
             << "' to update your profile" << std::endl;
    std::cout << "Attributes: gender, height, age, weight, activity, method" << std::endl;
}
