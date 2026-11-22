#ifndef ROUTE_H
#define ROUTE_H

#include <string>
#include <vector>

// One planned collection trip
class Route {
private:
    std::string vehicleId;
    std::vector<std::string> path;          // every location travelled through
    std::vector<std::string> binIds;        // bins picked up on the way
    std::vector<std::string> binLocations;  // where those bins are
    double totalDistance;                   // km
    double estimatedTime;                   // minutes
    double totalWaste;                      // kg
    bool reRouted;
    std::string reRouteReason;

public:
    Route();
    Route(std::string vehicleId,
          std::vector<std::string> path,
          std::vector<std::string> binIds,
          std::vector<std::string> binLocations,
          double totalDistance,
          double totalWaste,
          bool reRouted = false,
          std::string reRouteReason = "");

    std::string getVehicleId() const { return vehicleId; }
    const std::vector<std::string>& getPath() const { return path; }
    const std::vector<std::string>& getBinIds() const { return binIds; }
    const std::vector<std::string>& getBinLocations() const { return binLocations; }
    double getTotalDistance() const { return totalDistance; }
    double getEstimatedTime() const { return estimatedTime; }
    double getTotalWaste() const { return totalWaste; }
    bool isReRouted() const { return reRouted; }
    std::string getReRouteReason() const { return reRouteReason; }

    void setReRouted(bool value, std::string reason);

    // True if this route drives on the road between the two locations
    bool usesRoad(std::string from, std::string to) const;

    void displaySummary() const;
    void displayPath() const;
};

#endif
