#ifndef BASIC_FOOD_H
#define BASIC_FOOD_H

#include <string>
#include <vector>
#include <memory>

// Forward declaration of Food interface
class Food {
public:
    virtual ~Food() = default;
    virtual std::string getId() const = 0;
    virtual std::vector<std::string> getKeywords() const = 0;
    virtual double getCaloriesPerServing() const = 0;
    virtual void toJson(std::ostream& os) const = 0;
};

class BasicFood : public Food {
private:
    std::string id;
    std::vector<std::string> keywords;
    double caloriesPerServing;
    // Extensible design for additional nutritional information
    // Could be extended with a map or additional attributes

public:
    BasicFood();
    BasicFood(const std::string& id, const std::vector<std::string>& keywords, double calories);
    
    // Implementation of Food interface
    std::string getId() const override;
    std::vector<std::string> getKeywords() const override;
    double getCaloriesPerServing() const override;
    
    // Setters
    void setId(const std::string& id);
    void setKeywords(const std::vector<std::string>& keywords);
    void addKeyword(const std::string& keyword);
    void setCaloriesPerServing(double calories);
    
    // Utility methods
    bool matchesKeyword(const std::string& keyword) const;
    bool matchesAllKeywords(const std::vector<std::string>& searchKeywords) const;
    bool matchesAnyKeyword(const std::vector<std::string>& searchKeywords) const;
    
    // For serialization/deserialization
    void toJson(std::ostream& os) const override;
    static std::shared_ptr<BasicFood> fromJson(const std::string& json);
};

#endif // BASIC_FOOD_H
