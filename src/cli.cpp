#include "cli.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

CLI::CLI(const std::string& basicFoodFilePath, 
         const std::string& compositeFoodFilePath,
         const std::string& logFilePath,
         const std::string& userFilePath)
    : userFilePath(userFilePath), running(false) {
    
    // Initialize food database
    foodDatabase = std::make_shared<FoodDatabase>(basicFoodFilePath, compositeFoodFilePath);
    
    // Load user profile or create a new one
    loadUserProfile();
    
    // Initialize log manager
    logManager = std::make_shared<LogManager>(logFilePath, user, foodDatabase);
    
    // Register available commands
    registerCommands();
}

void CLI::run() {
    running = true;
    
    // Display welcome message and logo
    displayLogo();
    std::cout << "Welcome to Diet Manager!" << std::endl;
    std::cout << "Type 'help' to see available commands." << std::endl;
    
    // Load data
    foodDatabase->loadDatabase();
    logManager->loadLogs();
    
    // Main CLI loop
    std::string input;
    while (running) {
        std::cout << "\n> ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            continue;
        }
        
        // Parse input into command and arguments
        std::vector<std::string> tokens = parseInput(input);
        if (tokens.empty()) {
            continue;
        }
        
        std::string commandName = tokens[0];
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());
        
        // Find and execute command
        auto it = commands.find(commandName);
        if (it != commands.end()) {
            it->second.handler(args);
        } else {
            std::cout << "Unknown command: " << commandName << std::endl;
            std::cout << "Type 'help' to see available commands." << std::endl;
        }
    }
    
    // Save data before exiting
    saveUserProfile();
    foodDatabase->saveDatabase();
    logManager->saveLogs();
    
    std::cout << "Thank you for using Diet Manager. Goodbye!" << std::endl;
}

void CLI::registerCommands() {
    commands["help"] = {"help", "Display available commands", [this](const auto& args) { handleHelp(args); }};
    commands["quit"] = {"quit", "Exit the program", [this](const auto& args) { handleQuit(args); }};
    commands["exit"] = {"exit", "Exit the program", [this](const auto& args) { handleQuit(args); }};
    
    // Food database commands
    commands["add-basic-food"] = {"add-basic-food", "Add a new basic food item", 
                                 [this](const auto& args) { handleAddBasicFood(args); }};
    commands["list-foods"] = {"list-foods", "List all available foods", 
                             [this](const auto& args) { handleListFoods(args); }};
    commands["search-foods"] = {"search-foods", "Search for foods by keywords", 
                               [this](const auto& args) { handleSearchFoods(args); }};
    commands["create-composite"] = {"create-composite", "Create a new composite food", 
                                   [this](const auto& args) { handleCreateCompositeFood(args); }};
    
    // Log management commands
    commands["add-food"] = {"add-food", "Add food to daily log", 
                           [this](const auto& args) { handleAddFoodToLog(args); }};
    commands["remove-food"] = {"remove-food", "Remove food from daily log", 
                              [this](const auto& args) { handleRemoveFoodFromLog(args); }};
    commands["view-log"] = {"view-log", "View the log for a specific date", 
                           [this](const auto& args) { handleViewLogEntry(args); }};
    commands["set-date"] = {"set-date", "Set the current working date", 
                           [this](const auto& args) { handleSetDate(args); }};
    commands["undo"] = {"undo", "Undo the last log operation", 
                       [this](const auto& args) { handleUndo(args); }};
    commands["redo"] = {"redo", "Redo the last undone operation", 
                       [this](const auto& args) { handleRedo(args); }};
    
    // User profile commands
    commands["profile"] = {"profile", "Display or update user profile", 
                          [this](const auto& args) { handleUpdateProfile(args); }};
    commands["calories"] = {"calories", "Show calorie intake and target", 
                           [this](const auto& args) { handleShowCalories(args); }};
    
    // Data management commands
    commands["save"] = {"save", "Save all data", 
                       [this](const auto& args) { handleSaveData(args); }};
    commands["load"] = {"load", "Load all data", 
                       [this](const auto& args) { handleLoadData(args); }};
}

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
    
    for (const auto& food : foods) {
        double calories = food.first->getCaloriesPerServing() * food.second;
        totalCalories += calories;
        
        std::cout << std::left << std::setw(20) << food.first->getId()
                 << std::left << std::setw(10) << food.second
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
