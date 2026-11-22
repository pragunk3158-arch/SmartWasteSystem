#include "../include/Route.h"
#include <cstdio>

using namespace std;

Route::Route() {
    vehicleId = "";
    totalDistance = 0;
    estimatedTime = 0;
    totalWaste = 0;
    reRouted = false;
    reRouteReason = "";
}

Route::Route(string vehicleId, vector<string> path, vector<string> binIds,
             vector<string> binLocations, double totalDistance,
             double totalWaste, bool reRouted, string reRouteReason) {
    this->vehicleId = vehicleId;
    this->path = path;
    this->binIds = binIds;
    this->binLocations = binLocations;
    this->totalDistance = totalDistance;
    this->totalWaste = totalWaste;
    this->reRouted = reRouted;
    this->reRouteReason = reRouteReason;

    // Time model: the truck moves at 25 km/h in city traffic and needs
    // about 4 minutes to empty one bin.
    estimatedTime = (totalDistance / 25.0) * 60.0 + binIds.size() * 4.0;
}

void Route::setReRouted(bool value, string reason) {
    reRouted = value;
    reRouteReason = reason;
}

// True if this route drives on the road between the two locations
bool Route::usesRoad(string from, string to) const {
    for (int i = 0; i + 1 < (int)path.size(); i++) {
        if (path[i] == from && path[i + 1] == to) return true;
        if (path[i] == to && path[i + 1] == from) return true;
    }
    return false;
}

void Route::displaySummary() const {
    printf("\n------------------------------------------------------\n");
    printf("  TRUCK %s\n", vehicleId.c_str());
    printf("------------------------------------------------------\n");
    printf("Total Distance   : %.2f km\n", totalDistance);
    printf("Estimated Time   : %.1f mins\n", estimatedTime);
    printf("Waste to Collect : %.1f kg\n", totalWaste);

    printf("Bins (%d)         : ", (int)binIds.size());
    for (int i = 0; i < (int)binIds.size(); i++) {
        printf("%s", binIds[i].c_str());
        if (i + 1 < (int)binIds.size()) printf(", ");
    }
    printf("\n");

    if (reRouted)
        printf("Status           : RE-ROUTED (%s)\n", reRouteReason.c_str());
    else
        printf("Status           : NORMAL OPTIMIZED ROUTE\n");

    printf("------------------------------------------------------\n");
}

void Route::displayPath() const {
    if (path.empty()) {
        printf("No path to show.\n");
        return;
    }

    printf("PATH: ");
    for (int i = 0; i < (int)path.size(); i++) {
        printf("%s", path[i].c_str());

        // mark the location if a bin is collected there
        for (int j = 0; j < (int)binLocations.size(); j++) {
            if (binLocations[j] == path[i]) {
                printf("(%s)", binIds[j].c_str());
                break;
            }
        }
        if (i + 1 < (int)path.size()) printf(" -> ");
    }
    printf("\n");
}
