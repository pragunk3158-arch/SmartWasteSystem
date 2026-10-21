#include "../include/Route.h"
#include <iostream>
#include <iomanip>

Route::Route()
    : vehicleId(""), totalDistance(0.0), estimatedTimeMinutes(0.0),
      totalWasteCollected(0.0), isReRouted(false), reRouteReason("") {}

Route::Route(const std::string &vehicleId,
             const std::vector<std::string> &pathNodes,
             const std::vector<std::string> &binIds,
             const std::vector<std::string> &binLocations,
             double totalDistance,
             double totalWasteCollected,
             bool isReRouted,
             const std::string &reRouteReason)
    : vehicleId(vehicleId), pathNodes(pathNodes), binIds(binIds),
      binLocations(binLocations), totalDistance(totalDistance),
      totalWasteCollected(totalWasteCollected), isReRouted(isReRouted),
      reRouteReason(reRouteReason)
{

    // Urban collection speed model:
    // 25 km/h driving speed -> 2.4 minutes per km
    // + 4 minutes collection handling time per bin
    estimatedTimeMinutes = (totalDistance / 25.0 * 60.0) + (binIds.size() * 4.0);
}

void Route::setReRouted(bool reRouted, const std::string &reason)
{
    isReRouted = reRouted;
    reRouteReason = reason;
}

void Route::displaySummary() const
{
    std::cout << "\n------------------------------------------------------\n";
    std::cout << "               COLLECTION ROUTE SUMMARY               \n";
    std::cout << "------------------------------------------------------\n";
    std::cout << "Assigned Vehicle:      " << Color::CYAN << vehicleId << Color::RESET << "\n";
    std::cout << "Total Distance:        " << Color::GREEN << std::fixed << std::setprecision(2)
              << totalDistance << " km" << Color::RESET << "\n";
    std::cout << "Estimated Duration:    " << std::fixed << std::setprecision(1)
              << estimatedTimeMinutes << " mins\n";
    std::cout << "Bins Targeted (" << binIds.size() << "):     ";
    for (size_t i = 0; i < binIds.size(); ++i)
    {
        std::cout << binIds[i] << (i + 1 < binIds.size() ? ", " : "");
    }
    std::cout << "\n";
    std::cout << "Total Waste to Pick:   " << totalWasteCollected << " kg\n";
    if (isReRouted)
    {
        std::cout << "Status:                " << Color::YELLOW << "[DYNAMIC RE-ROUTE ACTIVE]"
                  << Color::RESET << "\n";
        std::cout << "Reason:                " << reRouteReason << "\n";
    }
    else
    {
        std::cout << "Status:                " << Color::GREEN << "[NORMAL OPTIMIZED ROUTE]"
                  << Color::RESET << "\n";
    }
    std::cout << "------------------------------------------------------\n";
}

void Route::displayStepByStep() const
{
    if (pathNodes.empty())
    {
        std::cout << Color::RED << "No path available to display." << Color::RESET << "\n";
        return;
    }

    std::cout << "\n"
              << Color::BOLD << "PATH TRAVERSED:" << Color::RESET << "\n\n";
    for (size_t i = 0; i < pathNodes.size(); ++i)
    {
        bool isBin = false;
        std::string matchingBin = "";
        for (size_t b = 0; b < binLocations.size(); ++b)
        {
            if (binLocations[b] == pathNodes[i])
            {
                isBin = true;
                matchingBin = binIds[b];
                break;
            }
        }

        if (i == 0 || i == pathNodes.size() - 1)
        {
            std::cout << "  " << Color::MAGENTA << "[" << pathNodes[i] << "]" << Color::RESET;
        }
        else if (isBin)
        {
            std::cout << "  " << Color::YELLOW << "[" << pathNodes[i] << " - Bin " << matchingBin << "]" << Color::RESET;
        }
        else
        {
            std::cout << "  " << Color::WHITE << pathNodes[i] << Color::RESET;
        }

        if (i + 1 < pathNodes.size())
        {
            std::cout << "\n    " << Color::CYAN << "↓" << Color::RESET << "\n";
        }
    }
    std::cout << "\n\n";
}
