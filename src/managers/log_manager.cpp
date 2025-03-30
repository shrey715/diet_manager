#include "log_manager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Command implementations
AddFoodCommand::AddFoodCommand(LogManager* logManager, const std::string& foodId, 
                             double servings, const std::chrono::system_clock::time_point& date)
    : logManager(logManager), foodId(foodId), servings(servings), date(date), executed(false) {}

void AddFoodCommand::execute() {
    executed = logManager->addFoodToLog(foodId, servings, date);
}

void AddFoodCommand::undo() {
    if (executed) {
        logManager->removeFoodFromLog(foodId, date);
        executed = false;
    }
}

RemoveFoodCommand::RemoveFoodCommand(LogManager* logManager, const std::string& foodId, 
                                   const std::chrono::system_clock::time_point& date)
    : logManager(logManager), foodId(foodId), date(date), executed(false), servings(1.0) {
    
    // Save servings for potential undo
    auto logEntry = logManager->getLogEntry(date);
    if (logEntry) {
        for (const auto& entry : logEntry->getConsumedFoods()) {
            if (std::get<0>(entry)->getId() == foodId) {
                servings = std::get<1>(entry);
                break;
            }
        }
    }
}

void RemoveFoodCommand::execute() {
    executed = logManager->removeFoodFromLog(foodId, date);
}

void RemoveFoodCommand::undo() {
    if (executed) {
        logManager->addFoodToLog(foodId, servings, date);
        executed = false;
    }
}

// LogManager implementation
LogManager::LogManager(const std::string& logFilePath, 
                     std::shared_ptr<User> user,
                     std::shared_ptr<FoodDatabase> foodDatabase)
    : logFilePath(logFilePath), user(user), foodDatabase(foodDatabase), 
      currentDate(std::chrono::system_clock::now()) {}

std::shared_ptr<LogEntry> LogManager::getLogEntry(const std::chrono::system_clock::time_point& date) {
    std::string dateStr = dateToString(date);
    
    // Create entry if it doesn't exist
    if (logEntries.find(dateStr) == logEntries.end()) {
        auto newEntry = std::make_shared<LogEntry>(LogEntry::generateId(), date);
        logEntries[dateStr] = newEntry;
    }
    
    return logEntries[dateStr];
}

bool LogManager::addFoodToLog(const std::string& foodId, double servings, 
                            const std::chrono::system_clock::time_point& date) {
    auto food = foodDatabase->getFood(foodId);
    if (!food || servings <= 0) {
        return false;
    }
    
    auto logEntry = getLogEntry(date);
    logEntry->addFood(food, servings, MealType::OTHER);
    return true;
}

bool LogManager::removeFoodFromLog(const std::string& foodId, 
                                 const std::chrono::system_clock::time_point& date) {
    auto logEntry = getLogEntry(date);
    
    // Check if food exists before removal
    bool foodExists = false;
    for (const auto& entry : logEntry->getConsumedFoods()) {
        if (std::get<0>(entry)->getId() == foodId) {
            foodExists = true;
            break;
        }
    }
    
    if (!foodExists) return false;
    
    logEntry->removeFood(foodId);
    return true;
}

// Date operations
void LogManager::setCurrentDate(const std::chrono::system_clock::time_point& date) {
    currentDate = date;
}

std::chrono::system_clock::time_point LogManager::getCurrentDate() const {
    return currentDate;
}

// Command execution with undo/redo support
void LogManager::executeCommand(std::shared_ptr<Command> command) {
    command->execute();
    commandHistory.push(command);
    
    // Clear redo stack when new command is executed
    while (!redoStack.empty()) {
        redoStack.pop();
    }
}

bool LogManager::canUndo() const { return !commandHistory.empty(); }
bool LogManager::canRedo() const { return !redoStack.empty(); }

void LogManager::undo() {
    if (canUndo()) {
        auto command = commandHistory.top();
        commandHistory.pop();
        command->undo();
        redoStack.push(command);
    }
}

void LogManager::redo() {
    if (canRedo()) {
        auto command = redoStack.top();
        redoStack.pop();
        command->execute();
        commandHistory.push(command);
    }
}

void LogManager::clearHistory() {
    while (!commandHistory.empty()) commandHistory.pop();
    while (!redoStack.empty()) redoStack.pop();
}

// Calorie calculations
double LogManager::getConsumedCalories(const std::chrono::system_clock::time_point& date) const {
    std::string dateStr = dateToString(date);
    auto it = logEntries.find(dateStr);
    return (it != logEntries.end()) ? it->second->getTotalCalories() : 0.0;
}

double LogManager::getTargetCalories() const {
    return user ? user->calculateTargetCalories() : 0.0;
}

double LogManager::getCalorieDifference(const std::chrono::system_clock::time_point& date) const {
    return getConsumedCalories(date) - getTargetCalories();
}

// Save and load operations
void LogManager::saveLogs() const {
    saveLogEntries();
}

void LogManager::loadLogs() {
    loadLogEntries();
}

void LogManager::setUser(std::shared_ptr<User> user) {
    this->user = user;
}

std::shared_ptr<User> LogManager::getUser() const {
    return user;
}

std::vector<std::shared_ptr<LogEntry>> LogManager::getAllLogEntries() const {
    std::vector<std::shared_ptr<LogEntry>> entries;
    entries.reserve(logEntries.size());
    for (const auto& pair : logEntries) {
        entries.push_back(pair.second);
    }
    return entries;
}

// Private helper methods
void LogManager::saveLogEntries() const {
    std::ofstream file(logFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not save logs to " << logFilePath << std::endl;
        return;
    }
    
    json logsJson = json::array();
    
    for (const auto& pair : logEntries) {
        std::stringstream ss;
        pair.second->toJson(ss);
        logsJson.push_back(json::parse(ss.str()));
    }
    
    file << logsJson.dump(4);
    file.close();
}

void LogManager::loadLogEntries() {
    logEntries.clear();
    
    std::ifstream file(logFilePath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not load logs from " << logFilePath << std::endl;
        return;
    }
    
    try {
        std::string jsonContent((std::istreambuf_iterator<char>(file)), 
                               std::istreambuf_iterator<char>());
        file.close();
        
        if (jsonContent.empty()) return;

        json logsJson = json::parse(jsonContent);
        
        for (const auto& logJson : logsJson) {
            auto logEntry = LogEntry::fromJson(logJson.dump(), foodDatabase->getFoodMap());
            if (logEntry) {
                std::string dateStr = dateToString(logEntry->getDate());
                logEntries[dateStr] = logEntry;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing logs: " << e.what() << std::endl;
    }
}

std::string LogManager::dateToString(const std::chrono::system_clock::time_point& date) const {
    return LogEntry::dateToString(date);
}

std::chrono::system_clock::time_point LogManager::stringToDate(const std::string& dateStr) const {
    return LogEntry::stringToDate(dateStr);
}
