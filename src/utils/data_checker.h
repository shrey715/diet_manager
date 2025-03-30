#ifndef DATA_CHECKER_H
#define DATA_CHECKER_H

#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

/**
 * @namespace DataChecker
 * @brief Utilities for checking and fixing data files
 */
namespace DataChecker {
    /**
     * @brief Check if a JSON file is valid and create it if needed
     * @param filePath Path to the file
     * @param createIfMissing Whether to create the file if missing
     * @return True if file is valid or was created successfully
     */
    inline bool isValidJsonFile(const std::string& filePath, bool createIfMissing = true) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << filePath << std::endl;
            
            if (createIfMissing) {
                std::ofstream newFile(filePath);
                if (newFile.is_open()) {
                    newFile << "[]";  // Create empty JSON array
                    newFile.close();
                    std::cerr << "Created new JSON file: " << filePath << std::endl;
                    return true;
                }
                std::cerr << "Error: Could not create file: " << filePath << std::endl;
            }
            return false;
        }
        
        try {
            std::string content((std::istreambuf_iterator<char>(file)), 
                               std::istreambuf_iterator<char>());
            file.close();
            
            if (content.empty()) {
                // Fix empty file
                std::ofstream newFile(filePath);
                if (newFile.is_open()) {
                    newFile << "[]";
                    newFile.close();
                    std::cerr << "Fixed empty file: " << filePath << std::endl;
                }
                return true;
            }
            
            // Test parsing (store result to avoid warning)
            auto parsed = nlohmann::json::parse(content);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid JSON in " << filePath << ": " << e.what() << std::endl;
            
            if (createIfMissing) {
                // Replace corrupted file
                std::ofstream newFile(filePath);
                if (newFile.is_open()) {
                    newFile << "[]";
                    newFile.close();
                    std::cerr << "Replaced corrupted file: " << filePath << std::endl;
                    return true;
                }
            }
            return false;
        }
    }
    
    /**
     * @brief Check and fix all data files
     * @param basicFoodPath Path to basic food database
     * @param compositeFoodPath Path to composite food database
     * @param logPath Path to log database
     * @param userPath Path to user profile
     */
    inline void checkDataFiles(const std::string& basicFoodPath, 
                              const std::string& compositeFoodPath,
                              const std::string& logPath,
                              const std::string& userPath) {
        isValidJsonFile(basicFoodPath);
        isValidJsonFile(compositeFoodPath);
        isValidJsonFile(logPath);
        isValidJsonFile(userPath, false); // Don't auto-create user file
    }
}

#endif // DATA_CHECKER_H
