#include "../include/Vehicle.h"
#include <iomanip>
#include <sstream>

Vehicle::Vehicle(const std::string& id, const std::string& typeName, double capacity,
                 double currentLoad, bool available, const std::string& currentLocation,
                 const std::string& status, double fuelPerKm)
    : id(id), typeName(typeName), capacity(capacity), currentLoad(currentLoad),
      available(available), currentLocation(currentLocation), status(status),
      fuelPerKm(fuelPerKm) {}

bool Vehicle::canAccommodate(double additionalWaste) const {
    if (!available) return false;
    return (currentLoad + additionalWaste) <= getEffectiveCapacity();
}

bool Vehicle::loadWaste(double wasteAmount) {
    if (canAccommodate(wasteAmount)) {
        currentLoad += wasteAmount;
        return true;
    }
    return false;
}

void Vehicle::resetLoad() {
    currentLoad = 0.0;
    available = true;
    status = "Available";
    currentLocation = "Depot";
}

void Vehicle::displayTableRow() const {
    std::cout << "| " << std::left << std::setw(6) << id
              << "| " << std::setw(18) << getTypeDescription()
              << "| " << std::right << std::setw(6) << std::fixed << std::setprecision(1) << capacity << " kg"
              << " | " << std::setw(6) << std::fixed << std::setprecision(1) << currentLoad << " kg"
              << " | " << std::setw(6) << std::fixed << std::setprecision(1) << getRemainingCapacity() << " kg"
              << " | " << std::setw(15) << (available ? (Color::GREEN + "Yes" + Color::RESET) : (Color::RED + "No " + Color::RESET))
              << " | " << std::left << std::setw(14) << currentLocation
              << "| " << std::setw(12) << status
              << " |\n";
}

void Vehicle::displayCard() const {
    std::cout << "------------------------------------------\n";
    std::cout << "Vehicle ID:         " << id << "\n";
    std::cout << "Type:               " << getTypeDescription() << "\n";
    std::cout << "Nominal Capacity:   " << capacity << " kg\n";
    std::cout << "Effective Capacity: " << getEffectiveCapacity() << " kg\n";
    std::cout << "Current Load:       " << currentLoad << " kg\n";
    std::cout << "Remaining Space:    " << getRemainingCapacity() << " kg\n";
    std::cout << "Available:          " << (available ? "Yes" : "No") << "\n";
    std::cout << "Location:           " << currentLocation << "\n";
    std::cout << "Status:             " << status << "\n";
    std::cout << "------------------------------------------\n";
}

std::string Vehicle::toCSV() const {
    std::ostringstream oss;
    oss << id << ",Generic," << capacity << "," << currentLoad << ","
        << (available ? 1 : 0) << "," << currentLocation << "," << status << ",1.0";
    return oss.str();
}

// Derived: StandardCompactorTruck
StandardCompactorTruck::StandardCompactorTruck(
    const std::string& id, double capacity, double currentLoad,
    bool available, const std::string& currentLocation,
    const std::string& status, double compactionFactor)
    : Vehicle(id, "CompactorTruck", capacity, currentLoad, available, currentLocation, status, 0.28),
      compactionFactor(compactionFactor) {}

void StandardCompactorTruck::displayTableRow() const {
    Vehicle::displayTableRow();
}

std::string StandardCompactorTruck::toCSV() const {
    std::ostringstream oss;
    oss << id << ",Compactor," << capacity << "," << currentLoad << ","
        << (available ? 1 : 0) << "," << currentLocation << "," << status << "," << compactionFactor;
    return oss.str();
}

// Derived: ElectricMiniTruck
ElectricMiniTruck::ElectricMiniTruck(
    const std::string& id, double capacity, double currentLoad,
    bool available, const std::string& currentLocation,
    const std::string& status, int batteryLevel)
    : Vehicle(id, "ElectricMini", capacity, currentLoad, available, currentLocation, status, 0.12),
      batteryLevelPercent(batteryLevel) {}

void ElectricMiniTruck::displayTableRow() const {
    Vehicle::displayTableRow();
}

std::string ElectricMiniTruck::toCSV() const {
    std::ostringstream oss;
    oss << id << ",Electric," << capacity << "," << currentLoad << ","
        << (available ? 1 : 0) << "," << currentLocation << "," << status << "," << batteryLevelPercent;
    return oss.str();
}

Vehicle* Vehicle::fromCSV(const std::string& line) {
    std::stringstream ss(line);
    std::string id, type, capStr, loadStr, availStr, loc, status, extraStr;

    if (std::getline(ss, id, ',') &&
        std::getline(ss, type, ',') &&
        std::getline(ss, capStr, ',') &&
        std::getline(ss, loadStr, ',') &&
        std::getline(ss, availStr, ',') &&
        std::getline(ss, loc, ',') &&
        std::getline(ss, status, ',')) {

        std::getline(ss, extraStr);
        if (!extraStr.empty() && extraStr.back() == '\r') extraStr.pop_back();

        double cap = std::stod(capStr);
        double load = std::stod(loadStr);
        bool avail = (availStr == "1" || availStr == "true" || availStr == "Yes");

        if (type == "Compactor") {
            double comp = extraStr.empty() ? 1.25 : std::stod(extraStr);
            return new StandardCompactorTruck(id, cap, load, avail, loc, status, comp);
        } else if (type == "Electric") {
            int batt = extraStr.empty() ? 100 : std::stoi(extraStr);
            return new ElectricMiniTruck(id, cap, load, avail, loc, status, batt);
        } else {
            return new Vehicle(id, "StandardTruck", cap, load, avail, loc, status);
        }
    }
    return nullptr;
}
