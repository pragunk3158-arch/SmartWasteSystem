#ifndef BIN_H
#define BIN_H

#include "Common.h"
#include <string>
#include <iostream>

// ============================================================================
// CLASS: Bin
// Demonstrates:
// - Encapsulation (private data members, public getters/setters)
// - Constructors (default & parameterized)
// - Priority determination logic
// - CSV serialization for file handling
// ============================================================================
class Bin {
private:
    std::string id;
    std::string location;
    double capacity;        // in kg
    double currentWaste;    // in kg
    double fillPercentage;  // in %
    double fillRate;        // in kg/day
    PriorityLevel priority;
    double urgencyScore;    // computed score for Priority Queue ranking
    std::string status;     // "Pending", "Collected", "In Route"

public:
    Bin();
    Bin(const std::string& id, const std::string& location, double capacity,
        double currentWaste, double fillRate = 5.0, const std::string& status = "Pending");

    // Recalculates fill percentage, PriorityLevel, and urgency score
    void calculatePriority();

    // Getters
    std::string getId() const { return id; }
    std::string getLocation() const { return location; }
    double getCapacity() const { return capacity; }
    double getCurrentWaste() const { return currentWaste; }
    double getFillPercentage() const { return fillPercentage; }
    double getFillRate() const { return fillRate; }
    PriorityLevel getPriority() const { return priority; }
    double getUrgencyScore() const { return urgencyScore; }
    std::string getStatus() const { return status; }

    // Setters
    void setCurrentWaste(double waste);
    void setFillRate(double rate);
    void setStatus(const std::string& newStatus);
    void setLocation(const std::string& newLoc);
    void setCapacity(double newCap);

    // Mark as collected (resets waste to 0)
    void markCollected();

    // Formatted terminal display
    void displayTableRow() const;
    void displayCard() const;

    // File serialization
    std::string toCSV() const;
    static Bin fromCSV(const std::string& line);

    // Operator for Priority Queue ordering (greater urgency score = higher priority)
    bool operator<(const Bin& other) const {
        return this->urgencyScore < other.urgencyScore;
    }

    bool operator>(const Bin& other) const {
        return this->urgencyScore > other.urgencyScore;
    }
};

#endif // BIN_H
