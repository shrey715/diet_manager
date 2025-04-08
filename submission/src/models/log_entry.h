#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

/**
 * LogEntry Class
 * This class represents a single log entry for food consumption on a specific date.
 */
class LogEntry {
public:
    // Constructor
    LogEntry(const string& date = "");
    
    // Methods
    void addFood(const string& foodId, float servings);
    void removeFood(const string& foodId);
    map<string, float> getFoods() const;
    string getDate() const;
    void setDate(const string& date);
    
    // Serialization
    json toJson() const;
    static LogEntry fromJson(const json& j);

private:
    string date;
    map<string, float> foods; // foodId -> servings
};

/**
 * LogHistory Class
 * This class manages the history of log entries and provides undo/redo functionality.
 */
class LogHistory {
public:
    // Constructor
    LogHistory();
    
    // Log entry methods
    LogEntry* getCurrentLog();
    LogEntry* getLog(const string& date);
    void setCurrentDate(const string& date);
    string getCurrentDate() const;
    vector<string> getAvailableDates() const;
    
    // Command methods
    void executeCommand(const string& command, const map<string, string>& params);
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();
    
    // Serialization
    json toJson() const;
    void fromJson(const json& j);

private:
    map<string, LogEntry> logs;
    string currentDate;
    
    // Command history for undo/redo
    class Command {
    public:
        virtual ~Command() = default;
        virtual void execute() = 0;
        virtual void unexecute() = 0;
        virtual string toString() const = 0;
    };
    
    class AddFoodCommand : public Command {
    public:
        AddFoodCommand(LogEntry* log, const string& foodId, float servings);
        void execute() override;
        void unexecute() override;
        string toString() const override;
    private:
        LogEntry* log;
        string foodId;
        float servings;
    };
    
    class RemoveFoodCommand : public Command {
    public:
        RemoveFoodCommand(LogEntry* log, const string& foodId, float servings);
        void execute() override;
        void unexecute() override;
        string toString() const override;
    private:
        LogEntry* log;
        string foodId;
        float servings;
    };
    
    vector<unique_ptr<Command>> commandHistory;
    size_t currentCommandIndex;
    
    string getCurrentDateString() const;
    void addCommand(unique_ptr<Command> command);
};

#endif // LOG_ENTRY_H
