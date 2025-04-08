#include "terminal_colors.h"
using namespace std;

namespace TerminalColors {
    // Color codes
    const string RESET = "\033[0m";
    const string BLACK = "\033[30m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN = "\033[36m";
    const string WHITE = "\033[37m";
    
    // Text styles
    const string BOLD = "\033[1m";
    const string UNDERLINE = "\033[4m";

    string colorize(const string& text, const string& color) {
        return color + text + RESET;
    }

    string bold(const string& text) {
        return BOLD + text + RESET;
    }

    string underline(const string& text) {
        return UNDERLINE + text + RESET;
    }

    string error(const string& text) {
        return BOLD + RED + text + RESET;
    }

    string success(const string& text) {
        return GREEN + text + RESET;
    }

    string warning(const string& text) {
        return YELLOW + text + RESET;
    }

    string info(const string& text) {
        return CYAN + text + RESET;
    }
}
