#include "log_entry.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

/**
 * LogEntry Constructor
 * @param date The date for this log entry (YYYY-MM-DD format)
 */
LogEntry::LogEntry(const std::string& date) : date(date) {
    if (this->date.empty()) {
        // Use current date if none provided
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
        this->date = ss.str();
    }
}

/**
 * addFood Method
 * @param foodId The ID of the food to add
 * @param servings The number of servings
 */
void LogEntry::addFood(const std::string& foodId, float servings) {
    foods[foodId] += servings;
    if (foods[foodId] <= 0) {
        foods.erase(foodId);
    }
}

/**
 * removeFood Method
 * @param foodId The ID of the food to remove
 */
void LogEntry::removeFood(const std::string& foodId) {
    if (foods.find(foodId) != foods.end()) {
        foods.erase(foodId);
    }
}

/**
 * getFoods Method
 * @return A map of food IDs to servings
 */
std::map<std::string, float> LogEntry::getFoods() const {
    return foods;
}

/**
 * getDate Method
 * @return The date of this log entry
 */
std::string LogEntry::getDate() const {
    return date;
}

/**
 * setDate Method
 * @param date The new date for this log entry
 */
void LogEntry::setDate(const std::string& date) {
    this->date = date;
}

/**
 * toJson Method
 * @return A JSON representation of this log entry
 */
json LogEntry::toJson() const {
    json j;
    j["date"] = date;
    j["foods"] = foods;
    return j;
}

/**
 * fromJson Method
 * @param j The JSON to parse
 * @return A LogEntry object
 */
LogEntry LogEntry::fromJson(const json& j) {
    LogEntry entry;
    if (j.contains("date")) {
        entry.date = j["date"].get<std::string>();
    }
    if (j.contains("foods") && j["foods"].is_object()) {
        for (auto& [key, value] : j["foods"].items()) {
            entry.foods[key] = value.get<float>();
        }
    }
    return entry;
}

/**
 * LogHistory Constructor
 */
LogHistory::LogHistory() : currentCommandIndex(0) {
    currentDate = getCurrentDateString();
    logs[currentDate] = LogEntry(currentDate);
}

/**
 * getCurrentLog Method
 * @return Pointer to the current log entry
 */
LogEntry* LogHistory::getCurrentLog() {
    if (logs.find(currentDate) == logs.end()) {
        logs[currentDate] = LogEntry(currentDate);
    }
    return &logs[currentDate];
}

/**
 * getLog Method
 * @param date The date to get the log for
 * @return Pointer to the log entry for the specified date
 */
LogEntry* LogHistory::getLog(const std::string& date) {
    if (logs.find(date) == logs.end()) {
        logs[date] = LogEntry(date);
    }
    return &logs[date];
}

/**
 * setCurrentDate Method
 * @param date The new current date
 */
void LogHistory::setCurrentDate(const std::string& date) {
    currentDate = date;
    if (logs.find(currentDate) == logs.end()) {
        logs[currentDate] = LogEntry(currentDate);
    }
}

/**
 * getCurrentDate Method
 * @return The current date
 */
std::string LogHistory::getCurrentDate() const {
    return currentDate;
}

/**
 * getAvailableDates Method
 * @return A vector of dates that have log entries
 */
std::vector<std::string> LogHistory::getAvailableDates() const {
    std::vector<std::string> dates;
    for (const auto& [date, _] : logs) {
        dates.push_back(date);
    }
    return dates;
}

/**
 * executeCommand Method
 * @param command The command to execute
 * @param params The parameters for the command
 * Executes a command and adds it to the history
 */
void LogHistory::executeCommand(const std::string& command, const std::map<std::string, std::string>& params) {
    if (command == "add-food") {
        std::string foodId = params.at("food_id");
        float servings = std::stof(params.at("servings"));
        auto cmd = std::make_unique<AddFoodCommand>(getCurrentLog(), foodId, servings);
        addCommand(std::move(cmd));
    } else if (command == "remove-food") {
        std::string foodId = params.at("food_id");
        float servings = getCurrentLog()->getFoods().at(foodId);
        auto cmd = std::make_unique<RemoveFoodCommand>(getCurrentLog(), foodId, servings);
        addCommand(std::move(cmd));
    }
}

/**
 * canUndo Method
 * @return Whether undo is possible
 */
bool LogHistory::canUndo() const {
    return currentCommandIndex > 0;
}

/**
 * canRedo Method
 * @return Whether redo is possible
 */
bool LogHistory::canRedo() const {
    return currentCommandIndex < commandHistory.size();
}

/**
 * undo Method
 * Undoes the last command
 */
void LogHistory::undo() {
    if (canUndo()) {
        currentCommandIndex--;
        commandHistory[currentCommandIndex]->unexecute();
    }
}

/**
 * redo Method
 * Redoes the last undone command
 */
void LogHistory::redo() {
    if (canRedo()) {
        commandHistory[currentCommandIndex]->execute();
        currentCommandIndex++;
    }
}

/**
 * toJson Method
 * @return A JSON representation of the log history
 */
json LogHistory::toJson() const {
    json j = json::array();
    for (const auto& [date, log] : logs) {
        j.push_back(log.toJson());
    }
    return j;
}

/**
 * fromJson Method
 * @param j The JSON to parse
 * Loads log history from JSON
 */
void LogHistory::fromJson(const json& j) {
    logs.clear();
    if (j.is_array()) {
        for (const auto& logJson : j) {
            LogEntry log = LogEntry::fromJson(logJson);
            logs[log.getDate()] = log;
        }
    }
    // If there's no log for current date, create one
    std::string today = getCurrentDateString();
    if (logs.find(today) == logs.end()) {
        logs[today] = LogEntry(today);
    }
    currentDate = today;
}

/**
 * getCurrentDateString Method
 * @return The current date as a string (YYYY-MM-DD)
 */
std::string LogHistory::getCurrentDateString() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}

/**
 * addCommand Method
 * @param command The command to add to history
 */
void LogHistory::addCommand(std::unique_ptr<Command> command) {
    // If we're not at the end of history, remove everything after currentCommandIndex
    if (currentCommandIndex < commandHistory.size()) {
        commandHistory.resize(currentCommandIndex);
    }
    
    // Execute the command and add it to history
    command->execute();
    commandHistory.push_back(std::move(command));
    currentCommandIndex++;
}

/**
 * AddFoodCommand Constructor
 */
LogHistory::AddFoodCommand::AddFoodCommand(LogEntry* log, const std::string& foodId, float servings) 
    : log(log), foodId(foodId), servings(servings) {}

/**
 * execute Method for AddFoodCommand
 */
void LogHistory::AddFoodCommand::execute() {
    log->addFood(foodId, servings);
}

/**
 * unexecute Method for AddFoodCommand
 */
void LogHistory::AddFoodCommand::unexecute() {
    log->addFood(foodId, -servings);
}

/**
 * toString Method for AddFoodCommand
 */
std::string LogHistory::AddFoodCommand::toString() const {
    return "AddFood: " + foodId + ", " + std::to_string(servings) + " servings";
}

/**
 * RemoveFoodCommand Constructor
 */
LogHistory::RemoveFoodCommand::RemoveFoodCommand(LogEntry* log, const std::string& foodId, float servings) 
    : log(log), foodId(foodId), servings(servings) {}

/**
 * execute Method for RemoveFoodCommand
 */
void LogHistory::RemoveFoodCommand::execute() {
    log->removeFood(foodId);
}

/**
 * unexecute Method for RemoveFoodCommand
 */
void LogHistory::RemoveFoodCommand::unexecute() {
    log->addFood(foodId, servings);
}

/**
 * toString Method for RemoveFoodCommand
 */
std::string LogHistory::RemoveFoodCommand::toString() const {
    return "RemoveFood: " + foodId;
}
