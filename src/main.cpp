#include "cli.h"
#include <iostream>
#include <string>
#include <filesystem>

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
    // this assignment is already late and I can't deal with more bugs
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
    
    // actual program starts here
    // wrapped in try-catch because my professor will fail me if I don't handle exceptions
    try {
        CLI cli(basicFoodFilePath, compositeFoodFilePath, logFilePath, userFilePath);
        cli.run();
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
