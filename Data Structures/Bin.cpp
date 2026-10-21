#include "../include/Bin.h"
#include <iomanip>
#include <sstream>

Bin::Bin()
    : id(""), location(""), capacity(100.0), currentWaste(0.0),
      fillPercentage(0.0), fillRate(5.0), priority(PriorityLevel::LOW),
      urgencyScore(0.0), status("Pending") {
    calculatePriority();
}

Bin::Bin(const std::string& id, const std::string& location, double capacity,
         double currentWaste, double fillRate, const std::string& status)
    : id(id), location(location), capacity(capacity), currentWaste(currentWaste),
      fillRate(fillRate), status(status) {
    calculatePriority();
}

void Bin::calculatePriority() {
    if (capacity <= 0.0) {
        fillPercentage = 0.0;
    } else {
        fillPercentage = (currentWaste / capacity) * 100.0;
    }

    if (fillPercentage > 100.0) fillPercentage = 100.0;
    if (fillPercentage < 0.0) fillPercentage = 0.0;

    // College Specification:
    // 90% or above -> CRITICAL
    // 75% to 89% -> HIGH
    // 50% to 74% -> MEDIUM
    // Below 50% -> LOW
    if (fillPercentage >= 90.0) {
        priority = PriorityLevel::CRITICAL;
    } else if (fillPercentage >= 75.0) {
        priority = PriorityLevel::HIGH;
    } else if (fillPercentage >= 50.0) {
        priority = PriorityLevel::MEDIUM;
    } else {
        priority = PriorityLevel::LOW;
    }

    // Urgency formula combines fill percentage with daily fill rate risk
    // High fill rate means faster overflow risk!
    urgencyScore = fillPercentage + (fillRate * 1.5);
    if (priority == PriorityLevel::CRITICAL) urgencyScore += 50.0; // Boost critical bins
}

void Bin::setCurrentWaste(double waste) {
    currentWaste = waste;
    if (currentWaste < 0.0) currentWaste = 0.0;
    calculatePriority();
}

void Bin::setFillRate(double rate) {
    fillRate = rate;
    calculatePriority();
}

void Bin::setStatus(const std::string& newStatus) {
    status = newStatus;
}

void Bin::setLocation(const std::string& newLoc) {
    location = newLoc;
}

void Bin::setCapacity(double newCap) {
    if (newCap > 0.0) {
        capacity = newCap;
        calculatePriority();
    }
}

void Bin::markCollected() {
    currentWaste = 0.0;
    status = "Collected";
    calculatePriority();
}

void Bin::displayTableRow() const {
    std::cout << "| " << std::left << std::setw(6) << id
              << "| " << std::setw(17) << location
              << "| " << std::right << std::setw(6) << std::fixed << std::setprecision(1) << capacity << " kg"
              << " | " << std::setw(6) << std::fixed << std::setprecision(1) << currentWaste << " kg"
              << " | " << std::setw(6) << std::fixed << std::setprecision(1) << fillPercentage << "%"
              << " | " << std::setw(19) << priorityToColorString(priority)
              << " | " << std::left << std::setw(10) << status
              << " |\n";
}

void Bin::displayCard() const {
    std::cout << "------------------------------------------\n";
    std::cout << "Bin ID:         " << id << "\n";
    std::cout << "Location:       " << location << "\n";
    std::cout << "Capacity:       " << capacity << " kg\n";
    std::cout << "Current Waste:  " << currentWaste << " kg (" << fillPercentage << "%)\n";
    std::cout << "Fill Rate:      " << fillRate << " kg/day\n";
    std::cout << "Priority:       " << priorityToString(priority) << "\n";
    std::cout << "Urgency Score:  " << std::fixed << std::setprecision(1) << urgencyScore << "\n";
    std::cout << "Status:         " << status << "\n";
    std::cout << "------------------------------------------\n";
}

std::string Bin::toCSV() const {
    std::ostringstream oss;
    oss << id << "," << location << "," << capacity << ","
        << currentWaste << "," << fillRate << "," << status;
    return oss.str();
}

Bin Bin::fromCSV(const std::string& line) {
    std::stringstream ss(line);
    std::string id, loc, capStr, wasteStr, rateStr, status;
    
    if (std::getline(ss, id, ',') &&
        std::getline(ss, loc, ',') &&
        std::getline(ss, capStr, ',') &&
        std::getline(ss, wasteStr, ',') &&
        std::getline(ss, rateStr, ',') &&
        std::getline(ss, status)) {
        
        // Remove carriage returns if present
        if (!status.empty() && status.back() == '\r') status.pop_back();

        double cap = std::stod(capStr);
        double waste = std::stod(wasteStr);
        double rate = std::stod(rateStr);
        return Bin(id, loc, cap, waste, rate, status);
    }
    return Bin();
}
