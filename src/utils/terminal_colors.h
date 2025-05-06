/**
 * @file terminal_colors.h
 * @brief Terminal Text Formatting Utilities
 * 
 * This file defines a namespace with constants and utility functions for
 * formatting text output in the terminal with colors and styles. It provides
 * a simple interface for adding visual elements to the command-line interface.
 * 
 * Key components:
 * - Color code constants for different text colors
 * - Text style constants (bold, underline)
 * - Utility functions for applying colors and styles to text
 * - Specialized formatting for different message types (error, success, warning, info)
 * 
 * These utilities enhance the user experience by providing visual cues and
 * improving the readability of the command-line interface.
 */

#ifndef TERMINAL_COLORS_H
#define TERMINAL_COLORS_H

#include <string>
using namespace std;

namespace TerminalColors {
    // Color codes
    extern const string RESET;
    extern const string BLACK;
    extern const string RED;
    extern const string GREEN;
    extern const string YELLOW;
    extern const string BLUE;
    extern const string MAGENTA;
    extern const string CYAN;
    extern const string WHITE;
    
    // Text styles
    extern const string BOLD;
    extern const string UNDERLINE;

    // Utility functions
    string colorize(const string& text, const string& color);
    string bold(const string& text);
    string underline(const string& text);
    string error(const string& text);
    string success(const string& text);
    string warning(const string& text);
    string info(const string& text);
}

#endif // TERMINAL_COLORS_H
