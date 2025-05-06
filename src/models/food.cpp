/**
 * @file food.cpp
 * @brief Food Model Implementation
 * 
 * This file implements the Food class hierarchy defined in food.h, providing
 * the concrete functionality for basic and composite food items. It includes
 * methods for accessing food properties, calculating calories, and managing
 * composite food components.
 * 
 * Key implementations:
 * - Constructors for Food, BasicFood, and CompositeFood classes
 * - Calorie calculation for both basic and composite foods
 * - Component management for composite foods (recipes)
 * - Food type identification and property access
 * 
 * The implementation supports the Composite design pattern, allowing for
 * uniform treatment of both basic ingredients and complex recipes.
 */

#include "food.h"

/**
 * Food Class Constructor
 * @param id The unique identifier for the food item
 * @param keywords A vector of keywords associated with the food item
 * This constructor initializes the Food object with a unique id and a list of keywords using initializer list syntax.
 */
Food::Food(const std::string& id, const std::vector<std::string>& keywords)
    : id(id), keywords(keywords) { 
}

/**
 * getId Method
 * @return The unique identifier for the food item
 * This method returns the unique identifier of the food item.
 */
string Food::getId() const {
    return id;
}

/**
 * getKeywords Method
 * @return A vector of keywords associated with the food item
 * This method returns the keywords associated with the food item.
 */
vector<string> Food::getKeywords() const {
    return keywords;
}

/**
 * BasicFood Class Constructor
 * @param id The unique identifier for the food item
 * @param keywords A vector of keywords associated with the food item
 * @param calories The number of calories per serving of the food item
 * This constructor initializes the BasicFood object with a unique id, a list of keywords, and the number of calories.
 */
BasicFood::BasicFood(const std::string& id, const std::vector<std::string>& keywords, float calories)
    : Food(id, keywords), calories(calories) { 
}

/**
 * getCaloriesPerServing Method
 * @return The number of calories per serving of the food item
 * This method returns the number of calories per serving of the food item.
 */
float BasicFood::getCaloriesPerServing() const {
    return calories;
}

/**
 * isComposite Method
 * @return A boolean indicating whether the food item is a composite food item
 * This method returns false, indicating that BasicFood is not a composite food item.
 */
bool BasicFood::isComposite() const {
    return false;
}

/**
 * CompositeFood Class Constructor
 * @param id The unique identifier for the food item
 * @param keywords A vector of keywords associated with the food item
 * This constructor initializes the CompositeFood object with a unique id and a list of keywords.
 */
CompositeFood::CompositeFood(const std::string& id, const std::vector<std::string>& keywords)
    : Food(id, keywords), totalCalories(0) {
}

/**
 * addComponent Method
 * @param foodId The unique identifier for the component food item
 * @param servings The number of servings of the component food item
 * This method adds a component food item and its servings to the composite food item.
 */
void CompositeFood::addComponent(const std::string& foodId, float servings) {
    components[foodId] += servings;
}

/**
 * getComponents Method
 * @return A map of component food items and their servings
 * This method returns a map of component food items and their servings.
 */
map<string, float> CompositeFood::getComponents() const {
    return components;
}

/**
 * setTotalCalories Method
 * @param calories The total number of calories for the composite food item
 * This method sets the total number of calories for the composite food item.
 */
void CompositeFood::setTotalCalories(float calories) {
    totalCalories = calories;
}

/**
 * getCaloriesPerServing Method
 * @return The number of calories per serving of the composite food item
 * This method returns the number of calories per serving of the composite food item.
 */
float CompositeFood::getCaloriesPerServing() const {
    return totalCalories;
}

/**
 * isComposite Method
 * @return A boolean indicating whether the food item is a composite food item
 * This method returns true, indicating that CompositeFood is a composite food item.
 */
bool CompositeFood::isComposite() const {
    return true;
}
