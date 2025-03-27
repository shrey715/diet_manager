#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include "../models/log_entry.h"
#include "../models/user.h"
#include "food_database.h"
#include <map>
#include <string>
#include <memory>
#include <chrono>
#include <vector>
#include <stack>

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class LogManager {
private:
    std::map<std::string, std::shared_ptr<LogEntry>> logEntries; // Date string -> LogEntry
    std::string logFilePath;
    std::shared_ptr<User> user;
    std::shared_ptr<FoodDatabase> foodDatabase;
    std::stack<std::shared_ptr<Command>> commandHistory;
    std::stack<std::shared_ptr<Command>> redoStack;
    std::chrono::system_clock::time_point currentDate;
    
    // Helper methods
    void saveLogEntries() const;
    void loadLogEntries();
    std::string dateToString(const std::chrono::system_clock::time_point& date) const;
    std::chrono::system_clock::time_point stringToDate(const std::string& dateStr) const;
    
public:
    LogManager(const std::string& logFilePath, 
               std::shared_ptr<User> user,
               std::shared_ptr<FoodDatabase> foodDatabase);
    
    // Log operations
    std::shared_ptr<LogEntry> getLogEntry(const std::chrono::system_clock::time_point& date);
    bool addFoodToLog(const std::string& foodId, double servings, 
                      const std::chrono::system_clock::time_point& date);
    bool removeFoodFromLog(const std::string& foodId, 
                          const std::chrono::system_clock::time_point& date);
    
    // Date operations
    void setCurrentDate(const std::chrono::system_clock::time_point& date);
    std::chrono::system_clock::time_point getCurrentDate() const;
    
    // Command pattern for undo/redo
    void executeCommand(std::shared_ptr<Command> command);
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();
    void clearHistory();
    
    // Calorie calculations
    double getConsumedCalories(const std::chrono::system_clock::time_point& date) const;
    double getTargetCalories() const;
    double getCalorieDifference(const std::chrono::system_clock::time_point& date) const;
    
    // Save and load operations
    void saveLogs() const;
    void loadLogs();
    
    // User profile management
    void setUser(std::shared_ptr<User> user);
    std::shared_ptr<User> getUser() const;
    
    // Get all logs for reporting
    std::vector<std::shared_ptr<LogEntry>> getAllLogEntries() const;
};

// Concrete command classes
class AddFoodCommand : public Command {
private:
    LogManager* logManager;
    std::string foodId;
    double servings;
    std::chrono::system_clock::time_point date;
    bool executed;
    
public:
    AddFoodCommand(LogManager* logManager, const std::string& foodId, 
                  double servings, const std::chrono::system_clock::time_point& date);
    void execute() override;
    void undo() override;
};

class RemoveFoodCommand : public Command {
private:
    LogManager* logManager;
    std::string foodId;
    double servings;
    std::chrono::system_clock::time_point date;
    bool executed;
    
public:
    RemoveFoodCommand(LogManager* logManager, const std::string& foodId, 
                     const std::chrono::system_clock::time_point& date);
    void execute() override;
    void undo() override;
};

#endif // LOG_MANAGER_H
