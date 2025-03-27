#ifndef FOOD_DATABASE_H
#define FOOD_DATABASE_H

#include "../models/basic_food.h"
#include "../models/composite_food.h"
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <fstream>

// Abstract food importer for extensibility with different data sources
class FoodImporter {
public:
    virtual ~FoodImporter() = default;
    virtual std::vector<std::shared_ptr<Food>> importFoods() = 0;
};

// Example concrete importer for a specific website or format
class WebsiteAImporter : public FoodImporter {
public:
    std::vector<std::shared_ptr<Food>> importFoods() override;
};

class FoodDatabase {
private:
    std::map<std::string, std::shared_ptr<Food>> foods;
    std::string basicFoodFilePath;
    std::string compositeFoodFilePath;
    
    // Helper methods
    void saveBasicFoods() const;
    void saveCompositeFoods() const;
    void loadBasicFoods();
    void loadCompositeFoods();
    
public:
    FoodDatabase(const std::string& basicFoodPath, const std::string& compositeFoodPath);
    
    // Database operations
    bool addFood(std::shared_ptr<Food> food);
    bool removeFood(const std::string& id);
    std::shared_ptr<Food> getFood(const std::string& id) const;
    std::vector<std::shared_ptr<Food>> getAllFoods() const;
    
    // Search functionality
    std::vector<std::shared_ptr<Food>> searchByKeyword(const std::string& keyword) const;
    std::vector<std::shared_ptr<Food>> searchByAllKeywords(const std::vector<std::string>& keywords) const;
    std::vector<std::shared_ptr<Food>> searchByAnyKeyword(const std::vector<std::string>& keywords) const;
    
    // Save and load operations
    void saveDatabase() const;
    void loadDatabase();
    
    // Import foods from external sources
    void importFoods(FoodImporter& importer);
    
    // Utility
    const std::map<std::string, std::shared_ptr<Food>>& getFoodMap() const;
};

#endif // FOOD_DATABASE_H
