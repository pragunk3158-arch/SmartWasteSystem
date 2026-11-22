#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>

// Base class of all collection vehicles.
// Shows inheritance and polymorphism: the two derived classes below
// override getEffectiveCapacity() and getTypeName().
class Vehicle {
protected:
    std::string id;
    double capacity;        // kg written on the truck
    double currentLoad;     // kg already loaded
    bool available;
    std::string location;
    std::string status;     // "Available", "On Route", "Completed"

public:
    Vehicle(std::string id, double capacity, double currentLoad = 0,
            bool available = true, std::string location = "Depot",
            std::string status = "Available");

    virtual ~Vehicle() {}

    // Overridden by the derived classes
    virtual double getEffectiveCapacity() const { return capacity; }
    virtual std::string getTypeName() const { return "Standard Truck"; }
    virtual std::string toCSV() const;

    // Capacity checks
    bool canCarry(double extraWaste) const;
    bool loadWaste(double waste);
    void resetLoad();

    // Getters
    std::string getId() const { return id; }
    double getCapacity() const { return capacity; }
    double getCurrentLoad() const { return currentLoad; }
    double getFreeSpace() const { return getEffectiveCapacity() - currentLoad; }
    bool isAvailable() const { return available; }
    std::string getLocation() const { return location; }
    std::string getStatus() const { return status; }

    // Setters
    void setStatus(std::string s) { status = s; }

    void displayRow() const;

    // Reads one line of vehicles.txt and creates the right type of object
    static Vehicle* fromCSV(std::string line);
};

// Can press the waste, so it carries more than its written capacity
class CompactorTruck : public Vehicle {
private:
    double compactionFactor;   // e.g. 1.25 means 25% extra

public:
    CompactorTruck(std::string id, double capacity, double currentLoad = 0,
                   bool available = true, std::string location = "Depot",
                   std::string status = "Available", double compactionFactor = 1.25);

    double getEffectiveCapacity() const { return capacity * compactionFactor; }
    std::string getTypeName() const { return "Compactor Truck"; }
    std::string toCSV() const;
};

// Small battery vehicle used inside the campus
class ElectricMiniTruck : public Vehicle {
private:
    int batteryPercent;

public:
    ElectricMiniTruck(std::string id, double capacity, double currentLoad = 0,
                      bool available = true, std::string location = "Depot",
                      std::string status = "Available", int batteryPercent = 100);

    std::string getTypeName() const { return "Electric Mini Truck"; }
    std::string toCSV() const;
};

#endif
