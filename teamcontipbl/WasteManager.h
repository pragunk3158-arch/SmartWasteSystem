#ifndef WASTE_MANAGER_H
#define WASTE_MANAGER_H

#include "Common.h"
#include "Bin.h"
#include "Vehicle.h"
#include "Graph.h"
#include "Route.h"
#include "DataStructures.h"
#include "FileHandler.h"
#include <string>
#include <vector>
#include <memory>

// ============================================================================
// STRUCT: RouteComparisonResult
// Holds metrics for Fixed vs Smart route comparison
// ============================================================================
struct RouteComparisonResult {
    double fixedDistance;
    double fixedTimeMinutes;
    int fixedBinsCount;
    double fixedWasteCollected;
    double fixedUtilizationPercent;
    std::string fixedOverflowRisk;

    double smartDistance;
    double smartTimeMinutes;
    int smartBinsCount;
    double smartWasteCollected;
    double smartUtilizationPercent;
    std::string smartOverflowRisk;

    double distanceSaved;
    double timeSavedMinutes;
    double fuelSavedLiters;
    double efficiencyGainPercent;
};

// ============================================================================
// CLASS: WasteManager
// System Orchestrator / Facade
// Demonstrates:
// - Composition of Data Structures (LinkedList, MaxHeap PriorityQueue, Graph, Queue)
// - OOP architecture and encapsulation
// - Route optimization with Dijkstra
// - Dynamic Re-routing
// - File persistence
// ============================================================================
class WasteManager {
private:
    LinkedList<Bin> bins;
    std::vector<Vehicle*> vehicles;
    Graph roadGraph;
    Queue<std::string> dispatchQueue; // FIFO Queue for pending dispatch requests
    std::string depotLocation;
    std::string dataDirectory;

    Route currentActiveRoute;
    Route lastFixedRoute;
    double totalDistanceSavedLifetime;
    int completedTripsCount;

public:
    WasteManager(const std::string& dataDir = "data");
    ~WasteManager();

    // Initialization & Data Loading
    void initSampleData();
    bool loadFromFiles();
    bool saveToFiles();

    // Module 1: Bin Management
    void addBin(const Bin& bin);
    bool updateBinFill(const std::string& binId, double currentWaste);
    bool markBinCollected(const std::string& binId);
    Bin* findBinById(const std::string& binId);
    std::vector<Bin> searchBinsByLocation(const std::string& locSubstr);
    void sortBinsByFillPercentage(bool ascending = false);
    void sortBinsByPriority(bool ascending = false);
    void displayAllBins() const;
    std::vector<Bin> getAllBinsAsVector() const;

    // Module 2: Priority Queue Urgency Sorting
    // Uses MaxHeapPriorityQueue to extract urgent bins
    std::vector<Bin> extractUrgentBins(double minFillThreshold = 75.0);

    // Module 3: Vehicle Management
    void addVehicle(Vehicle* v);
    Vehicle* findVehicleById(const std::string& vId);
    Vehicle* selectSuitableVehicle(double requiredCapacity);
    void displayAllVehicles() const;
    const std::vector<Vehicle*>& getVehicles() const { return vehicles; }

    // Module 4: Road Network & Graph
    void addLocation(const std::string& loc);
    void addRoad(const std::string& u, const std::string& v, double dist, const std::string& name = "");
    bool blockRoad(const std::string& u, const std::string& v);
    bool unblockRoad(const std::string& u, const std::string& v);
    void displayRoadNetwork() const;
    const Graph& getGraph() const { return roadGraph; }

    // Module 5: Smart Route Planning & Dijkstra Optimization
    Route generateSmartRoute(const std::string& vehicleId = "", double threshold = 75.0);
    Route generateFixedRoute(const std::string& vehicleId = "");

    // Module 6: Dynamic Re-routing
    // Blocks a road and recalculates current active route if affected
    bool triggerDynamicReRoute(const std::string& u, const std::string& v, const std::string& reason = "Road Blocked / Maintenance");

    // Module 7: Fixed vs Smart Comparison
    RouteComparisonResult compareRoutes();
    void displayComparison() const;

    // Module 8: Trip Execution
    bool executeCurrentRoute();

    // Module 9: Dashboard & Metrics
    int getTotalBins() const;
    int getCriticalBinsCount() const;
    int getHighBinsCount() const;
    int getAvailableVehiclesCount() const;
    double getTotalWasteWaiting() const;
    double getTotalDistanceSaved() const { return totalDistanceSavedLifetime; }
    int getCompletedTripsCount() const { return completedTripsCount; }
    void displayDashboard() const;

    // Active route getters
    const Route& getCurrentRoute() const { return currentActiveRoute; }
    const Route& getLastFixedRoute() const { return lastFixedRoute; }

    // JSON API output for Web UI
    std::string exportStateJSON() const;
};

#endif // WASTE_MANAGER_H
