#ifndef ROUTE_H
#define ROUTE_H

#include "Common.h"
#include <string>
#include <vector>

// ============================================================================
// CLASS: Route
// Encapsulates the complete optimized tour generated for waste collection
// ============================================================================
class Route {
private:
    std::string vehicleId;
    std::vector<std::string> pathNodes;      // Detailed node-by-node path
    std::vector<std::string> binIds;         // Bin IDs collected along this route
    std::vector<std::string> binLocations;   // Locations of the bins
    double totalDistance;                   // in km
    double estimatedTimeMinutes;            // in minutes
    double totalWasteCollected;             // in kg
    bool isReRouted;
    std::string reRouteReason;

public:
    Route();
    Route(const std::string& vehicleId,
          const std::vector<std::string>& pathNodes,
          const std::vector<std::string>& binIds,
          const std::vector<std::string>& binLocations,
          double totalDistance,
          double totalWasteCollected,
          bool isReRouted = false,
          const std::string& reRouteReason = "");

    // Getters
    std::string getVehicleId() const { return vehicleId; }
    const std::vector<std::string>& getPathNodes() const { return pathNodes; }
    const std::vector<std::string>& getBinIds() const { return binIds; }
    const std::vector<std::string>& getBinLocations() const { return binLocations; }
    double getTotalDistance() const { return totalDistance; }
    double getEstimatedTimeMinutes() const { return estimatedTimeMinutes; }
    double getTotalWasteCollected() const { return totalWasteCollected; }
    bool getIsReRouted() const { return isReRouted; }
    std::string getReRouteReason() const { return reRouteReason; }

    // Setters
    void setReRouted(bool reRouted, const std::string& reason);

    // Displays
    void displaySummary() const;
    void displayStepByStep() const;
};

#endif // ROUTE_H
