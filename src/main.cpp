/**
 * @file main.cpp
 * @brief Main entry point for the diet manager application.
 * 
 */

#include <iostream>
#include <string>
#include <filesystem>
#include "cli.h"

/**
 * Create directories if they don't exist
 */
void ensureDirectoriesExist() {
    std::filesystem::create_directories("data");
}

/**
 * Main function - entry point of the application
 */
int main(int argc, char* argv[]) {
    try {
        // Ensure data directory exists
        ensureDirectoriesExist();
        
        // Custom data paths could be provided as command line args
        // Not implemented in this version, but could be added
        (void)argc; // Suppress unused parameter warning
        (void)argv; // Suppress unused parameter warning
        
        // Initialize and run the CLI
        CLI cli;
        cli.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
