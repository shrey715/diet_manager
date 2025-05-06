/**
 * @file food.h
 * @brief Food Model Definitions
 * 
 * This file defines the classes that represent food items in the diet manager application.
 * It implements a class hierarchy using the Composite design pattern to model both basic
 * food items and composite foods (recipes) that consist of multiple ingredients.
 * 
 * Key components:
 * - Abstract Food base class defining the common interface for all food items
 * - BasicFood class representing individual food items with direct calorie values
 * - CompositeFood class representing recipes composed of other food items
 * 
 * The Food hierarchy allows for uniform treatment of both simple and complex foods
 * throughout the application, simplifying calorie calculations and food management.
 */

#ifndef FOOD_H
#define FOOD_H

#include <string>
#include <vector>
#include <map>
using namespace std;

/**
 * Food Class
 * This class represents a food item with an id and associated keywords.
 */
class Food{
    public:
        Food(const std::string& id, const std::vector<std::string>& keywords);

        string getId() const;
        vector<string> getKeywords() const;
        virtual float getCaloriesPerServing() const = 0;
        virtual bool isComposite() const = 0;

    protected:
        string id;
        vector<string> keywords;
};

/**
 * BasicFood Class - inherits from Food
 * This class represents a basic food item with an id, associated keywords, and calories per serving.
 */
class BasicFood:public Food{
    public:
        BasicFood(const std::string& id, const std::vector<std::string>& keywords, float calories);

        float getCaloriesPerServing() const override;
        bool isComposite() const override;

    private:
        float calories;
};

/**
 * CompositeFood Class - inherits from Food
 * This class represents a composite food item with an id, associated keywords, and components.
 */
class CompositeFood:public Food{
    public:
        CompositeFood(const std::string& id, const std::vector<std::string>& keywords);

        void addComponent(const std::string& foodId, float servings);
        map<string, float> getComponents() const;
        void setTotalCalories(float calories);
        float getCaloriesPerServing() const override;
        bool isComposite() const override;

    private:
        map<string, float> components;
        float totalCalories;
};

#endif // FOOD_H
