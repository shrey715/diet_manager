#include "cli.h"
#include <iostream>
#include <filesystem>
#include "utils/data_checker.h"

namespace fs = std::filesystem;

/**
 * @brief Create directory for a file if it doesn't exist
 * @param path Path to the file
 */
void ensureDirectoryExists(const std::string& path) {
    fs::path dirPath = fs::path(path).parent_path();
    if (!dirPath.empty() && !fs::exists(dirPath)) {
        try {
            fs::create_directories(dirPath);
        } catch (const std::exception& e) {
            std::cerr << "Failed to create directory: " << dirPath << "\n" 
                      << "Error: " << e.what() << std::endl;
        }
    }
}

/**
 * @brief Main program entry point
 * @param argc Argument count
 * @param argv Argument values
 * @return Exit status code
 */
int main(int argc, char* argv[]) {
    // Define data paths
    std::string dataDir = "data";
    std::string basicFoodPath = dataDir + "/basic_food.json";
    std::string compositeFoodPath = dataDir + "/composite_food.json";
    std::string logPath = dataDir + "/logs.json";
    std::string userPath = dataDir + "/user.json";
    
    // Process command-line arguments
    if (argc > 1) dataDir = argv[1];
    if (argc > 2) basicFoodPath = argv[2];
    if (argc > 3) compositeFoodPath = argv[3];
    if (argc > 4) logPath = argv[4];
    if (argc > 5) userPath = argv[5];
    
    try {
        // Set up environment
        ensureDirectoryExists(basicFoodPath);
        ensureDirectoryExists(compositeFoodPath);
        ensureDirectoryExists(logPath);
        ensureDirectoryExists(userPath);
        
        // Verify data files integrity
        DataChecker::checkDataFiles(basicFoodPath, compositeFoodPath, logPath, userPath);
        
        // Create and run CLI
        CLI cli(basicFoodPath, compositeFoodPath, logPath, userPath);
        cli.run();
        
        return 0;
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "ERROR: Memory allocation failed. " << e.what() << std::endl;
    }
    catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON ERROR: " << e.what() << std::endl;
        std::cerr << "Try fixing or deleting the corrupted data files." << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "An unknown error occurred." << std::endl;
    }
    
    return 1;
}
