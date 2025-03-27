#ifndef DATA_CHECKER_H
#define DATA_CHECKER_H

#include <string>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace DataChecker {
    // Check if file exists and contains valid JSON
    inline bool isValidJsonFile(const std::string& filePath, bool createIfMissing = true) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file: " << filePath << std::endl;
            
            if (createIfMissing) {
                std::ofstream newFile(filePath);
                if (newFile.is_open()) {
                    newFile << "[]";  // Write an empty JSON array
                    newFile.close();
                    std::cerr << "Created new empty JSON file: " << filePath << std::endl;
                    return true;
                } else {
                    std::cerr << "Error: Could not create file: " << filePath << std::endl;
                    return false;
                }
            }
            
            return false;
        }
        
        try {
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            file.close();
            
            if (content.empty()) {
                // Empty file - replace with valid JSON
                std::ofstream newFile(filePath);
                if (newFile.is_open()) {
                    newFile << "[]";  // Write an empty JSON array
                    newFile.close();
                    std::cerr << "File was empty. Replaced with empty JSON array: " << filePath << std::endl;
                }
                return true;
            }
            
            // Test if it can be parsed (and store the result to avoid the warning)
            auto parsedJson = nlohmann::json::parse(content);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error: Invalid JSON in file " << filePath << ": " << e.what() << std::endl;
            
            if (createIfMissing) {
                std::ofstream newFile(filePath);
                if (newFile.is_open()) {
                    newFile << "[]";  // Write an empty JSON array
                    newFile.close();
                    std::cerr << "Replaced corrupted file with empty JSON array: " << filePath << std::endl;
                    return true;
                }
            }
            
            return false;
        }
    }
    
    // Fix all data files
    inline void checkAndFixDataFiles(const std::string& basicFoodPath, 
                                    const std::string& compositeFoodPath,
                                    const std::string& logPath,
                                    const std::string& userPath) {
        isValidJsonFile(basicFoodPath);
        isValidJsonFile(compositeFoodPath);
        isValidJsonFile(logPath);
        isValidJsonFile(userPath, false);  // Don't create user file automatically
    }
}

#endif // DATA_CHECKER_H
