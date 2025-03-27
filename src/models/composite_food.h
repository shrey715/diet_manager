#ifndef COMPOSITE_FOOD_H
#define COMPOSITE_FOOD_H

#include "basic_food.h"
#include <map>
#include <memory>

// CompositeFood inherits from Food interface defined in basic_food.h
class CompositeFood : public Food {
private:
    std::string id;
    std::vector<std::string> keywords;
    std::map<std::shared_ptr<Food>, double> components; // Food and servings

public:
    CompositeFood();
    CompositeFood(const std::string& id, const std::vector<std::string>& keywords);
    
    // Implementation of Food interface
    std::string getId() const override;
    std::vector<std::string> getKeywords() const override;
    double getCaloriesPerServing() const override;
    
    // Setters
    void setId(const std::string& id);
    void setKeywords(const std::vector<std::string>& keywords);
    void addKeyword(const std::string& keyword);
    
    // Component management
    void addComponent(std::shared_ptr<Food> food, double servings);
    void removeComponent(const std::string& foodId);
    std::map<std::shared_ptr<Food>, double> getComponents() const;
    
    // Search methods
    bool matchesKeyword(const std::string& keyword) const;
    bool matchesAllKeywords(const std::vector<std::string>& searchKeywords) const;
    bool matchesAnyKeyword(const std::vector<std::string>& searchKeywords) const;
    
    // Serialization
    void toJson(std::ostream& os) const override;
    static std::shared_ptr<CompositeFood> fromJson(const std::string& json, 
                                                  const std::map<std::string, std::shared_ptr<Food>>& foodDatabase);
};

#endif // COMPOSITE_FOOD_H
