#ifndef TEST_SUITE_H
#define TEST_SUITE_H

#include "WasteManager.h"
#include <iostream>
#include <vector>
#include <string>

// ============================================================================
// TEST SUITE: 12 ACADEMIC TEST CASES (From Section 25)
// ============================================================================
class TestSuite {
public:
    static bool runAllTests() {
        int passed = 0;
        int total = 12;

        std::cout << "\n======================================================\n";
        std::cout << "        RUNNING 12 ACADEMIC RIGOR TEST CASES          \n";
        std::cout << "======================================================\n\n";

        // Test 1: Empty bin (0% fill)
        {
            Bin b("TEST_01", "Lab", 100.0, 0.0);
            if (b.getFillPercentage() == 0.0 && b.getPriority() == PriorityLevel::LOW) {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 1: Empty bin handled correctly (0% -> LOW)\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 1: Empty bin failed\n";
            }
        }

        // Test 2: 50% filled bin
        {
            Bin b("TEST_02", "Lab", 100.0, 50.0);
            if (b.getFillPercentage() == 50.0 && b.getPriority() == PriorityLevel::MEDIUM) {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 2: 50% filled bin classified as MEDIUM priority\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 2: 50% filled bin classification failed\n";
            }
        }

        // Test 3: 80% filled bin
        {
            Bin b("TEST_03", "Lab", 100.0, 80.0);
            if (b.getFillPercentage() == 80.0 && b.getPriority() == PriorityLevel::HIGH) {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 3: 80% filled bin classified as HIGH priority\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 3: 80% filled bin classification failed\n";
            }
        }

        // Test 4: 95% filled bin
        {
            Bin b("TEST_04", "Lab", 100.0, 95.0);
            if (b.getFillPercentage() == 95.0 && b.getPriority() == PriorityLevel::CRITICAL) {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 4: 95% filled bin classified as CRITICAL priority\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 4: 95% filled bin classification failed\n";
            }
        }

        // Test 5: Vehicle unavailable
        {
            Vehicle v("TEST_V1", "Truck", 500.0, 0.0, false);
            if (!v.canAccommodate(50.0)) {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 5: Unavailable vehicle rejects collection assignment\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 5: Unavailable vehicle failed\n";
            }
        }

        // Test 6: Vehicle capacity insufficient
        {
            Vehicle v("TEST_V2", "Truck", 200.0, 180.0, true);
            if (!v.canAccommodate(50.0)) { // 180 + 50 = 230 > 200
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 6: Insufficient capacity properly detected and rejected\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 6: Capacity overflow not caught\n";
            }
        }

        // Test 7: Shortest path available via Dijkstra
        {
            Graph g;
            g.addRoad("A", "B", 4.0);
            g.addRoad("B", "C", 3.0);
            g.addRoad("A", "C", 10.0);
            auto path = g.dijkstra("A", "C");
            if (path.first == 7.0 && path.second.size() == 3 && path.second[1] == "B") {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 7: Dijkstra finds optimal shortest path (A->B->C = 7 km)\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 7: Dijkstra shortest path calculation failed\n";
            }
        }

        // Test 8: Road blocked properly in graph
        {
            Graph g;
            g.addRoad("A", "B", 4.0);
            g.blockRoad("A", "B");
            if (g.isRoadBlocked("A", "B")) {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 8: Road blocking correctly registered\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 8: Road blocking failed\n";
            }
        }

        // Test 9: Alternative route available when primary road is blocked
        {
            Graph g;
            g.addRoad("A", "B", 2.0);
            g.addRoad("B", "C", 2.0);
            g.addRoad("A", "D", 3.0);
            g.addRoad("D", "C", 3.0);
            // Normal path is A->B->C (dist 4)
            g.blockRoad("B", "C");
            auto alt = g.dijkstra("A", "C");
            if (alt.first == 6.0 && alt.second.size() == 3 && alt.second[1] == "D") {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 9: Dynamic re-routing found alternative path via D (6 km)\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 9: Alternative route recalculation failed\n";
            }
        }

        // Test 10: No alternative route when network is isolated
        {
            Graph g;
            g.addRoad("A", "B", 5.0);
            g.blockRoad("A", "B");
            auto res = g.dijkstra("A", "B");
            if (res.first < 0 && res.second.empty()) {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 10: Disconnected graph condition properly identified\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 10: Disconnected graph detection failed\n";
            }
        }

        // Test 11: All bins collected marks currentWaste=0 and status=Collected
        {
            Bin b("TEST_COLLECT", "Gym", 100.0, 90.0);
            b.markCollected();
            if (b.getCurrentWaste() == 0.0 && b.getStatus() == "Collected" && b.getPriority() == PriorityLevel::LOW) {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 11: Bin collection state update verified (Waste=0, Collected)\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 11: Bin collection mark failed\n";
            }
        }

        // Test 12: File data successfully loaded
        {
            WasteManager wm("data");
            if (wm.getTotalBins() >= 10 && wm.getVehicles().size() >= 4 && wm.getGraph().getLocationCount() >= 10) {
                std::cout << Color::GREEN << "[PASS]" << Color::RESET << " Test 12: File persistence & data loading verified ("
                          << wm.getTotalBins() << " bins, " << wm.getVehicles().size() << " vehicles)\n";
                passed++;
            } else {
                std::cout << Color::RED << "[FAIL]" << Color::RESET << " Test 12: File data loading failed\n";
            }
        }

        std::cout << "\n------------------------------------------------------\n";
        std::cout << "TEST RESULTS: " << (passed == total ? Color::GREEN : Color::YELLOW)
                  << passed << "/" << total << " Tests Passed" << Color::RESET << "\n";
        std::cout << "======================================================\n\n";

        return (passed == total);
    }
};

#endif // TEST_SUITE_H
