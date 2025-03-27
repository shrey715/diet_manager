#include "food_database.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Implementation of WebsiteAImporter
std::vector<std::shared_ptr<Food>> WebsiteAImporter::importFoods() {
    // This would implement specific parsing logic for Website A's format
    // For example, it might use a REST API client or parse HTML/XML
    std::vector<std::shared_ptr<Food>> importedFoods;
    
    // Mock implementation for demonstration
    auto mockFood = std::make_shared<BasicFood>("imported_food_1", 
                                               std::vector<std::string>{"imported", "website_a"}, 
                                               250.0);
    importedFoods.push_back(mockFood);
    
    return importedFoods;
}

FoodDatabase::FoodDatabase(const std::string& basicFoodPath, const std::string& compositeFoodPath)
    : basicFoodFilePath(basicFoodPath), compositeFoodFilePath(compositeFoodPath) {
}

bool FoodDatabase::addFood(std::shared_ptr<Food> food) {
    if (!food) return false;
    
    const std::string& id = food->getId();
    
    // Check if food already exists
    if (foods.find(id) != foods.end()) {
        return false;
    }
    
    foods[id] = food;
    return true;
}

bool FoodDatabase::removeFood(const std::string& id) {
    return foods.erase(id) > 0;
}

std::shared_ptr<Food> FoodDatabase::getFood(const std::string& id) const {
    auto it = foods.find(id);
    if (it != foods.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Food>> FoodDatabase::getAllFoods() const {
    std::vector<std::shared_ptr<Food>> allFoods;
    for (const auto& pair : foods) {
        allFoods.push_back(pair.second);
    }
    return allFoods;
}

std::vector<std::shared_ptr<Food>> FoodDatabase::searchByKeyword(const std::string& keyword) const {
    std::vector<std::shared_ptr<Food>> results;
    
    for (const auto& pair : foods) {
        if (pair.second->getKeywords().empty()) {
            continue;
        }
        
        // Use the food's matchesKeyword method (depending on whether it's BasicFood or CompositeFood)
        auto basicFood = std::dynamic_pointer_cast<BasicFood>(pair.second);
        if (basicFood && basicFood->matchesKeyword(keyword)) {
            results.push_back(pair.second);
            continue;
        }
        
        auto compositeFood = std::dynamic_pointer_cast<CompositeFood>(pair.second);
        if (compositeFood && compositeFood->matchesKeyword(keyword)) {
            results.push_back(pair.second);
        }
    }
    
    return results;
}

std::vector<std::shared_ptr<Food>> FoodDatabase::searchByAllKeywords(const std::vector<std::string>& keywords) const {
    if (keywords.empty()) {
        return getAllFoods();
    }
    
    std::vector<std::shared_ptr<Food>> results;
    
    for (const auto& pair : foods) {
        auto basicFood = std::dynamic_pointer_cast<BasicFood>(pair.second);
        if (basicFood && basicFood->matchesAllKeywords(keywords)) {
            results.push_back(pair.second);
            continue;
        }
        
        auto compositeFood = std::dynamic_pointer_cast<CompositeFood>(pair.second);
        if (compositeFood && compositeFood->matchesAllKeywords(keywords)) {
            results.push_back(pair.second);
        }
    }
    
    return results;
}

std::vector<std::shared_ptr<Food>> FoodDatabase::searchByAnyKeyword(const std::vector<std::string>& keywords) const {
    if (keywords.empty()) {
        return getAllFoods();
    }
    
    std::vector<std::shared_ptr<Food>> results;
    
    for (const auto& pair : foods) {
        auto basicFood = std::dynamic_pointer_cast<BasicFood>(pair.second);
        if (basicFood && basicFood->matchesAnyKeyword(keywords)) {
            results.push_back(pair.second);
            continue;
        }
        
        auto compositeFood = std::dynamic_pointer_cast<CompositeFood>(pair.second);
        if (compositeFood && compositeFood->matchesAnyKeyword(keywords)) {
            results.push_back(pair.second);
        }
    }
    
    return results;
}

void FoodDatabase::saveDatabase() const {
    saveBasicFoods();
    saveCompositeFoods();
}

void FoodDatabase::loadDatabase() {
    foods.clear();
    loadBasicFoods();
    loadCompositeFoods();
}

void FoodDatabase::importFoods(FoodImporter& importer) {
    auto importedFoods = importer.importFoods();
    for (auto& food : importedFoods) {
        addFood(food);
    }
}

const std::map<std::string, std::shared_ptr<Food>>& FoodDatabase::getFoodMap() const {
    return foods;
}

void FoodDatabase::saveBasicFoods() const {
    std::ofstream file(basicFoodFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << basicFoodFilePath << std::endl;
        return;
    }
    
    json basicFoodsJson = json::array();
    
    for (const auto& pair : foods) {
        auto basicFood = std::dynamic_pointer_cast<BasicFood>(pair.second);
        if (basicFood) {
            std::stringstream ss;
            basicFood->toJson(ss);
            basicFoodsJson.push_back(json::parse(ss.str()));
        }
    }
    
    file << basicFoodsJson.dump(4);
    file.close();
}

void FoodDatabase::saveCompositeFoods() const {
    std::ofstream file(compositeFoodFilePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << compositeFoodFilePath << std::endl;
        return;
    }
    
    json compositeFoodsJson = json::array();
    
    for (const auto& pair : foods) {
        auto compositeFood = std::dynamic_pointer_cast<CompositeFood>(pair.second);
        if (compositeFood) {
            std::stringstream ss;
            compositeFood->toJson(ss);
            compositeFoodsJson.push_back(json::parse(ss.str()));
        }
    }
    
    file << compositeFoodsJson.dump(4);
    file.close();
}

void FoodDatabase::loadBasicFoods() {
    std::ifstream file(basicFoodFilePath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open file for reading: " << basicFoodFilePath << std::endl;
        return;
    }
    
    try {
        std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        if (jsonContent.empty()) {
            std::cerr << "Warning: Basic food file is empty" << std::endl;
            return;
        }
        
        json basicFoodsJson = json::parse(jsonContent);
        
        for (const auto& foodJson : basicFoodsJson) {
            try {
                auto food = BasicFood::fromJson(foodJson.dump());
                if (food && !food->getId().empty()) {
                    foods[food->getId()] = food;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing food item: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing basic foods: " << e.what() << std::endl;
    }
}

void FoodDatabase::loadCompositeFoods() {
    std::ifstream file(compositeFoodFilePath);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open file for reading: " << compositeFoodFilePath << std::endl;
        return;
    }
    
    try {
        std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        if (jsonContent.empty()) {
            std::cerr << "Warning: Composite food file is empty" << std::endl;
            return;
        }
        
        json compositeFoodsJson = json::parse(jsonContent);
        
        for (const auto& foodJson : compositeFoodsJson) {
            try {
                auto food = CompositeFood::fromJson(foodJson.dump(), foods);
                if (food && !food->getId().empty()) {
                    foods[food->getId()] = food;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing composite food: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing composite foods: " << e.what() << std::endl;
    }
}
