#include "../include/Common.h"
#include "../include/WasteManager.h"
#include "../include/TestSuite.h"
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
void enableVirtualTerminalProcessing() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(hOut, dwMode);
}
#else
void enableVirtualTerminalProcessing() {}
#endif

// Helper to pause before returning to menu
void pauseScreen() {
    std::cout << "\n" << Color::GRAY << "Press Enter to continue..." << Color::RESET;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Clear screen or separator
void printHeader(const std::string& title) {
    std::cout << "\n" << Color::BOLD << Color::CYAN;
    std::cout << "====================================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "====================================================================\n";
    std::cout << Color::RESET;
}

// Ready-to-Demo Scenario (Section 26)
void runDemoScenario(WasteManager& wm) {
    printHeader("READY-TO-DEMO SCENARIO (Section 26)");
    std::cout << Color::BOLD << "Step 1: Inspecting Initial Bins & Priority Ranking\n" << Color::RESET;
    std::cout << "Required demo initial states:\n";
    std::cout << "  B101 = 90% (CRITICAL)\n";
    std::cout << "  B102 = 55% (MEDIUM)\n";
    std::cout << "  B103 = 95% (CRITICAL)\n";
    std::cout << "  B104 = 40% (LOW)\n";
    std::cout << "  B105 = 80% (HIGH)\n\n";

    // Ensure initial demo states
    wm.updateBinFill("B101", 90.0);
    wm.updateBinFill("B102", 82.5); // 55% of 150
    wm.updateBinFill("B103", 95.0);
    wm.updateBinFill("B104", 32.0); // 40% of 80
    wm.updateBinFill("B105", 96.0); // 80% of 120
    wm.unblockRoad("Hostel", "Canteen"); // Ensure open initially

    std::cout << Color::BOLD << "Step 2: Processing Bins via Priority Queue (Max-Heap)\n" << Color::RESET;
    auto urgentBins = wm.extractUrgentBins(50.0);
    std::cout << "Priority Queue Extraction Order (Urgent First):\n";
    for (size_t i = 0; i < urgentBins.size(); ++i) {
        std::cout << "  #" << (i + 1) << " " << urgentBins[i].getId() << " -> "
                  << urgentBins[i].getLocation() << " ("
                  << std::fixed << std::setprecision(1) << urgentBins[i].getFillPercentage() << "%) -> "
                  << priorityToColorString(urgentBins[i].getPriority()) << "\n";
    }

    std::cout << "\n" << Color::BOLD << "Step 3: Selecting Vehicle V01 & Generating Smart Route\n" << Color::RESET;
    Route route1 = wm.generateSmartRoute("V01", 75.0);
    route1.displaySummary();
    route1.displayStepByStep();

    std::cout << Color::BOLD << "Step 4: Simulating Road Blockage for Dynamic Re-routing\n" << Color::RESET;
    std::cout << "Simulating unexpected road block: " << Color::RED << "Hostel <-> Canteen" << Color::RESET
              << " (Reason: Construction / Pipe Repair)\n";

    wm.triggerDynamicReRoute("Hostel", "Canteen", "Road Construction at Campus Walk");
    Route route2 = wm.getCurrentRoute();
    route2.displaySummary();
    route2.displayStepByStep();

    std::cout << Color::BOLD << "Step 5: Fixed Route vs Smart Route Comparison\n" << Color::RESET;
    wm.displayComparison();

    std::cout << "\n" << Color::GREEN << "Demo Scenario Completed Successfully!" << Color::RESET << "\n";
}

int main(int argc, char* argv[]) {
    enableVirtualTerminalProcessing();
    WasteManager wm("data");

    // Command-line flags
    if (argc > 1) {
        std::string flag = argv[1];
        if (flag == "--test") {
            bool ok = TestSuite::runAllTests();
            return ok ? 0 : 1;
        } else if (flag == "--demo") {
            runDemoScenario(wm);
            return 0;
        } else if (flag == "--json") {
            std::cout << wm.exportStateJSON();
            return 0;
        } else if (flag == "--api") {
            if (argc > 2) {
                std::string action = argv[2];
                if (action == "block" && argc >= 5) {
                    wm.blockRoad(argv[3], argv[4]);
                } else if (action == "unblock" && argc >= 5) {
                    wm.unblockRoad(argv[3], argv[4]);
                } else if (action == "reroute" && argc >= 5) {
                    std::string reason = (argc >= 6) ? argv[5] : "Emergency Block";
                    wm.triggerDynamicReRoute(argv[3], argv[4], reason);
                } else if (action == "smart") {
                    std::string vid = (argc >= 4) ? argv[3] : "V01";
                    wm.generateSmartRoute(vid, 75.0);
                } else if (action == "collect") {
                    wm.executeCurrentRoute();
                } else if (action == "update" && argc >= 5) {
                    wm.updateBinFill(argv[3], std::stod(argv[4]));
                } else if (action == "reset") {
                    wm.initSampleData();
                    wm.saveToFiles();
                }
            }
            std::cout << wm.exportStateJSON();
            return 0;
        }
    }

    // Interactive Console UI
    int choice = -1;
    while (choice != 0) {
        wm.displayDashboard();

        std::cout << Color::BOLD << "MAIN MENU / SYSTEM MODULES:" << Color::RESET << "\n";
        std::cout << "  1. Bin Management           (View, Add, Update Fill, Search, Sort)\n";
        std::cout << "  2. Vehicle Management       (View Fleet, Add, Capacity Check)\n";
        std::cout << "  3. Route Optimization       (Generate Smart Route via Dijkstra)\n";
        std::cout << "  4. Road Network & Graph     (View Adjacency List, Add Road, Pathfinding)\n";
        std::cout << "  5. Dynamic Re-routing       (Simulate Road Blockage & Recalculate)\n";
        std::cout << "  6. Fixed vs Smart Route     (Compare Traditional vs Smart System)\n";
        std::cout << "  7. Collection History Logs  (View History from collections.txt)\n";
        std::cout << "  8. File / Data Management   (Save, Reload, Reset Sample Data)\n";
        std::cout << "  9. Run Academic Test Suite  (12 Automated Test Cases)\n";
        std::cout << " 10. Run Ready-to-Demo Scenario (College Viva Scenario)\n";
        std::cout << "  0. Exit\n\n";
        std::cout << "Enter option (0-10): ";

        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: { // Bin Management
                int sub = -1;
                while (sub != 0) {
                    printHeader("MODULE 1: BIN MANAGEMENT");
                    std::cout << "1. View All Bins\n";
                    std::cout << "2. Add New Bin\n";
                    std::cout << "3. Update Fill Level (Current Waste)\n";
                    std::cout << "4. Search Bin by Location\n";
                    std::cout << "5. Search Bin by ID\n";
                    std::cout << "6. Sort Bins by Fill Level (QuickSort)\n";
                    std::cout << "7. Sort Bins by Urgency Score (Priority Queue Sort)\n";
                    std::cout << "8. Mark Bin as Collected\n";
                    std::cout << "0. Return to Main Menu\n";
                    std::cout << "\nEnter choice: ";
                    if (!(std::cin >> sub)) { std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); continue; }
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    if (sub == 1) {
                        wm.displayAllBins();
                        pauseScreen();
                    } else if (sub == 2) {
                        std::string id, loc;
                        double cap, waste, rate;
                        std::cout << "Enter Bin ID (e.g. B111): "; std::getline(std::cin, id);
                        std::cout << "Enter Location (e.g. Science Park): "; std::getline(std::cin, loc);
                        std::cout << "Enter Capacity in kg (e.g. 100): "; std::cin >> cap;
                        std::cout << "Enter Current Waste in kg (e.g. 85): "; std::cin >> waste;
                        std::cout << "Enter Fill Rate in kg/day (e.g. 6): "; std::cin >> rate;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        wm.addBin(Bin(id, loc, cap, waste, rate));
                        std::cout << Color::GREEN << "Bin " << id << " added successfully!\n" << Color::RESET;
                        pauseScreen();
                    } else if (sub == 3) {
                        std::string id;
                        double waste;
                        std::cout << "Enter Bin ID to update: "; std::getline(std::cin, id);
                        std::cout << "Enter New Current Waste (kg): "; std::cin >> waste;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        if (wm.updateBinFill(id, waste)) {
                            std::cout << Color::GREEN << "Bin " << id << " updated successfully!\n" << Color::RESET;
                        } else {
                            std::cout << Color::RED << "Bin ID not found!\n" << Color::RESET;
                        }
                        pauseScreen();
                    } else if (sub == 4) {
                        std::string loc;
                        std::cout << "Enter location search term: "; std::getline(std::cin, loc);
                        auto found = wm.searchBinsByLocation(loc);
                        std::cout << "Found " << found.size() << " matching bins:\n";
                        for (const auto& b : found) b.displayTableRow();
                        pauseScreen();
                    } else if (sub == 5) {
                        std::string id;
                        std::cout << "Enter Bin ID: "; std::getline(std::cin, id);
                        Bin* b = wm.findBinById(id);
                        if (b) b->displayCard();
                        else std::cout << Color::RED << "Bin not found!\n" << Color::RESET;
                        pauseScreen();
                    } else if (sub == 6) {
                        wm.sortBinsByFillPercentage(false);
                        std::cout << Color::GREEN << "Bins sorted by Fill % (Descending):\n" << Color::RESET;
                        wm.displayAllBins();
                        pauseScreen();
                    } else if (sub == 7) {
                        wm.sortBinsByPriority(false);
                        std::cout << Color::GREEN << "Bins sorted by Priority/Urgency Score:\n" << Color::RESET;
                        wm.displayAllBins();
                        pauseScreen();
                    } else if (sub == 8) {
                        std::string id;
                        std::cout << "Enter Bin ID to mark collected: "; std::getline(std::cin, id);
                        if (wm.markBinCollected(id)) {
                            std::cout << Color::GREEN << "Bin marked as collected!\n" << Color::RESET;
                        } else {
                            std::cout << Color::RED << "Bin not found!\n" << Color::RESET;
                        }
                        pauseScreen();
                    }
                }
                break;
            }

            case 2: { // Vehicle Management
                printHeader("MODULE 2: VEHICLE MANAGEMENT");
                wm.displayAllVehicles();
                std::cout << "\n1. Check Vehicle Assignment for Given Load\n";
                std::cout << "2. Add New Vehicle\n";
                std::cout << "3. Reset Vehicle Loads\n";
                std::cout << "0. Back\n";
                std::cout << "\nEnter choice: ";
                int vsub;
                if (std::cin >> vsub) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    if (vsub == 1) {
                        double req;
                        std::cout << "Enter required waste capacity (kg): "; std::cin >> req;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        Vehicle* v = wm.selectSuitableVehicle(req);
                        if (v) {
                            std::cout << Color::GREEN << "Recommended Vehicle: " << v->getId()
                                      << " (" << v->getTypeDescription() << ") with "
                                      << v->getRemainingCapacity() << " kg remaining capacity.\n" << Color::RESET;
                        } else {
                            std::cout << Color::RED << "No available vehicle has sufficient capacity!\n" << Color::RESET;
                        }
                        pauseScreen();
                    } else if (vsub == 2) {
                        std::string id;
                        int typeChoice;
                        double cap;
                        std::cout << "Enter Vehicle ID: "; std::getline(std::cin, id);
                        std::cout << "Type (1=Compactor Truck, 2=Electric Mini-Truck, 3=Standard Truck): "; std::cin >> typeChoice;
                        std::cout << "Enter Capacity (kg): "; std::cin >> cap;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        if (typeChoice == 1) {
                            wm.addVehicle(new StandardCompactorTruck(id, cap));
                        } else if (typeChoice == 2) {
                            wm.addVehicle(new ElectricMiniTruck(id, cap));
                        } else {
                            wm.addVehicle(new Vehicle(id, "StandardTruck", cap));
                        }
                        std::cout << Color::GREEN << "Vehicle " << id << " added successfully!\n" << Color::RESET;
                        pauseScreen();
                    } else if (vsub == 3) {
                        for (auto v : wm.getVehicles()) v->resetLoad();
                        std::cout << Color::GREEN << "All vehicle loads reset to 0 kg.\n" << Color::RESET;
                        pauseScreen();
                    }
                }
                break;
            }

            case 3: { // Route Optimization
                printHeader("MODULE 3: SMART ROUTE OPTIMIZATION (DIJKSTRA + PRIORITY QUEUE)");
                std::cout << "1. Generate Smart Route for Priority Bins (>= 75% High/Critical)\n";
                std::cout << "2. Generate Smart Route for Moderate Bins (>= 50% Medium/High/Critical)\n";
                std::cout << "3. Execute Current Collection Trip (Pick up waste)\n";
                std::cout << "0. Back\n";
                std::cout << "\nEnter choice: ";
                int rsub;
                if (std::cin >> rsub) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    if (rsub == 1 || rsub == 2) {
                        double thresh = (rsub == 1) ? 75.0 : 50.0;
                        Route r = wm.generateSmartRoute("V01", thresh);
                        r.displaySummary();
                        r.displayStepByStep();
                        pauseScreen();
                    } else if (rsub == 3) {
                        if (wm.executeCurrentRoute()) {
                            std::cout << Color::GREEN << "Trip executed successfully! Targeted bins marked as collected.\n"
                                      << "Collection logged to data/collections.txt\n" << Color::RESET;
                        } else {
                            std::cout << Color::YELLOW << "No active route to execute. Generate a route first!\n" << Color::RESET;
                        }
                        pauseScreen();
                    }
                }
                break;
            }

            case 4: { // Road Network & Graph
                printHeader("MODULE 4: ROAD NETWORK & GRAPH PATHFINDING");
                wm.displayRoadNetwork();
                std::cout << "\n1. Test Dijkstra Shortest Path Between Two Locations\n";
                std::cout << "2. Add New Road Connection\n";
                std::cout << "0. Back\n";
                std::cout << "\nEnter choice: ";
                int gsub;
                if (std::cin >> gsub) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    if (gsub == 1) {
                        std::string src, dest;
                        std::cout << "Enter Source Location: "; std::getline(std::cin, src);
                        std::cout << "Enter Destination Location: "; std::getline(std::cin, dest);
                        auto res = wm.getGraph().dijkstra(src, dest);
                        if (res.first >= 0) {
                            std::cout << Color::GREEN << "Shortest Path Found: " << res.first << " km\n" << Color::RESET;
                            std::cout << "Route: ";
                            for (size_t i = 0; i < res.second.size(); ++i) {
                                std::cout << res.second[i] << (i + 1 < res.second.size() ? " -> " : "");
                            }
                            std::cout << "\n";
                        } else {
                            std::cout << Color::RED << "No path found or location does not exist!\n" << Color::RESET;
                        }
                        pauseScreen();
                    } else if (gsub == 2) {
                        std::string u, v, rname;
                        double dist;
                        std::cout << "Enter Location 1: "; std::getline(std::cin, u);
                        std::cout << "Enter Location 2: "; std::getline(std::cin, v);
                        std::cout << "Enter Distance (km): "; std::cin >> dist;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "Enter Road Name: "; std::getline(std::cin, rname);
                        wm.addRoad(u, v, dist, rname);
                        std::cout << Color::GREEN << "Road added successfully!\n" << Color::RESET;
                        pauseScreen();
                    }
                }
                break;
            }

            case 5: { // Dynamic Re-routing
                printHeader("MODULE 5: DYNAMIC RE-ROUTING");
                auto blocked = wm.getGraph().getBlockedRoads();
                std::cout << "Currently Blocked Roads (" << blocked.size() << "):\n";
                for (const auto& b : blocked) {
                    std::cout << "  - " << Color::RED << b.first << " <---> " << b.second << Color::RESET << "\n";
                }

                std::cout << "\n1. Block a Road & Recalculate Active Route\n";
                std::cout << "2. Unblock a Road\n";
                std::cout << "0. Back\n";
                std::cout << "\nEnter choice: ";
                int dsub;
                if (std::cin >> dsub) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    if (dsub == 1) {
                        std::string u, v, reason;
                        std::cout << "Enter Location 1 (e.g. Hostel): "; std::getline(std::cin, u);
                        std::cout << "Enter Location 2 (e.g. Canteen): "; std::getline(std::cin, v);
                        std::cout << "Enter Reason (e.g. Water Logging / Repair): "; std::getline(std::cin, reason);
                        if (reason.empty()) reason = "Road Blocked";

                        bool rerouted = wm.triggerDynamicReRoute(u, v, reason);
                        if (rerouted) {
                            std::cout << Color::GREEN << "Dynamic re-routing successful! Route recalculated.\n" << Color::RESET;
                            wm.getCurrentRoute().displaySummary();
                            wm.getCurrentRoute().displayStepByStep();
                        } else {
                            std::cout << Color::YELLOW << "Road " << u << " <-> " << v
                                      << " is now blocked, but was not part of the active route.\n" << Color::RESET;
                        }
                        pauseScreen();
                    } else if (dsub == 2) {
                        std::string u, v;
                        std::cout << "Enter Location 1 to unblock: "; std::getline(std::cin, u);
                        std::cout << "Enter Location 2 to unblock: "; std::getline(std::cin, v);
                        wm.unblockRoad(u, v);
                        std::cout << Color::GREEN << "Road unblocked successfully!\n" << Color::RESET;
                        pauseScreen();
                    }
                }
                break;
            }

            case 6: { // Fixed vs Smart Comparison
                printHeader("MODULE 6: FIXED ROUTE VS SMART ROUTE COMPARISON");
                wm.displayComparison();
                pauseScreen();
                break;
            }

            case 7: { // Collection History
                printHeader("MODULE 7: COLLECTION HISTORY & AUDIT LOGS");
                auto hist = FileHandler::loadCollectionHistory("data/collections.txt");
                if (hist.empty()) {
                    std::cout << "No past collections logged yet.\n";
                } else {
                    for (const auto& line : hist) {
                        std::cout << "  " << line << "\n";
                    }
                }
                pauseScreen();
                break;
            }

            case 8: { // File Management
                printHeader("MODULE 8: FILE & DATA PERSISTENCE");
                std::cout << "1. Save All Current Data to Text Files\n";
                std::cout << "2. Reload Data from Text Files\n";
                std::cout << "3. Reset to Default College Demo Scenario\n";
                std::cout << "0. Back\n";
                std::cout << "\nEnter choice: ";
                int fsub;
                if (std::cin >> fsub) {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    if (fsub == 1) {
                        wm.saveToFiles();
                        std::cout << Color::GREEN << "Data successfully saved to data/bins.txt, vehicles.txt, roads.txt\n" << Color::RESET;
                        pauseScreen();
                    } else if (fsub == 2) {
                        wm.loadFromFiles();
                        std::cout << Color::GREEN << "Data reloaded successfully from files.\n" << Color::RESET;
                        pauseScreen();
                    } else if (fsub == 3) {
                        wm.initSampleData();
                        wm.saveToFiles();
                        std::cout << Color::GREEN << "Reset to default college sample scenario complete.\n" << Color::RESET;
                        pauseScreen();
                    }
                }
                break;
            }

            case 9: { // Test Suite
                TestSuite::runAllTests();
                pauseScreen();
                break;
            }

            case 10: { // Ready-to-Demo Scenario
                runDemoScenario(wm);
                pauseScreen();
                break;
            }

            case 0:
                std::cout << "\nExiting Intelligent Waste Management System. Goodbye!\n";
                break;

            default:
                std::cout << Color::RED << "Invalid option! Please select 0 to 10.\n" << Color::RESET;
                pauseScreen();
                break;
        }
    }

    return 0;
}
