#ifndef VEHICLE_H
#define VEHICLE_H

#include "Common.h"
#include <string>
#include <iostream>

// ============================================================================
// OOP CONCEPT: INHERITANCE & POLYMORPHISM
// Base Class: Vehicle
// Demonstrates:
// - Encapsulation (protected fields, public accessors)
// - Virtual methods for Polymorphism
// - Virtual destructor
// ============================================================================
class Vehicle {
protected:
    std::string id;
    std::string typeName;
    double capacity;       // nominal capacity in kg
    double currentLoad;    // current payload in kg
    bool available;
    std::string currentLocation;
    std::string status;    // "Available", "On Route", "Maintenance"
    double fuelPerKm;      // liters of fuel / kWh per km

public:
    Vehicle(const std::string& id, const std::string& typeName, double capacity,
            double currentLoad = 0.0, bool available = true,
            const std::string& currentLocation = "Depot",
            const std::string& status = "Available", double fuelPerKm = 0.25);

    virtual ~Vehicle() {}

    // Polymorphic method: derived vehicles calculate effective capacity differently
    virtual double getEffectiveCapacity() const { return capacity; }
    virtual double getCompactionRatio() const { return 1.0; }
    virtual std::string getTypeDescription() const { return typeName; }

    // Capacity checking
    bool canAccommodate(double additionalWaste) const;
    bool loadWaste(double wasteAmount);
    void resetLoad();

    // Getters
    std::string getId() const { return id; }
    std::string getTypeName() const { return typeName; }
    double getCapacity() const { return capacity; }
    double getCurrentLoad() const { return currentLoad; }
    double getRemainingCapacity() const { return getEffectiveCapacity() - currentLoad; }
    bool isAvailable() const { return available; }
    std::string getCurrentLocation() const { return currentLocation; }
    std::string getStatus() const { return status; }
    double getFuelPerKm() const { return fuelPerKm; }

    // Setters
    void setAvailable(bool avail) { available = avail; }
    void setCurrentLocation(const std::string& loc) { currentLocation = loc; }
    void setStatus(const std::string& stat) { status = stat; }
    void setCurrentLoad(double load) { currentLoad = load; }

    // Display
    virtual void displayTableRow() const;
    virtual void displayCard() const;

    // File Serialization
    virtual std::string toCSV() const;
    static Vehicle* fromCSV(const std::string& line);
};

// ============================================================================
// DERIVED CLASS 1: StandardCompactorTruck
// Demonstrates:
// - Inheritance (is-a Vehicle)
// - Method overriding (polymorphism for compaction factor)
// ============================================================================
class StandardCompactorTruck : public Vehicle {
private:
    double compactionFactor; // e.g., 1.25 (can compress waste)

public:
    StandardCompactorTruck(const std::string& id, double capacity,
                           double currentLoad = 0.0, bool available = true,
                           const std::string& currentLocation = "Depot",
                           const std::string& status = "Available",
                           double compactionFactor = 1.25);

    double getEffectiveCapacity() const override {
        return capacity * compactionFactor;
    }

    double getCompactionRatio() const override {
        return compactionFactor;
    }

    std::string getTypeDescription() const override {
        return "Compactor Truck (" + std::to_string((int)((compactionFactor - 1.0) * 100)) + "% comp.)";
    }

    void displayTableRow() const override;
    std::string toCSV() const override;
};

// ============================================================================
// DERIVED CLASS 2: ElectricMiniTruck
// Demonstrates:
// - Inheritance (is-a Vehicle)
// - Eco-friendly campus vehicle with lower emissions
// ============================================================================
class ElectricMiniTruck : public Vehicle {
private:
    int batteryLevelPercent;

public:
    ElectricMiniTruck(const std::string& id, double capacity,
                      double currentLoad = 0.0, bool available = true,
                      const std::string& currentLocation = "Depot",
                      const std::string& status = "Available",
                      int batteryLevel = 100);

    double getEffectiveCapacity() const override {
        return capacity; // standard uncompacted
    }

    std::string getTypeDescription() const override {
        return "Electric Mini-Truck (EV)";
    }

    int getBatteryLevel() const { return batteryLevelPercent; }
    void setBatteryLevel(int b) { batteryLevelPercent = b; }

    void displayTableRow() const override;
    std::string toCSV() const override;
};

#endif // VEHICLE_H
