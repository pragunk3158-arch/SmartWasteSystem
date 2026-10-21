#include "../include/WasteManager.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <ctime>

WasteManager::WasteManager(const std::string& dataDir)
    : depotLocation("Depot"), dataDirectory(dataDir),
      totalDistanceSavedLifetime(0.0), completedTripsCount(0) {
    if (!loadFromFiles()) {
        initSampleData();
        saveToFiles();
    }
}

WasteManager::~WasteManager() {
    for (auto v : vehicles) {
        delete v;
    }
    vehicles.clear();
}

void WasteManager::initSampleData() {
    // Clear existing
    bins.clear();
    for (auto v : vehicles) delete v;
    vehicles.clear();

    // 1. Initialize Sample Bins (10 bins across campus/city)
    // Matches sample scenario in Section 20 & 26
    bins.insertTail(Bin("B101", "GEU Gate",         100.0, 90.0, 8.0,  "Pending")); // 90%  CRITICAL
    bins.insertTail(Bin("B102", "Hostel",           150.0, 82.5, 6.0,  "Pending")); // 55%  MEDIUM
    bins.insertTail(Bin("B103", "Canteen",          100.0, 95.0, 12.0, "Pending")); // 95%  CRITICAL
    bins.insertTail(Bin("B104", "Library",           80.0, 32.0, 3.0,  "Pending")); // 40%  LOW
    bins.insertTail(Bin("B105", "Academic Block",   120.0, 96.0, 7.0,  "Pending")); // 80%  HIGH
    bins.insertTail(Bin("B106", "Sports Complex",   100.0, 85.0, 9.0,  "Pending")); // 85%  HIGH
    bins.insertTail(Bin("B107", "Parking",           80.0, 24.0, 2.0,  "Pending")); // 30%  LOW
    bins.insertTail(Bin("B108", "Market",           150.0, 138.0, 14.0, "Pending")); // 92%  CRITICAL
    bins.insertTail(Bin("B109", "Main Road",        100.0, 60.0, 5.0,  "Pending")); // 60%  MEDIUM
    bins.insertTail(Bin("B110", "Residential Area", 120.0, 54.0, 4.0,  "Pending")); // 45%  LOW

    // 2. Initialize Sample Vehicles (4 vehicles)
    vehicles.push_back(new StandardCompactorTruck("V01", 500.0, 0.0, true, "Depot", "Available", 1.25)); // effective 625 kg
    vehicles.push_back(new ElectricMiniTruck("V02", 300.0, 0.0, true, "Depot", "Available", 100));     // effective 300 kg
    vehicles.push_back(new StandardCompactorTruck("V03", 800.0, 0.0, true, "Depot", "Available", 1.25)); // effective 1000 kg
    vehicles.push_back(new Vehicle("V04", "StandardTruck", 400.0, 0.0, true, "Depot", "Available"));   // effective 400 kg

    // 3. Initialize Road Network Graph (11 nodes, 15+ edges)
    // Locations
    roadGraph.addLocation("Depot");
    roadGraph.addLocation("GEU Gate");
    roadGraph.addLocation("Hostel");
    roadGraph.addLocation("Canteen");
    roadGraph.addLocation("Library");
    roadGraph.addLocation("Parking");
    roadGraph.addLocation("Academic Block");
    roadGraph.addLocation("Sports Complex");
    roadGraph.addLocation("Market");
    roadGraph.addLocation("Main Road");
    roadGraph.addLocation("Residential Area");

    // Road connections with weights in km
    roadGraph.addRoad("Depot", "GEU Gate", 3.5, "North Link");
    roadGraph.addRoad("Depot", "Main Road", 4.0, "Central Blvd");
    roadGraph.addRoad("Depot", "Parking", 2.8, "Service Lane");
    roadGraph.addRoad("GEU Gate", "Hostel", 2.5, "University Ave");
    roadGraph.addRoad("GEU Gate", "Market", 3.0, "Commerce Way");
    roadGraph.addRoad("Hostel", "Canteen", 2.0, "Campus Walk");          // Key edge for blocking test
    roadGraph.addRoad("Hostel", "Library", 3.2, "West Pathway");         // Alternative bypass
    roadGraph.addRoad("Canteen", "Academic Block", 2.2, "Science Walk");
    roadGraph.addRoad("Canteen", "Library", 1.8, "Central Corridor");
    roadGraph.addRoad("Library", "Academic Block", 2.5, "Faculty Road");
    roadGraph.addRoad("Academic Block", "Sports Complex", 3.0, "Athletics Way");
    roadGraph.addRoad("Academic Block", "Parking", 4.5, "South Perimeter");
    roadGraph.addRoad("Sports Complex", "Residential Area", 3.8, "Ring Road");
    roadGraph.addRoad("Market", "Main Road", 2.4, "Market Express");
    roadGraph.addRoad("Main Road", "Residential Area", 4.2, "South Avenue");
}

bool WasteManager::loadFromFiles() {
    bool ok1 = FileHandler::loadBins(dataDirectory + "/bins.txt", bins);
    bool ok2 = FileHandler::loadVehicles(dataDirectory + "/vehicles.txt", vehicles);
    bool ok3 = FileHandler::loadRoads(dataDirectory + "/roads.txt", roadGraph);
    return (ok1 && ok2 && ok3);
}

bool WasteManager::saveToFiles() {
    bool ok1 = FileHandler::saveBins(dataDirectory + "/bins.txt", bins);
    bool ok2 = FileHandler::saveVehicles(dataDirectory + "/vehicles.txt", vehicles);
    bool ok3 = FileHandler::saveRoads(dataDirectory + "/roads.txt", roadGraph);
    return (ok1 && ok2 && ok3);
}

// ============================================================================
// MODULE 1: BIN MANAGEMENT
// ============================================================================
void WasteManager::addBin(const Bin& bin) {
    bins.insertTail(bin);
    saveToFiles();
}

bool WasteManager::updateBinFill(const std::string& binId, double currentWaste) {
    Bin* b = findBinById(binId);
    if (b) {
        b->setCurrentWaste(currentWaste);
        b->setStatus("Pending");
        saveToFiles();
        return true;
    }
    return false;
}

bool WasteManager::markBinCollected(const std::string& binId) {
    Bin* b = findBinById(binId);
    if (b) {
        b->markCollected();
        saveToFiles();
        return true;
    }
    return false;
}

Bin* WasteManager::findBinById(const std::string& binId) {
    return bins.find([&binId](const Bin& b) {
        return b.getId() == binId;
    });
}

std::vector<Bin> WasteManager::searchBinsByLocation(const std::string& locSubstr) {
    std::vector<Bin> results;
    bins.forEach([&](const Bin& b) {
        if (b.getLocation().find(locSubstr) != std::string::npos) {
            results.push_back(b);
        }
    });
    return results;
}

void WasteManager::sortBinsByFillPercentage(bool ascending) {
    std::vector<Bin> vec = bins.toVector();
    Algorithms::quickSort(vec, [ascending](const Bin& a, const Bin& b) {
        return ascending ? (a.getFillPercentage() < b.getFillPercentage())
                         : (a.getFillPercentage() > b.getFillPercentage());
    });
    bins.clear();
    for (const auto& b : vec) bins.insertTail(b);
}

void WasteManager::sortBinsByPriority(bool ascending) {
    std::vector<Bin> vec = bins.toVector();
    Algorithms::quickSort(vec, [ascending](const Bin& a, const Bin& b) {
        return ascending ? (a.getUrgencyScore() < b.getUrgencyScore())
                         : (a.getUrgencyScore() > b.getUrgencyScore());
    });
    bins.clear();
    for (const auto& b : vec) bins.insertTail(b);
}

void WasteManager::displayAllBins() const {
    std::cout << "\n===========================================================================================\n";
    std::cout << "                                    BIN INVENTORY STATUS                                    \n";
    std::cout << "===========================================================================================\n";
    std::cout << "| ID    | Location         | Capacity | Current | Fill % | Priority            | Status     |\n";
    std::cout << "-------------------------------------------------------------------------------------------\n";
    bins.forEach([](const Bin& b) {
        b.displayTableRow();
    });
    std::cout << "===========================================================================================\n";
}

std::vector<Bin> WasteManager::getAllBinsAsVector() const {
    return bins.toVector();
}

// ============================================================================
// MODULE 2: PRIORITY QUEUE EXTRACTION
// ============================================================================
std::vector<Bin> WasteManager::extractUrgentBins(double minFillThreshold) {
    MaxHeapPriorityQueue<Bin> pq;

    // Push all eligible bins into MaxHeap Priority Queue
    bins.forEach([&](const Bin& b) {
        if (b.getStatus() == "Pending" && b.getFillPercentage() >= minFillThreshold) {
            pq.push(b);
        }
    });

    std::vector<Bin> prioritized;
    while (!pq.isEmpty()) {
        prioritized.push_back(pq.pop());
    }
    return prioritized;
}

// ============================================================================
// MODULE 3: VEHICLE MANAGEMENT
// ============================================================================
void WasteManager::addVehicle(Vehicle* v) {
    vehicles.push_back(v);
    saveToFiles();
}

Vehicle* WasteManager::findVehicleById(const std::string& vId) {
    for (auto v : vehicles) {
        if (v->getId() == vId) return v;
    }
    return nullptr;
}

Vehicle* WasteManager::selectSuitableVehicle(double requiredCapacity) {
    Vehicle* bestVehicle = nullptr;
    double bestFitCapacity = 1e9;

    for (auto v : vehicles) {
        if (v->isAvailable() && v->getRemainingCapacity() >= requiredCapacity) {
            // Pick vehicle that best fits the load without excessive waste of capacity
            if (v->getRemainingCapacity() < bestFitCapacity) {
                bestFitCapacity = v->getRemainingCapacity();
                bestVehicle = v;
            }
        }
    }

    // Fallback: Pick any available vehicle with highest remaining capacity
    if (!bestVehicle) {
        double maxCap = 0;
        for (auto v : vehicles) {
            if (v->isAvailable() && v->getRemainingCapacity() > maxCap) {
                maxCap = v->getRemainingCapacity();
                bestVehicle = v;
            }
        }
    }

    return bestVehicle;
}

void WasteManager::displayAllVehicles() const {
    std::cout << "\n========================================================================================================\n";
    std::cout << "                                       VEHICLE FLEET MANAGEMENT                                         \n";
    std::cout << "========================================================================================================\n";
    std::cout << "| ID    | Type              | Capacity | Current | Rem. Cap | Available | Location      | Status       |\n";
    std::cout << "--------------------------------------------------------------------------------------------------------\n";
    for (const auto v : vehicles) {
        v->displayTableRow();
    }
    std::cout << "========================================================================================================\n";
}

// ============================================================================
// MODULE 4: ROAD NETWORK & GRAPH
// ============================================================================
void WasteManager::addLocation(const std::string& loc) {
    roadGraph.addLocation(loc);
    saveToFiles();
}

void WasteManager::addRoad(const std::string& u, const std::string& v, double dist, const std::string& name) {
    roadGraph.addRoad(u, v, dist, name, true);
    saveToFiles();
}

bool WasteManager::blockRoad(const std::string& u, const std::string& v) {
    bool res = roadGraph.blockRoad(u, v);
    if (res) saveToFiles();
    return res;
}

bool WasteManager::unblockRoad(const std::string& u, const std::string& v) {
    bool res = roadGraph.unblockRoad(u, v);
    if (res) saveToFiles();
    return res;
}

void WasteManager::displayRoadNetwork() const {
    roadGraph.displayGraph();
}

// ============================================================================
// MODULE 5: SMART ROUTE PLANNING (DIJKSTRA + PRIORITY QUEUE)
// ============================================================================
Route WasteManager::generateSmartRoute(const std::string& vehicleId, double threshold) {
    // 1. Get urgent bins using Priority Queue
    std::vector<Bin> urgentBins = extractUrgentBins(threshold);
    if (urgentBins.empty()) {
        // If no bins meet threshold, take any pending bins >= 50%
        urgentBins = extractUrgentBins(50.0);
    }
    if (urgentBins.empty()) {
        // Fallback: take all pending bins
        urgentBins = extractUrgentBins(0.0);
    }

    if (urgentBins.empty()) {
        currentActiveRoute = Route();
        return currentActiveRoute;
    }

    // 2. Select Vehicle
    Vehicle* selectedVehicle = nullptr;
    if (!vehicleId.empty()) {
        selectedVehicle = findVehicleById(vehicleId);
    }
    if (!selectedVehicle) {
        double totalUrgentWaste = 0;
        for (const auto& b : urgentBins) totalUrgentWaste += b.getCurrentWaste();
        selectedVehicle = selectSuitableVehicle(totalUrgentWaste);
    }

    std::string vId = selectedVehicle ? selectedVehicle->getId() : "V01";
    double availableCap = selectedVehicle ? selectedVehicle->getRemainingCapacity() : 500.0;

    // 3. Select bins that fit vehicle capacity
    std::vector<Bin> binsToCollect;
    double currentLoadAccum = 0.0;
    for (const auto& b : urgentBins) {
        if (currentLoadAccum + b.getCurrentWaste() <= availableCap) {
            binsToCollect.push_back(b);
            currentLoadAccum += b.getCurrentWaste();
        }
    }

    if (binsToCollect.empty() && !urgentBins.empty()) {
        // Take at least the most urgent bin if space is very tight
        binsToCollect.push_back(urgentBins[0]);
        currentLoadAccum = urgentBins[0].getCurrentWaste();
    }

    // 4. Greedy Nearest-Neighbor Tour using Dijkstra Shortest Paths
    std::string currentLoc = depotLocation;
    std::vector<bool> visited(binsToCollect.size(), false);
    std::vector<std::string> orderedStops;
    orderedStops.push_back(depotLocation);

    std::vector<std::string> orderedBinIds;
    std::vector<std::string> orderedBinLocs;

    for (size_t step = 0; step < binsToCollect.size(); ++step) {
        int bestIdx = -1;
        double bestDist = 1e9;

        for (size_t i = 0; i < binsToCollect.size(); ++i) {
            if (!visited[i]) {
                auto dij = roadGraph.dijkstra(currentLoc, binsToCollect[i].getLocation());
                if (dij.first >= 0 && dij.first < bestDist) {
                    bestDist = dij.first;
                    bestIdx = static_cast<int>(i);
                }
            }
        }

        if (bestIdx != -1) {
            visited[bestIdx] = true;
            currentLoc = binsToCollect[bestIdx].getLocation();
            orderedStops.push_back(currentLoc);
            orderedBinIds.push_back(binsToCollect[bestIdx].getId());
            orderedBinLocs.push_back(binsToCollect[bestIdx].getLocation());
        }
    }

    // Return to Depot
    orderedStops.push_back(depotLocation);

    // Calculate full node-by-node path and distance
    auto tourResult = roadGraph.calculateTour(orderedStops);

    currentActiveRoute = Route(vId, tourResult.second, orderedBinIds,
                               orderedBinLocs, tourResult.first, currentLoadAccum);

    return currentActiveRoute;
}

// Traditional Fixed Route:
// Visits ALL bin locations in a fixed predetermined sequence
Route WasteManager::generateFixedRoute(const std::string& vehicleId) {
    std::vector<std::string> fixedStops = {
        "Depot", "GEU Gate", "Hostel", "Canteen", "Library",
        "Academic Block", "Sports Complex", "Residential Area",
        "Main Road", "Market", "Parking", "Depot"
    };

    auto tourResult = roadGraph.calculateTour(fixedStops);

    std::vector<std::string> allBinIds;
    std::vector<std::string> allBinLocs;
    double totalWaste = 0.0;

    bins.forEach([&](const Bin& b) {
        allBinIds.push_back(b.getId());
        allBinLocs.push_back(b.getLocation());
        totalWaste += b.getCurrentWaste();
    });

    std::string vId = vehicleId.empty() ? "V01" : vehicleId;
    lastFixedRoute = Route(vId, tourResult.second, allBinIds, allBinLocs,
                           tourResult.first, totalWaste);

    return lastFixedRoute;
}

// ============================================================================
// MODULE 6: DYNAMIC RE-ROUTING
// ============================================================================
bool WasteManager::triggerDynamicReRoute(const std::string& u, const std::string& v, const std::string& reason) {
    // 1. Block the road in graph
    blockRoad(u, v);

    // 2. Check if current active route uses this blocked road
    const auto& path = currentActiveRoute.getPathNodes();
    bool routeAffected = false;

    if (path.size() >= 2) {
        for (size_t i = 0; i < path.size() - 1; ++i) {
            if ((path[i] == u && path[i + 1] == v) || (path[i] == v && path[i + 1] == u)) {
                routeAffected = true;
                break;
            }
        }
    }

    if (!routeAffected) {
        // Active route does not traverse this road, but road is now blocked in graph
        return false;
    }

    // 3. Recalculate route for the same targeted bins bypassing the blocked edge
    std::vector<std::string> targetStops;
    targetStops.push_back(depotLocation);
    for (const auto& loc : currentActiveRoute.getBinLocations()) {
        targetStops.push_back(loc);
    }
    targetStops.push_back(depotLocation);

    auto newTour = roadGraph.calculateTour(targetStops);

    if (newTour.first < 0) {
        // No alternative path available!
        currentActiveRoute.setReRouted(true, "NO ALTERNATIVE PATH - ROAD " + u + "-" + v + " BLOCKED");
        return true;
    }

    double oldDistance = currentActiveRoute.getTotalDistance();
    currentActiveRoute = Route(currentActiveRoute.getVehicleId(),
                               newTour.second,
                               currentActiveRoute.getBinIds(),
                               currentActiveRoute.getBinLocations(),
                               newTour.first,
                               currentActiveRoute.getTotalWasteCollected(),
                               true,
                               reason + " (" + u + " <-> " + v + ")");

    std::cout << "\n" << Color::YELLOW << ">>> DYNAMIC RE-ROUTING TRIGGERED <<<" << Color::RESET << "\n";
    std::cout << "Original Distance: " << oldDistance << " km\n";
    std::cout << "New Route Distance: " << newTour.first << " km\n";
    std::cout << "Impact: " << (newTour.first - oldDistance >= 0 ? "+" : "")
              << (newTour.first - oldDistance) << " km\n";

    return true;
}

// ============================================================================
// MODULE 7: FIXED VS SMART COMPARISON
// ============================================================================
RouteComparisonResult WasteManager::compareRoutes() {
    Route fixed = generateFixedRoute("V01");
    Route smart = generateSmartRoute("V01", 75.0);

    RouteComparisonResult res;
    res.fixedDistance = fixed.getTotalDistance();
    res.fixedTimeMinutes = fixed.getEstimatedTimeMinutes();
    res.fixedBinsCount = static_cast<int>(bins.size());
    res.fixedWasteCollected = fixed.getTotalWasteCollected();
    res.fixedUtilizationPercent = (res.fixedWasteCollected / 625.0) * 100.0;
    if (res.fixedUtilizationPercent > 100.0) res.fixedUtilizationPercent = 100.0;
    res.fixedOverflowRisk = "High (delay in reaching critical bins due to fixed order)";

    res.smartDistance = smart.getTotalDistance();
    res.smartTimeMinutes = smart.getEstimatedTimeMinutes();
    res.smartBinsCount = static_cast<int>(smart.getBinIds().size());
    res.smartWasteCollected = smart.getTotalWasteCollected();
    res.smartUtilizationPercent = (res.smartWasteCollected / 625.0) * 100.0;
    if (res.smartUtilizationPercent > 100.0) res.smartUtilizationPercent = 100.0;
    res.smartOverflowRisk = "Zero (Critical & High bins prioritized first via Max-Heap)";

    res.distanceSaved = res.fixedDistance - res.smartDistance;
    res.timeSavedMinutes = res.fixedTimeMinutes - res.smartTimeMinutes;
    res.fuelSavedLiters = res.distanceSaved * 0.28; // 0.28 L/km for compactor truck
    res.efficiencyGainPercent = (res.distanceSaved / res.fixedDistance) * 100.0;

    totalDistanceSavedLifetime = res.distanceSaved;
    return res;
}

void WasteManager::displayComparison() const {
    RouteComparisonResult res = const_cast<WasteManager*>(this)->compareRoutes();

    std::cout << "\n====================================================================================\n";
    std::cout << "               TRADITIONAL FIXED ROUTE vs SMART OPTIMIZED ROUTE                     \n";
    std::cout << "====================================================================================\n";
    std::cout << "| Metric                     | Fixed Route (Traditional) | Smart Route (Our System) |\n";
    std::cout << "------------------------------------------------------------------------------------\n";
    std::cout << "| Total Travel Distance      | " << std::left << std::setw(26)
              << (std::to_string((int)res.fixedDistance) + "." + std::to_string((int)((res.fixedDistance - (int)res.fixedDistance)*10)) + " km")
              << "| " << Color::GREEN << std::setw(24)
              << (std::to_string((int)res.smartDistance) + "." + std::to_string((int)((res.smartDistance - (int)res.smartDistance)*10)) + " km")
              << Color::RESET << " |\n";

    std::cout << "| Estimated Travel Time      | " << std::left << std::setw(26)
              << (std::to_string((int)res.fixedTimeMinutes) + " mins")
              << "| " << Color::GREEN << std::setw(24)
              << (std::to_string((int)res.smartTimeMinutes) + " mins")
              << Color::RESET << " |\n";

    std::cout << "| Bins Targeted              | " << std::left << std::setw(26)
              << (std::to_string(res.fixedBinsCount) + " (all bins)")
              << "| " << std::setw(25)
              << (std::to_string(res.smartBinsCount) + " (urgent only)")
              << "|\n";

    std::cout << "| Waste Collected            | " << std::left << std::setw(26)
              << (std::to_string((int)res.fixedWasteCollected) + " kg")
              << "| " << std::setw(25)
              << (std::to_string((int)res.smartWasteCollected) + " kg")
              << "|\n";

    std::cout << "| Vehicle Space Utilization  | " << std::left << std::setw(26)
              << (std::to_string((int)res.fixedUtilizationPercent) + "%")
              << "| " << std::setw(25)
              << (std::to_string((int)res.smartUtilizationPercent) + "%")
              << "|\n";

    std::cout << "| Overflow Risk              | " << std::left << std::setw(26)
              << (Color::RED + "HIGH (Fixed Order)" + Color::RESET)
              << "| " << std::setw(34)
              << (Color::GREEN + "ZERO (Prioritized First)" + Color::RESET)
              << "|\n";

    std::cout << "====================================================================================\n";
    std::cout << Color::BOLD << Color::GREEN
              << "  >>> TOTAL DISTANCE SAVED: " << std::fixed << std::setprecision(1) << res.distanceSaved << " km\n"
              << "  >>> TOTAL TIME SAVED:     " << std::fixed << std::setprecision(1) << res.timeSavedMinutes << " mins\n"
              << "  >>> FUEL SAVED:           " << std::fixed << std::setprecision(2) << res.fuelSavedLiters << " Liters\n"
              << "  >>> EFFICIENCY GAIN:      " << std::fixed << std::setprecision(1) << res.efficiencyGainPercent << "%\n"
              << Color::RESET;
    std::cout << "====================================================================================\n";
}

// ============================================================================
// MODULE 8: TRIP EXECUTION & PERSISTENCE
// ============================================================================
bool WasteManager::executeCurrentRoute() {
    if (currentActiveRoute.getBinIds().empty()) {
        return false;
    }

    Vehicle* v = findVehicleById(currentActiveRoute.getVehicleId());
    if (v) {
        v->loadWaste(currentActiveRoute.getTotalWasteCollected());
        v->setStatus("Completed");
    }

    // Mark targeted bins as collected
    for (const auto& id : currentActiveRoute.getBinIds()) {
        markBinCollected(id);
    }

    // Log to collections.txt
    std::time_t now = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", std::localtime(&now));

    std::ostringstream logEntry;
    logEntry << buf << " | Vehicle: " << currentActiveRoute.getVehicleId()
             << " | Distance: " << currentActiveRoute.getTotalDistance() << " km"
             << " | Waste: " << currentActiveRoute.getTotalWasteCollected() << " kg"
             << " | Bins: ";
    for (const auto& id : currentActiveRoute.getBinIds()) logEntry << id << " ";

    FileHandler::logCollection(dataDirectory + "/collections.txt", logEntry.str());

    completedTripsCount++;
    saveToFiles();
    return true;
}

// ============================================================================
// MODULE 9: DASHBOARD METRICS
// ============================================================================
int WasteManager::getTotalBins() const {
    return bins.size();
}

int WasteManager::getCriticalBinsCount() const {
    int c = 0;
    bins.forEach([&c](const Bin& b) {
        if (b.getPriority() == PriorityLevel::CRITICAL && b.getStatus() == "Pending") c++;
    });
    return c;
}

int WasteManager::getHighBinsCount() const {
    int c = 0;
    bins.forEach([&c](const Bin& b) {
        if (b.getPriority() == PriorityLevel::HIGH && b.getStatus() == "Pending") c++;
    });
    return c;
}

int WasteManager::getAvailableVehiclesCount() const {
    int c = 0;
    for (auto v : vehicles) {
        if (v->isAvailable()) c++;
    }
    return c;
}

double WasteManager::getTotalWasteWaiting() const {
    double w = 0.0;
    bins.forEach([&w](const Bin& b) {
        if (b.getStatus() == "Pending") w += b.getCurrentWaste();
    });
    return w;
}

void WasteManager::displayDashboard() const {
    std::cout << "\n" << Color::BOLD << Color::CYAN;
    std::cout << "====================================================================\n";
    std::cout << "    INTELLIGENT WASTE COLLECTION & ROUTE OPTIMIZATION DASHBOARD     \n";
    std::cout << "====================================================================\n" << Color::RESET;

    std::cout << "+-----------------------+-----------------------+------------------+\n";
    std::cout << "| " << Color::BOLD << "Total Bins: " << std::left << std::setw(10) << getTotalBins() << Color::RESET
              << "| " << Color::RED << "Critical (>=90%): " << std::left << std::setw(4) << getCriticalBinsCount() << Color::RESET
              << "| " << Color::YELLOW << "High (75-89%): " << std::left << std::setw(3) << getHighBinsCount() << Color::RESET << "|\n";
    std::cout << "+-----------------------+-----------------------+------------------+\n";
    std::cout << "| " << Color::GREEN << "Available Trucks: " << std::left << std::setw(4) << getAvailableVehiclesCount() << Color::RESET
              << "| " << "Pending Waste: " << std::fixed << std::setprecision(0) << getTotalWasteWaiting() << " kg "
              << "| " << Color::GREEN << "Saved: " << std::fixed << std::setprecision(1) << totalDistanceSavedLifetime << " km" << Color::RESET << "   |\n";
    std::cout << "+-----------------------+-----------------------+------------------+\n";

    if (!currentActiveRoute.getPathNodes().empty()) {
        std::cout << "\n" << Color::BOLD << "Active Optimized Route:" << Color::RESET << " "
                  << currentActiveRoute.getVehicleId() << " ("
                  << std::fixed << std::setprecision(1) << currentActiveRoute.getTotalDistance() << " km, "
                  << currentActiveRoute.getBinIds().size() << " bins)\n";
        if (currentActiveRoute.getIsReRouted()) {
            std::cout << "  " << Color::YELLOW << "Status: " << currentActiveRoute.getReRouteReason() << Color::RESET << "\n";
        }
    }
    std::cout << "\n";
}

// ============================================================================
// JSON EXPORT FOR WEB DASHBOARD
// ============================================================================
std::string WasteManager::exportStateJSON() const {
    std::ostringstream ss;
    ss << "{\n";

    // Dashboard metrics
    ss << "  \"metrics\": {\n";
    ss << "    \"totalBins\": " << getTotalBins() << ",\n";
    ss << "    \"criticalBins\": " << getCriticalBinsCount() << ",\n";
    ss << "    \"highBins\": " << getHighBinsCount() << ",\n";
    ss << "    \"availableVehicles\": " << getAvailableVehiclesCount() << ",\n";
    ss << "    \"pendingWaste\": " << getTotalWasteWaiting() << ",\n";
    ss << "    \"distanceSaved\": " << totalDistanceSavedLifetime << ",\n";
    ss << "    \"completedTrips\": " << completedTripsCount << "\n";
    ss << "  },\n";

    // Bins array
    ss << "  \"bins\": [\n";
    auto binVec = bins.toVector();
    for (size_t i = 0; i < binVec.size(); ++i) {
        const auto& b = binVec[i];
        ss << "    {\n";
        ss << "      \"id\": \"" << b.getId() << "\",\n";
        ss << "      \"location\": \"" << b.getLocation() << "\",\n";
        ss << "      \"capacity\": " << b.getCapacity() << ",\n";
        ss << "      \"currentWaste\": " << b.getCurrentWaste() << ",\n";
        ss << "      \"fillPercentage\": " << b.getFillPercentage() << ",\n";
        ss << "      \"fillRate\": " << b.getFillRate() << ",\n";
        ss << "      \"priority\": \"" << priorityToString(b.getPriority()) << "\",\n";
        ss << "      \"status\": \"" << b.getStatus() << "\"\n";
        ss << "    }" << (i + 1 < binVec.size() ? "," : "") << "\n";
    }
    ss << "  ],\n";

    // Vehicles array
    ss << "  \"vehicles\": [\n";
    for (size_t i = 0; i < vehicles.size(); ++i) {
        const auto v = vehicles[i];
        ss << "    {\n";
        ss << "      \"id\": \"" << v->getId() << "\",\n";
        ss << "      \"type\": \"" << v->getTypeDescription() << "\",\n";
        ss << "      \"capacity\": " << v->getCapacity() << ",\n";
        ss << "      \"effectiveCapacity\": " << v->getEffectiveCapacity() << ",\n";
        ss << "      \"currentLoad\": " << v->getCurrentLoad() << ",\n";
        ss << "      \"available\": " << (v->isAvailable() ? "true" : "false") << ",\n";
        ss << "      \"location\": \"" << v->getCurrentLocation() << "\",\n";
        ss << "      \"status\": \"" << v->getStatus() << "\"\n";
        ss << "    }" << (i + 1 < vehicles.size() ? "," : "") << "\n";
    }
    ss << "  ],\n";

    // Road Graph
    ss << "  \"roads\": [\n";
    auto adj = roadGraph.getAdjList();
    bool firstRoad = true;
    for (const auto& pair : adj) {
        for (const auto& edge : pair.second) {
            if (pair.first < edge.destination) {
                if (!firstRoad) ss << ",\n";
                firstRoad = false;
                ss << "    {\n";
                ss << "      \"from\": \"" << pair.first << "\",\n";
                ss << "      \"to\": \"" << edge.destination << "\",\n";
                ss << "      \"distance\": " << edge.distance << ",\n";
                ss << "      \"isBlocked\": " << (edge.isBlocked ? "true" : "false") << ",\n";
                ss << "      \"name\": \"" << edge.roadName << "\"\n";
                ss << "    }";
            }
        }
    }
    ss << "\n  ],\n";

    // Active Route
    ss << "  \"activeRoute\": {\n";
    ss << "    \"vehicleId\": \"" << currentActiveRoute.getVehicleId() << "\",\n";
    ss << "    \"totalDistance\": " << currentActiveRoute.getTotalDistance() << ",\n";
    ss << "    \"estimatedTimeMinutes\": " << currentActiveRoute.getEstimatedTimeMinutes() << ",\n";
    ss << "    \"totalWaste\": " << currentActiveRoute.getTotalWasteCollected() << ",\n";
    ss << "    \"isReRouted\": " << (currentActiveRoute.getIsReRouted() ? "true" : "false") << ",\n";
    ss << "    \"reRouteReason\": \"" << currentActiveRoute.getReRouteReason() << "\",\n";
    ss << "    \"path\": [";
    const auto& p = currentActiveRoute.getPathNodes();
    for (size_t i = 0; i < p.size(); ++i) {
        ss << "\"" << p[i] << "\"" << (i + 1 < p.size() ? ", " : "");
    }
    ss << "],\n";
    ss << "    \"bins\": [";
    const auto& bIds = currentActiveRoute.getBinIds();
    for (size_t i = 0; i < bIds.size(); ++i) {
        ss << "\"" << bIds[i] << "\"" << (i + 1 < bIds.size() ? ", " : "");
    }
    ss << "]\n";
    ss << "  }\n";

    ss << "}\n";
    return ss.str();
}
