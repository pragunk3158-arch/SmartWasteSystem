#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

// ANSI color codes for rich terminal display
namespace Color {
    const std::string RESET   = "\033[0m";
    const std::string BOLD    = "\033[1m";
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";
    const std::string WHITE   = "\033[37m";
    const std::string GRAY    = "\033[90m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_DARK = "\033[40m";
}

enum class PriorityLevel {
    LOW = 0,       // Below 50%
    MEDIUM = 1,    // 50% to 74%
    HIGH = 2,      // 75% to 89%
    CRITICAL = 3   // 90% and above
};

inline std::string priorityToString(PriorityLevel p) {
    switch (p) {
        case PriorityLevel::CRITICAL: return "CRITICAL";
        case PriorityLevel::HIGH:     return "HIGH";
        case PriorityLevel::MEDIUM:   return "MEDIUM";
        case PriorityLevel::LOW:      return "LOW";
        default:                      return "UNKNOWN";
    }
}

inline std::string priorityToColorString(PriorityLevel p) {
    switch (p) {
        case PriorityLevel::CRITICAL: return Color::RED + "CRITICAL" + Color::RESET;
        case PriorityLevel::HIGH:     return Color::YELLOW + "HIGH" + Color::RESET;
        case PriorityLevel::MEDIUM:   return Color::CYAN + "MEDIUM" + Color::RESET;
        case PriorityLevel::LOW:      return Color::GREEN + "LOW" + Color::RESET;
        default:                      return "UNKNOWN";
    }
}

inline PriorityLevel stringToPriority(const std::string& str) {
    if (str == "CRITICAL" || str == "Critical" || str == "critical") return PriorityLevel::CRITICAL;
    if (str == "HIGH" || str == "High" || str == "high")             return PriorityLevel::HIGH;
    if (str == "MEDIUM" || str == "Medium" || str == "medium")       return PriorityLevel::MEDIUM;
    return PriorityLevel::LOW;
}

#endif // COMMON_H
