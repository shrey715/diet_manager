/**
 * @file cli.cpp
 * @brief Command Line Interface (CLI) Implementation
 * 
 * This file implements the CLI class which handles user interactions with the diet manager
 * application. It processes commands entered by users, executes the appropriate functions,
 * and displays output in a formatted manner. The CLI connects the user interface with
 * the core functionality of the application, including food database management, user profile
 * operations, food logging, and calculation of nutritional information.
 * 
 * Key responsibilities:
 * - Processing user commands and arguments
 * - Displaying formatted output to the console
 * - Error handling and validation of user input
 * - Managing the application state and flow
 * - Handling data persistence operations (save/load)
 */

#include "cli.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <regex>
#include <fstream>

using namespace std;

/**
 * CLI Constructor
 * Initializes the command-line interface.
 */
CLI::CLI()
    : foodDb(FoodDatabase::getInstance()),
      userProfile(UserProfile::getInstance()) {
    registerCommands();
    
    // Set current date
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d");
    currentDate = ss.str();
    logHistory.setCurrentDate(currentDate);
    
    // Load data - this will also trigger user profile initialization if needed
    try {
        loadData({});
    } catch (const exception& e) {
        cerr << TerminalColors::error("Error loading data: ") << e.what() << endl;
    }
    
    // Make sure user is initialized
    userProfile.getUser();
}

/**
 * registerCommands Method
 * Registers all available commands with their handlers and help text.
 */
void CLI::registerCommands() {
    // General commands
    commands["help"] = [this](const auto& args) { displayHelp(args); };
    helpText["help"] = "Display available commands";

    commands["clear"] = [this](const auto& args) { clearScreen(args); };
    helpText["clear"] = "Clear the terminal screen";
    
    commands["quit"] = [this](const auto& args) { quitProgram(args); };
    helpText["quit"] = "Exit the program";
    
    commands["exit"] = [this](const auto& args) { quitProgram(args); };
    helpText["exit"] = "Exit the program";
    
    // Food database commands
    commands["add-basic-food"] = [this](const auto& args) { addBasicFood(args); };
    helpText["add-basic-food"] = "add-basic-food <calories> <keyword1> [keyword2] ... - Add a new basic food";
    
    commands["list-foods"] = [this](const auto& args) { listFoods(args); };
    helpText["list-foods"] = "list-foods - List all available foods";
    
    commands["search-foods"] = [this](const auto& args) { searchFoods(args); };
    helpText["search-foods"] = "search-foods <keyword1> [keyword2] ... [--all] - Search for foods by keywords";
    
    commands["create-composite"] = [this](const auto& args) { createCompositeFood(args); };
    helpText["create-composite"] = "create-composite <keyword1> [keyword2] ... --components <food1> <servings1> [<food2> <servings2> ...] - Create a composite food";
    
    // Log commands
    commands["add-food"] = [this](const auto& args) { addFoodToLog(args); };
    helpText["add-food"] = "add-food <food_id> <servings> - Add food to the current day's log";
    
    commands["remove-food"] = [this](const auto& args) { removeFoodFromLog(args); };
    helpText["remove-food"] = "remove-food <food_id> - Remove food from the log";
    
    commands["view-log"] = [this](const auto& args) { viewLog(args); };
    helpText["view-log"] = "view-log [date] - View the log for a specific date or current date";
    
    commands["set-date"] = [this](const auto& args) { setDate(args); };
    helpText["set-date"] = "set-date <YYYY-MM-DD> - Set the current working date";
    
    commands["undo"] = [this](const auto& args) { undoCommand(args); };
    helpText["undo"] = "undo - Undo the last log operation";
    
    commands["redo"] = [this](const auto& args) { redoCommand(args); };
    helpText["redo"] = "redo - Redo the last undone operation";
    
    // User profile commands
    commands["profile"] = [this](const auto& args) { 
        if (args.size() <= 1) viewProfile(args); 
        else updateProfile(args); 
    };
    helpText["profile"] = "profile - Display the user profile\nprofile <attribute> <value> - Update a profile attribute";
    
    commands["calories"] = [this](const auto& args) { viewCalories(args); };
    helpText["calories"] = "calories [date] - Show calorie intake and target";
    
    commands["history"] = [this](const auto& args) { viewDailyHistory(args); };
    helpText["history"] = "history [all|last N] - View history of your metrics (weight, age, activity level)";

    commands["save"] = [this](const auto& args) { saveData(args); };
    helpText["save"] = "save - Save the current state to disk";

    commands["load"] = [this](const auto& args) { loadData(args); };
    helpText["load"] = "load - Load the state from disk";
}

/**
 * run Method
 * Main loop for the CLI.
 */
void CLI::run() {
    cout << TerminalColors::bold("\nDiet Manager - Type 'help' for available commands\n");
    cout << "Current date: " << TerminalColors::info(currentDate) << endl << endl;
    
    while (true) {
        cout << TerminalColors::bold("> ");
        string line;
        getline(cin, line);
        
        if (line.empty()) {
            continue;
        }
        
        vector<string> args = parseCommandLine(line);
        string command = args[0];
        
        auto it = commands.find(command);
        if (it != commands.end()) {
            try {
                it->second(args);
            } catch (const exception& e) {
                cout << TerminalColors::error("Error: ") << e.what() << endl;
            }
        } else {
            cout << TerminalColors::error("Unknown command: ") << command << ". Type 'help' for available commands." << endl;
        }
    }
}

/**
 * parseCommandLine Method
 * @param line The command line to parse
 * @return A vector of command arguments
 */
vector<string> CLI::parseCommandLine(const string& line) {
    vector<string> args;
    string currentArg;
    bool inQuotes = false;
    
    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!currentArg.empty()) {
                args.push_back(currentArg);
                currentArg.clear();
            }
        } else {
            currentArg += c;
        }
    }
    
    if (!currentArg.empty()) {
        args.push_back(currentArg);
    }
    
    return args;
}

/**
 * displayHelp Method
 * @param args Command arguments
 * Shows help text for commands.
 */
void CLI::displayHelp(const vector<string>& args) {
    if (args.size() > 1) {
        string command = args[1];
        auto it = helpText.find(command);
        if (it != helpText.end()) {
            cout << TerminalColors::bold("Help for '" + command + "':\n");
            cout << it->second << endl;
        } else {
            cout << TerminalColors::error("No help available for '" + command + "'") << endl;
        }
    } else {
        cout << TerminalColors::bold("Available commands:\n");
        
        // Group commands by category
        map<string, vector<string>> categories = {
            {"General", {"help", "clear", "quit", "exit"}},
            {"Food Database", {"add-basic-food", "list-foods", "search-foods", "create-composite"}},
            {"Log Management", {"add-food", "remove-food", "view-log", "set-date", "undo", "redo"}},
            {"User Profile", {"profile", "calories", "history"}},
            {"Data Management", {"save", "load"}}
        };
        
        for (const auto& [category, categoryCommands] : categories) {
            cout << TerminalColors::underline("\n" + category + " Commands:\n");
            for (const auto& command : categoryCommands) {
                cout << TerminalColors::bold(command) << " - " << helpText[command].substr(0, helpText[command].find("\n")) << endl;
            }
        }
    }
}

/**
 * confirmAction Method
 * @param message The confirmation message
 * @return Whether the user confirmed the action
 */
bool CLI::confirmAction(const string& message) {
    cout << TerminalColors::warning(message + " (y/n): ");
    string response;
    getline(cin, response);
    return response == "y" || response == "Y" || response == "yes" || response == "Yes";
}

/**
 * viewLog Method
 * @param args Command arguments
 * Shows the log for a specific date.
 */
void CLI::viewLog(const vector<string>& args) {
    string date = currentDate;
    if (args.size() > 1) {
        date = args[1];
        // Validate date format
        regex dateRegex("\\d{4}-\\d{2}-\\d{2}");
        if (!regex_match(date, dateRegex)) {
            throw invalid_argument("Date must be in format YYYY-MM-DD");
        }
    }
    
    auto log = logHistory.getLog(date);
    auto foods = log->getFoods();
    
    cout << TerminalColors::bold("\nFood Log for " + date + ":\n");
    cout << left << setw(20) << "Food" << setw(10) << "Servings" << "Calories" << endl;
    cout << string(50, '-') << endl;
    
    float totalCalories = 0;
    
    for (const auto& [foodId, servings] : foods) {
        auto food = foodDb.getFood(foodId);
        if (!food) {
            cout << TerminalColors::warning("Warning: Food not found in database: " + foodId) << endl;
            continue;
        }
        
        float calories = food->getCaloriesPerServing() * servings;
        totalCalories += calories;
        
        cout << left << setw(20) << foodId 
                 << setw(10) << servings
                 << static_cast<int>(calories) << endl;
    }
    
    cout << string(50, '-') << endl;
    cout << left << setw(20) << "TOTAL" 
             << setw(10) << "" 
             << static_cast<int>(totalCalories) << endl;
    cout << endl;
}

/**
 * setDate Method
 * @param args Command arguments
 * Sets the current working date.
 */
void CLI::setDate(const vector<string>& args) {
    if (args.size() < 2) {
        throw invalid_argument("Usage: set-date <YYYY-MM-DD>");
    }
    
    string date = args[1];
    // Validate date format
    regex dateRegex("\\d{4}-\\d{2}-\\d{2}");
    if (!regex_match(date, dateRegex)) {
        throw invalid_argument("Date must be in format YYYY-MM-DD");
    }
    
    currentDate = date;
    logHistory.setCurrentDate(currentDate);
    cout << TerminalColors::success("Current date set to " + currentDate) << endl;
}

/**
 * undoCommand Method
 * @param args Command arguments (unused)
 * Undoes the last log operation.
 */
void CLI::undoCommand(const vector<string>& args) {
    (void)args; // Suppress unused parameter warning
    
    if (!logHistory.canUndo()) {
        cout << TerminalColors::warning("Nothing to undo.") << endl;
        return;
    }
    logHistory.undo();
    cout << TerminalColors::success("Last operation undone.") << endl;
}

/**
 * redoCommand Method
 * @param args Command arguments (unused)
 * Redoes the last undone log operation.
 */
void CLI::redoCommand(const vector<string>& args) {
    (void)args; // Suppress unused parameter warning
    
    if (!logHistory.canRedo()) {
        cout << TerminalColors::warning("Nothing to redo.") << endl;
        return;
    }
    logHistory.redo();
    cout << TerminalColors::success("Operation redone.") << endl;
}

/**
 * viewProfile Method
 * @param args Command arguments (unused)
 * Shows the user profile.
 */
void CLI::viewProfile(const vector<string>& args) {
    (void)args; // Suppress unused parameter warning
    
    User& user = userProfile.getUser();
    
    // Get user activity level, goal, and calorie method as integers
    int activityLevel = static_cast<int>(user.getActivityLevel());
    int goal = static_cast<int>(user.getGoal());
    int calorieMethod = static_cast<int>(user.getCalorieCalculationMethod());
    
    cout << TerminalColors::bold("\nUser Profile:\n");
    cout << left << setw(20) << "Name:" << user.getName() << endl;
    cout << left << setw(20) << "Age:" << user.getAge() << " years" << endl;
    cout << left << setw(20) << "Gender:" << User::genderToString(user.getGender()) << endl;
    cout << left << setw(20) << "Height:" << user.getHeight() << " cm" << endl;
    cout << left << setw(20) << "Weight:" << user.getWeight() << " kg" << endl;
    cout << left << setw(20) << "BMI:" << user.calculateBMI() << endl;
    
    // Display activity level with numeric option
    cout << left << setw(20) << "Activity Level:" << activityLevel << " - " 
              << User::activityLevelToString(user.getActivityLevel()) << endl;
    
    // Display goal with numeric option
    cout << left << setw(20) << "Goal:" << goal << " - " 
              << User::goalToString(user.getGoal()) << endl;
    
    // Display calorie method with numeric option
    cout << left << setw(20) << "Calorie Method:" << calorieMethod << " - " 
              << User::calorieMethodToString(user.getCalorieCalculationMethod()) << endl;
    
    cout << left << setw(20) << "BMR:" << static_cast<int>(user.calculateBMR()) << " calories" << endl;
    cout << left << setw(20) << "Daily Needs:" << static_cast<int>(user.calculateDailyCalorieNeeds()) << " calories" << endl;
    cout << left << setw(20) << "Target Calories:" << static_cast<int>(user.calculateTargetCalories()) << " calories" << endl;
    
    // Display available options
    cout << "\n" << TerminalColors::info("Available options:") << endl;
    cout << "Activity Levels: 1=Sedentary, 2=Lightly Active, 3=Moderately Active, 4=Active, 5=Very Active" << endl;
    cout << "Goals: 1=Lose Weight, 2=Maintain Weight, 3=Gain Weight" << endl;
    cout << "Calorie Methods: 1=Harris-Benedict, 2=Mifflin-St Jeor, 3=WHO Equation" << endl;
    cout << "\nTo update: profile <attribute> <value>\nExample: profile activity 3" << endl;
    cout << endl;
}

/**
 * updateProfile Method
 * @param args Command arguments
 * Updates a user profile attribute.
 */
void CLI::updateProfile(const vector<string>& args) {
    if (args.size() < 3) {
        throw invalid_argument("Usage: profile <attribute> <value>");
    }
    
    string attribute = args[1];
    string value = args[2];
    
    // Handle numeric values for activity, goal, and calorie_method
    if (attribute == "activity") {
        int level = stoi(value);
        if (level < 1 || level > 5) {
            throw invalid_argument("Activity level must be between 1 and 5");
        }
        // Convert to enum value (0-based) by subtracting 1
        User::ActivityLevel activityLevel = static_cast<User::ActivityLevel>(level - 1);
        userProfile.getUser().setActivityLevel(activityLevel);
        cout << TerminalColors::success("Updated activity level to " + to_string(level) 
                  + " (" + User::activityLevelToString(activityLevel) + ")") << endl;
        return;
    } 
    else if (attribute == "goal") {
        int goalNum = stoi(value);
        if (goalNum < 1 || goalNum > 3) {
            throw invalid_argument("Goal must be between 1 and 3");
        }
        // Convert to enum value (0-based) by subtracting 1
        User::Goal goal = static_cast<User::Goal>(goalNum - 1);
        userProfile.getUser().setGoal(goal);
        cout << TerminalColors::success("Updated goal to " + to_string(goalNum) 
                  + " (" + User::goalToString(goal) + ")") << endl;
        return;
    }
    else if (attribute == "calorie_method") {
        int methodNum = stoi(value);
        if (methodNum < 1 || methodNum > 3) {
            throw invalid_argument("Calorie method must be between 1 and 3");
        }
        // Convert to enum value (0-based) by subtracting 1
        User::CalorieCalculationMethod method = static_cast<User::CalorieCalculationMethod>(methodNum - 1);
        userProfile.getUser().setCalorieCalculationMethod(method);
        cout << TerminalColors::success("Updated calorie method to " + to_string(methodNum) 
                  + " (" + User::calorieMethodToString(method) + ")") << endl;
        return;
    }
    
    // Special case for multi-word values
    for (size_t i = 3; i < args.size(); i++) {
        value += " " + args[i];
    }
    
    // Handle other attributes with original method
    userProfile.setUserAttribute(attribute, value);
    cout << TerminalColors::success("Updated " + attribute + " to '" + value + "'") << endl;
}

/**
 * viewCalories Method
 * @param args Command arguments
 * Shows calorie intake and target for a specific date.
 */
void CLI::viewCalories(const vector<string>& args) {
    string date = currentDate;
    if (args.size() > 1) {
        date = args[1];
        // Validate date format
        regex dateRegex("\\d{4}-\\d{2}-\\d{2}");
        if (!regex_match(date, dateRegex)) {
            throw invalid_argument("Date must be in format YYYY-MM-DD");
        }
    }
    
    auto log = logHistory.getLog(date);
    auto foods = log->getFoods();
    
    float totalCalories = 0;
    for (const auto& [foodId, servings] : foods) {
        auto food = foodDb.getFood(foodId);
        if (food) {
            totalCalories += food->getCaloriesPerServing() * servings;
        }
    }
    
    float targetCalories = userProfile.calculateTargetCalories();
    float difference = totalCalories - targetCalories;
    
    cout << TerminalColors::bold("\nCalorie Summary for " + date + ":\n");
    cout << "Consumed Calories: " << static_cast<int>(totalCalories) << endl;
    cout << "Target Calories: " << static_cast<int>(targetCalories) << endl;
    
    string status;
    if (difference > 0) {
        status = TerminalColors::warning("Over target");
    } else if (difference < 0) {
        status = TerminalColors::success("Under target");
    } else {
        status = "On target";
    }
    
    cout << "Difference: " << static_cast<int>(difference) << " (" << status << ")" << endl;
    cout << endl;
}

/**
 * saveData Method
 * @param args Command arguments (unused)
 * Saves all data to files.
 */
void CLI::saveData(const vector<string>& args) {
    (void)args; // Suppress unused parameter warning
    
    try {
        // Save food database
        foodDb.saveToFiles();
        
        // Save user profile
        userProfile.saveUser();
        
        // Save logs
        ofstream file("data/logs.json");
        if (!file.is_open()) {
            throw runtime_error("Failed to open logs.json for writing");
        }
        
        json logsJson = logHistory.toJson();
        file << setw(4) << logsJson << endl;
        file.close();
        
        cout << TerminalColors::success("All data saved successfully.") << endl;
    } catch (const exception& e) {
        throw runtime_error("Error saving data: " + string(e.what()));
    }
}

/**
 * loadData Method
 * @param args Command arguments (unused)
 * Loads all data from files.
 */
void CLI::loadData(const vector<string>& args) {
    (void)args; // Suppress unused parameter warning
    
    try {
        // Load food database
        foodDb.loadFromFiles();
        
        // Load user profile
        userProfile.loadUser();
        
        // Load logs
        ifstream file("data/logs.json");
        if (file.is_open()) {
            json logsJson;
            file >> logsJson;
            file.close();
            
            logHistory.fromJson(logsJson);
        }
        
        cout << TerminalColors::success("All data loaded successfully.") << endl;
    } catch (const exception& e) {
        throw runtime_error("Error loading data: " + string(e.what()));
    }
}

/**
 * quitProgram Method
 * @param args Command arguments (unused)
 * Exits the program.
 */
void CLI::quitProgram(const vector<string>& args) {
    (void)args; // Suppress unused parameter warning
    
    if (confirmAction("Save before exiting?")) {
        try {
            saveData({});
        } catch (const exception& e) {
            cerr << TerminalColors::error("Error saving data: ") << e.what() << endl;
        }
    }
    
    cout << TerminalColors::bold("Goodbye!") << endl;
    exit(0);
}

/**
 * getCurrentDate Method
 * @return The current date
 */
string CLI::getCurrentDate() const {
    return currentDate;
}

/**
 * addBasicFood Method
 * @param args Command arguments
 * Adds a new basic food to the database.
 */
void CLI::addBasicFood(const vector<string>& args) {
    if (args.size() < 3) {
        throw invalid_argument("Usage: add-basic-food <calories> <keyword1> [keyword2] ...");
    }
    
    float calories;
    try {
        calories = stof(args[1]);
    } catch (const exception&) {
        throw invalid_argument("Calories must be a number");
    }
    
    vector<string> keywords(args.begin() + 2, args.end());
    
    // Use the version that autogenerates IDs
    string id = foodDb.addBasicFood(keywords, calories);
    cout << TerminalColors::success("Food '" + id + "' added successfully.") << endl;
}

/**
 * listFoods Method
 * @param args Command arguments
 * Lists all foods in the database.
 */
void CLI::listFoods(const vector<string>& args) {
    (void)args; // Suppress unused parameter warning
    
    auto foods = foodDb.getAllFoods();
    
    cout << TerminalColors::bold("\nAvailable Foods (" + to_string(foods.size()) + "):\n");
    cout << left << setw(20) << "ID" << setw(10) << "Calories" << setw(10) << "Type" << "Keywords" << endl;
    cout << string(70, '-') << endl;
    
    for (const auto& food : foods) {
        string type = food->isComposite() ? "Composite" : "Basic";
        string keywords;
        for (const auto& keyword : food->getKeywords()) {
            keywords += keyword + ", ";
        }
        if (!keywords.empty()) {
            keywords = keywords.substr(0, keywords.length() - 2);
        }
        
        cout << left << setw(20) << food->getId() 
                 << setw(10) << food->getCaloriesPerServing()
                 << setw(10) << type
                 << keywords << endl;
    }
    cout << endl;
}

/**
 * searchFoods Method
 * @param args Command arguments
 * Searches for foods by keywords.
 */
void CLI::searchFoods(const vector<string>& args) {
    if (args.size() < 2) {
        throw invalid_argument("Usage: search-foods <keyword1> [keyword2] ... [--all]");
    }
    
    bool matchAll = true;
    vector<string> keywords;
    
    for (size_t i = 1; i < args.size(); i++) {
        if (args[i] == "--all") {
            matchAll = true;
        } else if (args[i] == "--any") {
            matchAll = false;
        } else {
            keywords.push_back(args[i]);
        }
    }
    
    auto foods = foodDb.searchFoods(keywords, matchAll);
    
    string matchType = matchAll ? "all" : "any";
    cout << TerminalColors::bold("\nSearch Results - Foods matching " + matchType + " keywords (" + to_string(foods.size()) + "):\n");
    cout << left << setw(20) << "ID" << setw(10) << "Calories" << setw(10) << "Type" << "Keywords" << endl;
    cout << string(70, '-') << endl;
    
    for (const auto& food : foods) {
        string type = food->isComposite() ? "Composite" : "Basic";
        string keywordsList;
        for (const auto& keyword : food->getKeywords()) {
            keywordsList += keyword + ", ";
        }
        if (!keywordsList.empty()) {
            keywordsList = keywordsList.substr(0, keywordsList.length() - 2);
        }
        
        cout << left << setw(20) << food->getId() 
                 << setw(10) << food->getCaloriesPerServing()
                 << setw(10) << type
                 << keywordsList << endl;
    }
    cout << endl;
}

/**
 * createCompositeFood Method
 * @param args Command arguments
 * Creates a new composite food.
 */
void CLI::createCompositeFood(const vector<string>& args) {
    if (args.size() < 2) {
        throw invalid_argument("Usage: create-composite <keyword1> [keyword2] ... --components <food1> <servings1> [<food2> <servings2> ...]");
    }
    
    // Find the index of --components
    auto componentsIt = find(args.begin(), args.end(), "--components");
    if (componentsIt == args.end()) {
        throw invalid_argument("Missing --components flag");
    }
    
    size_t componentsIndex = componentsIt - args.begin();
    
    // Keywords are everything between the command name and --components
    vector<string> keywords(args.begin() + 1, args.begin() + componentsIndex);
    
    // Components are everything after --components, in pairs
    map<string, float> components;
    for (size_t i = componentsIndex + 1; i < args.size(); i += 2) {
        if (i + 1 >= args.size()) {
            throw invalid_argument("Missing serving count for component: " + args[i]);
        }
        
        string compId = args[i];
        float servings;
        try {
            servings = stof(args[i + 1]);
        } catch (const exception&) {
            throw invalid_argument("Serving count must be a number for component: " + compId);
        }
        
        components[compId] = servings;
    }
    
    // Use the version that autogenerates IDs
    string id = foodDb.createCompositeFood(keywords, components);
    cout << TerminalColors::success("Composite food '" + id + "' created successfully.") << endl;
}

/**
 * addFoodToLog Method
 * @param args Command arguments
 * Adds food to the log.
 */
void CLI::addFoodToLog(const vector<string>& args) {
    if (args.size() < 3) {
        throw invalid_argument("Usage: add-food <food_id> <servings>");
    }
    
    string foodId = args[1];
    float servings;
    try {
        servings = stof(args[2]);
    } catch (const exception&) {
        throw invalid_argument("Servings must be a number");
    }
    
    auto food = foodDb.getFood(foodId);
    if (!food) {
        throw invalid_argument("Food not found: " + foodId);
    }
    
    map<string, string> params = {
        {"food_id", foodId},
        {"servings", args[2]}
    };
    
    logHistory.executeCommand("add-food", params);
    
    float calories = food->getCaloriesPerServing() * servings;
    cout << TerminalColors::success("Added " + args[2] + " serving(s) of '" + foodId + "' (" 
                                     + to_string(static_cast<int>(calories)) + " calories) to the log.") << endl;
}

/**
 * removeFoodFromLog Method
 * @param args Command arguments
 * Removes food from the log.
 */
void CLI::removeFoodFromLog(const vector<string>& args) {
    if (args.size() < 2) {
        throw invalid_argument("Usage: remove-food <food_id>");
    }
    
    string foodId = args[1];
    
    auto log = logHistory.getCurrentLog();
    auto foods = log->getFoods();
    
    if (foods.find(foodId) == foods.end()) {
        throw invalid_argument("Food not in log: " + foodId);
    }
    
    map<string, string> params = {
        {"food_id", foodId}
    };
    
    logHistory.executeCommand("remove-food", params);
    
    cout << TerminalColors::success("Removed '" + foodId + "' from the log.") << endl;
}

/**
 * viewDailyHistory Method
 * @param args Command arguments
 * Shows the history of daily metrics.
 */
void CLI::viewDailyHistory(const vector<string>& args) {
    User& user = userProfile.getUser();
    const auto& metrics = user.getDailyMetrics();
    
    if (metrics.empty()) {
        cout << TerminalColors::warning("No metrics history available.") << endl;
        return;
    }
    
    int limit = -1; // Default: show all records
    
    // Parse arguments
    if (args.size() > 1) {
        if (args[1] == "all") {
            limit = -1; // Show all
        } else if (args[1] == "last" && args.size() > 2) {
            try {
                limit = stoi(args[2]);
                if (limit <= 0) {
                    throw invalid_argument("Number must be positive");
                }
            } catch (const exception&) {
                throw invalid_argument("Usage: history [all|last N] - N must be a positive number");
            }
        }
    }
    
    cout << TerminalColors::bold("\nDaily Metrics History:\n");
    cout << left 
         << setw(20) << "Date" 
         << setw(10) << "Weight" 
         << setw(10) << "Age" 
         << "Activity Level" << endl;
    cout << string(60, '-') << endl;
    
    int count = 0;
    // If limit is set, show only the last 'limit' entries
    int startIdx = limit > 0 ? max(0, static_cast<int>(metrics.size()) - limit) : 0;
    
    for (size_t i = startIdx; i < metrics.size(); i++) {
        const auto& metric = metrics[i];
        cout << left 
             << setw(20) << user.getFormattedDate(metric.timestamp)
             << setw(10) << metric.weight
             << setw(10) << metric.age
             << User::activityLevelToString(static_cast<User::ActivityLevel>(metric.activityLevel)) 
             << endl;
        count++;
    }
    
    cout << endl << TerminalColors::info("Showing " + to_string(count) + " of " + to_string(metrics.size()) + " records.") << endl;
}

/**
 * Clear Screen Method
 * Clears the terminal screen.
 */
void CLI::clearScreen(const vector<string>& args) {
    (void)args; // Suppress unused parameter warning
    
    // ANSI escape code to clear the screen
    cout << "\033[2J\033[1;1H";
    cout << TerminalColors::bold("Screen cleared.") << endl;
}