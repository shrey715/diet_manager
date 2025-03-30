#ifndef FORMAT_CLEANUP_H
#define FORMAT_CLEANUP_H

#include <string>
#include <sstream>
#include <algorithm>
#include <iomanip>

/**
 * @namespace FormatCleanup
 * @brief Utilities for cleaning up text formatting
 */
namespace FormatCleanup {
    /**
     * @brief Clean up a string by removing excessive whitespace
     * @param input String to clean
     * @return Cleaned string
     */
    inline std::string cleanWhitespace(const std::string& input) {
        std::string result;
        bool lastWasSpace = true;  // Start true to trim leading whitespace
        
        for (char c : input) {
            if (std::isspace(c)) {
                if (!lastWasSpace) {
                    result.push_back(' ');
                    lastWasSpace = true;
                }
            } else {
                result.push_back(c);
                lastWasSpace = false;
            }
        }
        
        // Trim trailing whitespace
        if (!result.empty() && result.back() == ' ') {
            result.pop_back();
        }
        
        return result;
    }
    
    /**
     * @brief Clean up a number string
     * @param number Number to format
     * @param precision Decimal precision
     * @return Formatted number string
     */
    inline std::string formatNumber(double number, int precision = 1) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << number;
        return ss.str();
    }
    
    /**
     * @brief Format a date string to YYYY-MM-DD
     * @param dateStr Date string to format
     * @return Formatted date string
     */
    inline std::string formatDateString(const std::string& dateStr) {
        // Simple validation - should be YYYY-MM-DD
        if (dateStr.length() != 10 || 
            dateStr[4] != '-' || 
            dateStr[7] != '-') {
            return dateStr; // Return unchanged if not in expected format
        }
        
        // Check if all other characters are digits
        for (size_t i = 0; i < dateStr.length(); i++) {
            if (i != 4 && i != 7 && !std::isdigit(dateStr[i])) {
                return dateStr; // Return unchanged if not in expected format
            }
        }
        
        return dateStr; // Already in correct format
    }
}

#endif // FORMAT_CLEANUP_H
