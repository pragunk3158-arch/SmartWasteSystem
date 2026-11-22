#include "../include/Vehicle.h"
#include "../include/Common.h"
#include <cstdio>

using namespace std;

Vehicle::Vehicle(string id, double capacity, double currentLoad,
                 bool available, string location, string status) {
    this->id = cleanText(id);
    this->capacity = capacity;
    this->currentLoad = currentLoad;
    this->available = available;
    this->location = cleanText(location);
    this->status = cleanText(status);
}

bool Vehicle::canCarry(double extraWaste) const {
    if (!available) return false;
    return (currentLoad + extraWaste) <= getEffectiveCapacity();
}

bool Vehicle::loadWaste(double waste) {
    if (!canCarry(waste)) return false;
    currentLoad = currentLoad + waste;
    return true;
}

void Vehicle::resetLoad() {
    currentLoad = 0;
    available = true;
    status = "Available";
    location = "Depot";
}

void Vehicle::displayRow() const {
    printf("| %-6s| %-20s| %8.1f | %8.1f | %7.1f | %9.1f | %-4s| %-14s| %-10s |\n",
           id.c_str(), getTypeName().c_str(), capacity, getEffectiveCapacity(),
           currentLoad, getFreeSpace(), (available ? "Yes" : "No"),
           location.c_str(), status.c_str());
}

string Vehicle::toCSV() const {
    return id + ",Standard," + numberToText(capacity) + "," + numberToText(currentLoad) +
           "," + (available ? "1" : "0") + "," + location + "," + status + ",0";
}

// ---------------- CompactorTruck ----------------

CompactorTruck::CompactorTruck(string id, double capacity, double currentLoad,
                               bool available, string location, string status,
                               double compactionFactor)
    : Vehicle(id, capacity, currentLoad, available, location, status) {
    this->compactionFactor = compactionFactor;
}

string CompactorTruck::toCSV() const {
    return id + ",Compactor," + numberToText(capacity) + "," + numberToText(currentLoad) +
           "," + (available ? "1" : "0") + "," + location + "," + status + "," +
           numberToText(compactionFactor);
}

// ---------------- ElectricMiniTruck ----------------

ElectricMiniTruck::ElectricMiniTruck(string id, double capacity, double currentLoad,
                                     bool available, string location, string status,
                                     int batteryPercent)
    : Vehicle(id, capacity, currentLoad, available, location, status) {
    this->batteryPercent = batteryPercent;
}

string ElectricMiniTruck::toCSV() const {
    return id + ",Electric," + numberToText(capacity) + "," + numberToText(currentLoad) +
           "," + (available ? "1" : "0") + "," + location + "," + status + "," +
           numberToText(batteryPercent);
}

// Reads: V01,Compactor,500,0,1,Depot,Available,1.25
// The Type column decides which class of object is created.
Vehicle* Vehicle::fromCSV(string line) {
    string parts[8];
    int count = splitLine(line, parts, 8);
    if (count < 8) return NULL;

    double capacity, load, extra;
    if (parts[0] == "") return NULL;
    if (!textToNumber(parts[2], capacity)) return NULL;
    if (!textToNumber(parts[3], load)) return NULL;
    if (capacity <= 0) return NULL;
    if (!textToNumber(parts[7], extra)) extra = 0;

    string id = parts[0];
    string type = parts[1];
    bool available = (parts[4] == "1");
    string location = parts[5];
    string status = parts[6];

    if (type == "Compactor") {
        double factor = (extra >= 1.0) ? extra : 1.25;
        return new CompactorTruck(id, capacity, load, available, location, status, factor);
    }
    if (type == "Electric") {
        int battery = (extra > 0) ? (int)extra : 100;
        return new ElectricMiniTruck(id, capacity, load, available, location, status, battery);
    }
    return new Vehicle(id, capacity, load, available, location, status);
}
