#ifndef COLOR_H
#define COLOR_H

#include <string>

/**
 * @namespace Colors
 * @brief ANSI color codes for terminal output
 */
namespace Colors {
    // Reset all formatting
    const std::string RESET = "\033[0m";
    
    // Text styles
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    
    // Basic colors - using bright variants for better visibility
    const std::string RED = "\033[91m";      // Error messages
    const std::string GREEN = "\033[92m";    // Success messages
    const std::string YELLOW = "\033[93m";   // Warnings
    const std::string BLUE = "\033[94m";     // Headers
    const std::string MAGENTA = "\033[95m";  // Highlights
    const std::string CYAN = "\033[96m";     // Information
    const std::string WHITE = "\033[97m";    // Normal text
    
    // Semantic aliases
    const std::string ERROR = RED;
    const std::string SUCCESS = GREEN;
    const std::string WARNING = YELLOW;
    const std::string INFO = CYAN;
    const std::string HEADER = BLUE + BOLD;
    
    // Simple background colors
    const std::string BG_BLACK = "\033[40m";
    const std::string BG_BLUE = "\033[44m";
    
    /**
     * @brief Check if the terminal supports colors
     * @return True if colors are supported
     */
    inline bool isSupported() {
        const char* term = std::getenv("TERM");
        const char* noColor = std::getenv("NO_COLOR");
        
        if (noColor != nullptr) return false;
        if (term == nullptr) return false;
        
        std::string termStr(term);
        return termStr != "dumb" && termStr.find("xterm") != std::string::npos;
    }
    
    /**
     * @brief Wraps text in specified color
     * @param text Text to colorize
     * @param color Color to use
     * @return Colorized string
     */
    inline std::string colorize(const std::string& text, const std::string& color) {
        return color + text + RESET;
    }
}

#endif // COLOR_H
