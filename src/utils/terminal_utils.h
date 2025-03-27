#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <sys/ioctl.h>
#include <unistd.h>

namespace TerminalUtils {
    // Terminal size functions
    inline std::pair<int, int> getTerminalSize() {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        
        // Default values if detection fails
        int width = w.ws_col > 0 ? w.ws_col : 80;
        int height = w.ws_row > 0 ? w.ws_row : 24;
        
        return std::make_pair(width, height);
    }
    
    // Clear the terminal screen
    inline void clearScreen() {
        // ANSI escape code to clear screen and move cursor to home position
        std::cout << "\033[2J\033[H";
    }
    
    // Move cursor to position
    inline void moveCursor(int row, int col) {
        std::cout << "\033[" << row << ";" << col << "H";
    }
    
    // Text formatting functions
    inline std::string centerText(const std::string& text, int width) {
        if (width <= text.length()) return text;
        
        int padding = (width - text.length()) / 2;
        return std::string(padding, ' ') + text;
    }
    
    inline std::string rightAlign(const std::string& text, int width) {
        if (width <= text.length()) return text;
        
        int padding = width - text.length();
        return std::string(padding, ' ') + text;
    }
    
    // Create a horizontal separator line
    inline std::string horizontalLine(int width, char symbol = '─') {
        return std::string(width, symbol);
    }
    
    // Box drawing functions
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
    
    // Progress bar
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
    
    // Check if terminal supports colors
    inline bool supportsColor() {
        const char* term = std::getenv("TERM");
        if (term == nullptr) return false;
        
        std::string termStr(term);
        return termStr != "dumb" && termStr != "vt100" && termStr.find("xterm") != std::string::npos;
    }
}

#endif // TERMINAL_UTILS_H
