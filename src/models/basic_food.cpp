#include "basic_food.h"
#include <algorithm>
#include <sstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

// Default constructor - needed because I kept getting compiler errors without it
BasicFood::BasicFood() : caloriesPerServing(0.0) {
    // nothing to do here, just initializing calories to 0
}

// Constructor with all the fields because that's what good classes do apparently
BasicFood::BasicFood(const std::string& id, const std::vector<std::string>& keywords, double calories)
    : id(id), keywords(keywords), caloriesPerServing(calories) {
    // make sure calories aren't negative because that would be weird
    if (calories < 0) {
        std::cerr << "Warning: Negative calories for " << id << ". Setting to 0." << std::endl;
        caloriesPerServing = 0.0;
    }
}

// Standard getters - boring but necessary

std::string BasicFood::getId() const {
    return id;
}

std::vector<std::string> BasicFood::getKeywords() const {
    return keywords;
}

double BasicFood::getCaloriesPerServing() const {
    return caloriesPerServing;
}

// Standard setters - also boring

void BasicFood::setId(const std::string& id) {
    // trim whitespace because people are messy
    std::string trimmed = id;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);
    
    if (trimmed.empty()) {
        std::cerr << "Warning: Empty ID provided. Using 'unknown_food' instead." << std::endl;
        this->id = "unknown_food";
    } else {
        this->id = trimmed;
    }
}

void BasicFood::setKeywords(const std::vector<std::string>& keywords) {
    this->keywords = keywords;
}

void BasicFood::addKeyword(const std::string& keyword) {
    // don't add empty keywords, that's just dumb
    if (!keyword.empty()) {
        keywords.push_back(keyword);
    }
}

void BasicFood::setCaloriesPerServing(double calories) {
    // again, no negative calories allowed
    if (calories < 0) {
        std::cerr << "Warning: Attempted to set negative calories. Using 0 instead." << std::endl;
        this->caloriesPerServing = 0.0;
    } else {
        this->caloriesPerServing = calories;
    }
}

// Checks if this food matches a keyword, case insensitive
// this took way too long to get right
bool BasicFood::matchesKeyword(const std::string& keyword) const {
    // empty keywords match nothing, not even empty strings
    if (keyword.empty()) {
        return false;
    }
    
    return std::find_if(keywords.begin(), keywords.end(), 
        [&keyword](const std::string& k) {
            // case insensitive comparison
            return std::equal(keyword.begin(), keyword.end(), k.begin(), k.end(),
                [](char a, char b) { return std::tolower(a) == std::tolower(b); });
        }) != keywords.end();
}

// I think this means it matches ALL the keywords in the list
bool BasicFood::matchesAllKeywords(const std::vector<std::string>& searchKeywords) const {
    // if there are no search keywords, then it's a match (by default)
    // prof said this was the expected behavior
    if (searchKeywords.empty()) {
        return true;
    }
    
    return std::all_of(searchKeywords.begin(), searchKeywords.end(),
        [this](const std::string& keyword) { return matchesKeyword(keyword); });
}

// Returns true if the food matches ANY of the keywords
bool BasicFood::matchesAnyKeyword(const std::vector<std::string>& searchKeywords) const {
    // if there are no search keywords, then it doesn't match anything
    // prof wasn't clear on this but it seems logical
    if (searchKeywords.empty()) {
        return false;
    }
    
    return std::any_of(searchKeywords.begin(), searchKeywords.end(),
        [this](const std::string& keyword) { return matchesKeyword(keyword); });
}

// Serialize to JSON - I hate JSON but whatever
void BasicFood::toJson(std::ostream& os) const {
    json j;
    j["id"] = id;
    j["keywords"] = keywords;
    j["calories"] = caloriesPerServing;
    j["type"] = "basic";
    
    os << j.dump(4); // pretty print with 4-space indent
}

// Create a BasicFood from a JSON string
// this can throw exceptions but I'm too tired to handle them
std::shared_ptr<BasicFood> BasicFood::fromJson(const std::string& jsonString) {
    try {
        json j = json::parse(jsonString);
        
        auto food = std::make_shared<BasicFood>();
        
        // Check required fields
        if (!j.contains("id") || !j.contains("calories") || !j.contains("keywords")) {
            std::cerr << "Missing required fields in food JSON" << std::endl;
            return nullptr;
        }
        
        food->setId(j["id"]);
        food->setKeywords(j["keywords"].get<std::vector<std::string>>());
        food->setCaloriesPerServing(j["calories"]);
        
        return food;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing basic food from JSON: " << e.what() << std::endl;
        return nullptr;
    }
}
