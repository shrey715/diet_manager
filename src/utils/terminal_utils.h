#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <sys/ioctl.h>
#include <unistd.h>

/**
 * @namespace TerminalUtils
 * @brief Utility functions for terminal display and formatting
 */
namespace TerminalUtils {
    /**
     * @brief Get current terminal dimensions
     * @return Pair containing width and height
     */
    inline std::pair<int, int> getTerminalSize() {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        
        // Default values if detection fails
        return std::make_pair(
            w.ws_col > 0 ? w.ws_col : 80,
            w.ws_row > 0 ? w.ws_row : 24
        );
    }
    
    /**
     * @brief Clear the terminal screen
     */
    inline void clearScreen() {
        std::cout << "\033[2J\033[H";
    }
    
    /**
     * @brief Move cursor to position
     * @param row Row position
     * @param col Column position
     */
    inline void moveCursor(int row, int col) {
        std::cout << "\033[" << row << ";" << col << "H";
    }
    
    /**
     * @brief Center text in a given width
     * @param text Text to center
     * @param width Width to center within
     * @return Centered text string
     */
    inline std::string centerText(const std::string& text, int width) {
        if (width <= text.length()) return text;
        
        int padding = (width - text.length()) / 2;
        return std::string(padding, ' ') + text + 
               std::string(width - text.length() - padding, ' ');
    }
    
    /**
     * @brief Right align text in a given width
     * @param text Text to align
     * @param width Width to align within
     * @return Right aligned text string
     */
    inline std::string rightAlign(const std::string& text, int width) {
        if (width <= text.length()) return text;
        
        int padding = width - text.length();
        return std::string(padding, ' ') + text;
    }
    
    /**
     * @brief Create a horizontal separator line
     * @param width Width of the line
     * @param c Character to use for line
     * @return Formatted line string
     */
    inline std::string horizontalLine(int width, char c = '─') {
        return std::string(width, c);
    }
    
    /**
     * @brief Draw a box with specified dimensions
     * @param width Width of the box
     * @param height Height of the box
     * @param startRow Starting row position
     * @param startCol Starting column position
     */
    inline void drawBox(int width, int height, int startRow = 1, int startCol = 1) {
        // Top border
        moveCursor(startRow, startCol);
        std::cout << "┌" << horizontalLine(width - 2) << "┐";
        
        // Sides
        for (int i = 1; i < height - 1; i++) {
            moveCursor(startRow + i, startCol);
            std::cout << "│";
            moveCursor(startRow + i, startCol + width - 1);
            std::cout << "│";
        }
        
        // Bottom border
        moveCursor(startRow + height - 1, startCol);
        std::cout << "└" << horizontalLine(width - 2) << "┘";
    }
    
    /**
     * @brief Create a progress bar
     * @param percentage Current percentage (0-100)
     * @param width Width of the progress bar
     * @param showPercentage Whether to show percentage
     * @return Formatted progress bar string
     */
    inline std::string progressBar(double percentage, int width, bool showPercentage = true) {
        if (percentage < 0) percentage = 0;
        if (percentage > 100) percentage = 100;
        
        int barWidth = width - (showPercentage ? 7 : 2); // Leave space for percentage display
        int fill = static_cast<int>(barWidth * percentage / 100.0);
        
        std::string bar = "[";
        for (int i = 0; i < barWidth; i++) {
            if (i < fill) {
                bar += "█";
            } else {
                bar += "░";
            }
        }
        
        if (showPercentage) {
            bar += "] ";
            std::stringstream ss;
            ss << std::fixed << std::setprecision(1) << percentage << "%";
            bar += ss.str();
        } else {
            bar += "]";
        }
        
        return bar;
    }
    
    /**
     * @brief Check if terminal supports colors
     * @return True if terminal supports colors, false otherwise
     */
    inline bool supportsColor() {
        const char* term = std::getenv("TERM");
        if (term == nullptr) return false;
        
        std::string termStr(term);
        return termStr != "dumb" && termStr != "vt100" && termStr.find("xterm") != std::string::npos;
    }
}

#endif // TERMINAL_UTILS_H
