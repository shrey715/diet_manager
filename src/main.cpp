#include "cli.h"
#include <iostream>
#include <string>
#include <filesystem>
#include "utils/data_checker.h"

namespace fs = std::filesystem;

// oh god this function is for making sure directories exist
// because apparently we need this for some reason
void ensureDirectoryExists(const std::string& path) {
    fs::path dirPath = fs::path(path).parent_path();
    if (!dirPath.empty() && !fs::exists(dirPath)) {
        // create the directory if it doesn't exist already
        // please don't fail, I really don't want to debug this at 3am
        try {
            fs::create_directories(dirPath);
        } catch (const std::exception& e) {
            std::cerr << "Failed to create directory: " << dirPath << " Error: " << e.what() << std::endl;
            // just let it crash later, whatever
        }
    }
}

int main(int argc, char* argv[]) {
    // default file paths, can override with cmd args if you care enough
    std::string dataDir = "data";
    std::string basicFoodFilePath = dataDir + "/basic_food.json";
    std::string compositeFoodFilePath = dataDir + "/composite_food.json";
    std::string logFilePath = dataDir + "/logs.json";
    std::string userFilePath = dataDir + "/user.json";
    
    // Override paths from command-line arguments if provided
    // because apparently someone might want to do that
    if (argc > 1) dataDir = argv[1];
    if (argc > 2) basicFoodFilePath = argv[2];
    if (argc > 3) compositeFoodFilePath = argv[3];
    if (argc > 4) logFilePath = argv[4];
    if (argc > 5) userFilePath = argv[5];
    
    // make sure directories exist so we don't crash later
    try {
        ensureDirectoryExists(basicFoodFilePath);
        ensureDirectoryExists(compositeFoodFilePath);
        ensureDirectoryExists(logFilePath);
        ensureDirectoryExists(userFilePath);
    } catch (const std::exception& e) {
        std::cerr << "Error creating data directories: " << e.what() << std::endl;
        std::cerr << "Fix your file system. I'm out." << std::endl;
        return 1;
    }
    
    // Set up signal handlers for better crash reporting
    std::set_terminate([]() {
        std::cerr << "Unhandled exception detected!" << std::endl;
        std::cerr << "Program terminated abnormally." << std::endl;
        std::abort();
    });
    
    // Check and fix data files if needed
    try {
        DataChecker::checkAndFixDataFiles(basicFoodFilePath, compositeFoodFilePath, logFilePath, userFilePath);
    } catch (const std::exception& e) {
        std::cerr << "Error checking data files: " << e.what() << std::endl;
        std::cerr << "The program will try to continue anyway..." << std::endl;
    }
    
    // actual program starts here
    try {
        // First check if files are readable
        if (!std::filesystem::exists(basicFoodFilePath) || 
            !std::filesystem::exists(compositeFoodFilePath)) {
            // Create empty files if they don't exist
            std::ofstream bf(basicFoodFilePath), cf(compositeFoodFilePath);
            bf << "[]"; // Valid empty JSON array
            cf << "[]"; // Valid empty JSON array
        }
        
        CLI cli(basicFoodFilePath, compositeFoodFilePath, logFilePath, userFilePath);
        cli.run();
    } catch (const std::bad_alloc& e) {
        std::cerr << "MEMORY ERROR: Out of memory! " << std::endl;
        std::cerr << "Try closing other applications or freeing up RAM." << std::endl;
        return 1;
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "JSON ERROR: " << e.what() << std::endl;
        std::cerr << "One of your data files might be corrupted. Check them and try again." << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        std::cerr << "Program crashed. Contact support (lol jk, it's just me)" << std::endl;
        return 1;
    } catch (...) {
        // catch anything else because who knows what could happen
        std::cerr << "Unknown error occurred. I didn't plan for this." << std::endl;
        return 1;
    }
    
    // if we got here, miracle happened and program exited normally
    return 0;
}
