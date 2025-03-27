#include "composite_food.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Default constructor - nothing exciting here
CompositeFood::CompositeFood() {
    // no need to initialize anything special
}

// Constructor with id and keywords
CompositeFood::CompositeFood(const std::string& id, const std::vector<std::string>& keywords)
    : id(id), keywords(keywords) {
    
    // Check for empty ID
    if (id.empty()) {
        std::cerr << "Warning: Empty ID provided for composite food. Using 'unknown_composite'." << std::endl;
        this->id = "unknown_composite";
    }
    
    // Check for empty keywords
    if (keywords.empty()) {
        std::cerr << "Warning: No keywords provided for composite food '" << id << "'." << std::endl;
    }
}

// Getters - standard boilerplate stuff
std::string CompositeFood::getId() const {
    return id;
}

std::vector<std::string> CompositeFood::getKeywords() const {
    return keywords;
}

// Calculate total calories by summing all components
double CompositeFood::getCaloriesPerServing() const {
    double totalCalories = 0.0;
    
    // Sum up calories from all components
    for (const auto& component : components) {
        if (!component.first) {
            std::cerr << "Warning: Null food component in composite food '" << id << "'" << std::endl;
            continue;
        }
        
        double componentCalories = component.first->getCaloriesPerServing() * component.second;
        totalCalories += componentCalories;
    }
    
    return totalCalories;
}

// Setters - nothing fancy
void CompositeFood::setId(const std::string& id) {
    // Trim whitespace from ID
    std::string trimmed = id;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r\f\v"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r\f\v") + 1);
    
    if (trimmed.empty()) {
        std::cerr << "Warning: Empty ID provided. Using 'unknown_composite' instead." << std::endl;
        this->id = "unknown_composite";
    } else {
        this->id = trimmed;
    }
}

void CompositeFood::setKeywords(const std::vector<std::string>& keywords) {
    this->keywords = keywords;
}

void CompositeFood::addKeyword(const std::string& keyword) {
    // Don't add empty keywords
    if (!keyword.empty()) {
        keywords.push_back(keyword);
    }
}

// Add a component to this composite food
void CompositeFood::addComponent(std::shared_ptr<Food> food, double servings) {
    // Validate inputs
    if (!food) {
        std::cerr << "Warning: Attempted to add null food to composite '" << id << "'" << std::endl;
        return;
    }
    
    if (servings <= 0) {
        std::cerr << "Warning: Invalid servings (" << servings << ") for food '" 
                 << food->getId() << "' in composite '" << id << "'. Using 1.0 instead." << std::endl;
        servings = 1.0;
    }
    
    // Check for circular reference (can't include yourself as a component)
    if (food->getId() == id) {
        std::cerr << "Error: Circular reference detected. A composite food cannot contain itself." << std::endl;
        return;
    }
    
    // Add or update component
    components[food] = servings;
}

// Remove a component by its ID
void CompositeFood::removeComponent(const std::string& foodId) {
    // Loop through components and remove any matching the ID
    for (auto it = components.begin(); it != components.end(); ) {
        if (it->first->getId() == foodId) {
            it = components.erase(it);
        } else {
            ++it;
        }
    }
}

// Get all the components of this composite food
std::map<std::shared_ptr<Food>, double> CompositeFood::getComponents() const {
    return components;
}

// Check if this food matches a keyword (case insensitive)
bool CompositeFood::matchesKeyword(const std::string& keyword) const {
    // Empty keyword matches nothing
    if (keyword.empty()) {
        return false;
    }
    
    // Look for the keyword in our keywords list
    return std::find_if(keywords.begin(), keywords.end(), 
        [&keyword](const std::string& k) {
            return std::equal(keyword.begin(), keyword.end(), k.begin(), k.end(),
                [](char a, char b) { return std::tolower(a) == std::tolower(b); });
        }) != keywords.end();
}

// Check if this food matches ALL the given keywords
bool CompositeFood::matchesAllKeywords(const std::vector<std::string>& searchKeywords) const {
    // Empty search means match everything (weird but ok)
    if (searchKeywords.empty()) {
        return true;
    }
    
    // Must match all keywords to return true
    return std::all_of(searchKeywords.begin(), searchKeywords.end(),
        [this](const std::string& keyword) { return matchesKeyword(keyword); });
}

// Check if this food matches ANY of the given keywords
bool CompositeFood::matchesAnyKeyword(const std::vector<std::string>& searchKeywords) const {
    // Empty search means match nothing (makes sense I guess)
    if (searchKeywords.empty()) {
        return false;
    }
    
    // Match any keyword to return true
    return std::any_of(searchKeywords.begin(), searchKeywords.end(),
        [this](const std::string& keyword) { return matchesKeyword(keyword); });
}

// Convert to JSON for saving to file
void CompositeFood::toJson(std::ostream& os) const {
    json j;
    j["id"] = id;
    j["keywords"] = keywords;
    j["type"] = "composite";
    
    // Convert components to JSON array
    json componentsJson = json::array();
    for (const auto& component : components) {
        if (!component.first) {
            std::cerr << "Warning: Skipping null component in composite food '" << id << "'" << std::endl;
            continue;
        }
        
        json componentJson;
        componentJson["food_id"] = component.first->getId();
        componentJson["servings"] = component.second;
        componentsJson.push_back(componentJson);
    }
    j["components"] = componentsJson;
    
    // Write pretty-printed JSON to output stream
    os << j.dump(4);
}

// Create a CompositeFood from JSON string
std::shared_ptr<CompositeFood> CompositeFood::fromJson(
    const std::string& jsonString, 
    const std::map<std::string, std::shared_ptr<Food>>& foodDatabase) {
    
    try {
        json j = json::parse(jsonString);
        
        // Create new composite food
        auto food = std::make_shared<CompositeFood>();
        
        // Check required fields
        if (!j.contains("id") || !j.contains("keywords") || !j.contains("components")) {
            std::cerr << "Error: Missing required fields in composite food JSON" << std::endl;
            return nullptr;
        }
        
        // Set basic properties
        food->setId(j["id"]);
        food->setKeywords(j["keywords"].get<std::vector<std::string>>());
        
        // Add components
        int componentsAdded = 0;
        int componentsFailed = 0;
        
        for (const auto& componentJson : j["components"]) {
            if (!componentJson.contains("food_id") || !componentJson.contains("servings")) {
                std::cerr << "Warning: Component missing food_id or servings in " << j["id"].get<std::string>() << std::endl;
                componentsFailed++;
                continue;
            }
            
            std::string foodId = componentJson["food_id"];
            double servings = componentJson["servings"];
            
            // Look up component in food database
            if (foodDatabase.find(foodId) != foodDatabase.end()) {
                food->addComponent(foodDatabase.at(foodId), servings);
                componentsAdded++;
            } else {
                std::cerr << "Warning: Component food '" << foodId << "' not found in database for " 
                         << j["id"].get<std::string>() << std::endl;
                componentsFailed++;
            }
        }
        
        // Warn if all components failed
        if (componentsAdded == 0 && componentsFailed > 0) {
            std::cerr << "Warning: No components could be added to composite food " 
                     << j["id"].get<std::string>() << std::endl;
        }
        
        return food;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing composite food from JSON: " << e.what() << std::endl;
        return nullptr;
    }
}
