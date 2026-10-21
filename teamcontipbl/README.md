# Intelligent Waste Collection and Dynamic Route Optimization System

> **Academic College Project**  
> **Course Topics Covered:** Data Structures in C/C++, Object-Oriented Programming (OOP) in C++, File Handling in C++  
> **Target Audience:** College Lab Demonstrations, End-Semester Project Submissions, Practical Viva Examinations

---

## 1. Executive Summary & Objective

Traditional municipal waste collection operates on **static, fixed routes**: collection trucks traverse every single street and visit every bin regardless of whether a bin is 10% full or overflowing. This traditional method leads to:
1. Significant waste of vehicle fuel and driver hours.
2. Delayed collection of overflowing bins in high-density areas.
3. Inability to adapt when roads become blocked due to maintenance, traffic, or water-logging.

### Project Objective:
> *"Minimize unnecessary collection travel while ensuring timely, proactive pickup of high-priority and overflowing bins through intelligent prioritization, graph-based shortest path routing, dynamic road re-routing, and file persistence."*

---

## 2. Technologies & Core Academic Concepts

### A. Data Structures (visible & explicitly implemented)
| Data Structure | Implementation File | Academic Purpose in System |
| :--- | :--- | :--- |
| **Array / Vector** | `include/DataStructures.h` | Fast indexed access to fixed vehicle pools, binary heap representation, and search buffers. |
| **Singly Linked List** | `include/DataStructures.h` (`LinkedList<T>`) | Dynamic node-based memory allocation for bin inventory and collection history without pre-allocating static limits. |
| **Priority Queue (Max-Heap)** | `include/DataStructures.h` (`MaxHeapPriorityQueue<T>`) | Arranges urgent bins so that CRITICAL ($\ge 90\%$) and HIGH ($\ge 75\%$) bins are extracted first in $O(\log N)$ time. |
| **Queue (FIFO)** | `include/DataStructures.h` (`Queue<T>`) | First-In-First-Out processing of routine collection requests and dispatch job orders. |
| **Graph (Adjacency List)** | `include/Graph.h`, `src/Graph.cpp` | Models campus / city road networks where nodes represent locations and edges represent roads with distance weights (km). |
| **Dijkstra's Algorithm** | `src/Graph.cpp` | Computes the true shortest / least-cost route between locations and bypasses blocked roads. |
| **Searching Algorithms** | `include/DataStructures.h` | **Linear Search** for partial location substring matching and **Binary Search** ($O(\log N)$) for sorted ID lookups. |
| **Sorting Algorithms** | `include/DataStructures.h` | **QuickSort** ($O(N \log N)$) and **BubbleSort** to reorder bins dynamically by fill percentage or urgency score. |

### B. Object-Oriented Programming (OOP in C++)
- **Encapsulation:** Private data members in `Bin`, `Vehicle`, `Graph`, and `Route` accessed strictly through getters, setters, and operational methods.
- **Inheritance:** Base class `Vehicle` inherited by `StandardCompactorTruck` (with hydraulic compaction factor) and `ElectricMiniTruck` (zero-emission campus vehicle).
- **Polymorphism:** Virtual methods (`getEffectiveCapacity()`, `displayTableRow()`, `getTypeDescription()`) dynamically resolved at runtime.
- **Composition:** `WasteManager` combines `LinkedList<Bin>`, `std::vector<Vehicle*>`, `Graph`, and `FileHandler` into an orchestration engine.

### C. File Handling (No DBMS)
- Uses `std::ifstream`, `std::ofstream`, and `std::fstream`.
- Persistent CSV text files in `data/`:
  - `bins.txt`
  - `vehicles.txt`
  - `roads.txt`
  - `collections.txt`
- **Zero Database Engines:** Absolutely no MySQL, SQLite, or MongoDB required.

---

## 3. Project Directory Structure

```
SmartWaste/
│
├── include/
│   ├── Common.h            # Enums (PriorityLevel), ANSI terminal colors, utilities
│   ├── DataStructures.h    # Custom LinkedList, MaxHeap PriorityQueue, FIFO Queue, Sort/Search
│   ├── Bin.h               # Bin entity class declaration
│   ├── Vehicle.h           # Vehicle base class and derived Compactor/Electric classes
│   ├── Graph.h             # Road network Graph (Adjacency List + Dijkstra)
│   ├── Route.h             # Route encapsulation, step-by-step path, metrics
│   ├── FileHandler.h       # C++ file I/O operations (ifstream/ofstream)
│   ├── WasteManager.h      # Central Orchestrator & API facade
│   └── TestSuite.h         # Automated 12 academic test cases
│
├── src/
│   ├── Bin.cpp             # Bin logic & priority calculation
│   ├── Vehicle.cpp         # Vehicle implementations & compaction logic
│   ├── Graph.cpp           # Adjacency list & Dijkstra shortest path
│   ├── Route.cpp           # Tour formatting and step-by-step display
│   ├── WasteManager.cpp    # Smart optimization, dynamic re-routing, comparison
│   └── main.cpp            # Interactive ANSI Dashboard & CLI options
│
├── data/
│   ├── bins.txt            # Persistent bin records
│   ├── vehicles.txt        # Persistent vehicle records
│   ├── roads.txt           # Persistent road connections
│   └── collections.txt     # Audit history of completed collection trips
│
├── web/                    # Modern local UI dashboard
│   ├── index.html          # Clean smart city dashboard layout
│   ├── style.css           # Modern dark mode aesthetics
│   └── app.js              # HTML5 Canvas graph visualizer & API controller
│
├── server.py               # Zero-dependency local web bridge (Python stdlib http.server)
├── build.bat               # 1-Click compiler script for Windows (g++)
├── run_console.bat         # Launch C++ interactive terminal dashboard
├── run_demo.bat            # Launch ready-to-demo scenario (Section 26)
├── run_tests.bat           # Run 12 academic test cases
├── run_web.bat             # Launch local browser GUI (http://localhost:5000)
├── Makefile                # Standard makefile for cross-platform compilation
└── README.md               # Complete project documentation & Viva guide
```

---

## 4. Compilation & Running Instructions

### Prerequisites
- Any modern C++ compiler supporting C++17:
  - **Windows:** MinGW-w64 / MSYS2 `g++` (already tested with `g++ 16.2.0`)
  - **Linux / Mac:** `g++` or `clang++`
- Optional: Python 3.8+ (only needed if running the web GUI; console runs 100% pure C++).

### Option 1: 1-Click Batch Files (Recommended on Windows)
- **Compile:** Double-click `build.bat`
- **Interactive Terminal:** Double-click `run_console.bat`
- **Automated Demo Scenario:** Double-click `run_demo.bat`
- **Academic Test Suite:** Double-click `run_tests.bat`
- **Modern Web Dashboard:** Double-click `run_web.bat` (opens `http://localhost:5000` in your default browser)

### Option 2: Manual Terminal Commands
```powershell
# Compile the project:
g++ -std=c++17 -Wall -Wextra -Iinclude src/Bin.cpp src/Vehicle.cpp src/Graph.cpp src/Route.cpp src/WasteManager.cpp src/main.cpp -o SmartWaste.exe

# Run Interactive Terminal Dashboard:
.\SmartWaste.exe

# Run Ready-to-Demo Scenario (Section 26):
.\SmartWaste.exe --demo

# Run 12 Academic Unit Tests:
.\SmartWaste.exe --test

# Launch Web Dashboard:
python server.py
```

---

## 5. System Architecture & Project Modules

```
                    +---------------------------+
                    |    BIN & VEHICLE INPUT    |
                    +-------------+-------------+
                                  |
                                  v
                    +---------------------------+
                    |  Calculate Bin Priority   |
                    | (Fill %, Rate, Threshold) |
                    +-------------+-------------+
                                  |
                                  v
                    +---------------------------+
                    | Priority Queue (Max-Heap) |
                    | Extracts Urgent Bins First|
                    +-------------+-------------+
                                  |
                                  v
                    +---------------------------+
                    | Check Vehicle Capacity &  |
                    | Availability Constraints  |
                    +-------------+-------------+
                                  |
                                  v
                    +---------------------------+
                    | Road Network Graph (Adj)  |
                    | Nodes: Stops | Edges: Km  |
                    +-------------+-------------+
                                  |
                                  v
                    +---------------------------+
                    | Dijkstra Shortest Path    |
                    | Generates Multi-Stop Tour |
                    +-------------+-------------+
                                  |
                +-----------------+-----------------+
                |                                   |
                v (Normal)                          v (Road Obstruction)
      +-------------------+               +-------------------+
      | Generate Baseline |               | Dynamic Re-route  |
      |   Smart Route     |               | (Dijkstra Bypass) |
      +---------+---------+               +---------+---------+
                |                                   |
                +-----------------+-----------------+
                                  |
                                  v
                    +---------------------------+
                    | Compare vs Fixed Route    |
                    | (Distance/Time/Fuel Saved)|
                    +-------------+-------------+
                                  |
                                  v
                    +---------------------------+
                    | Execute Collection Trip   |
                    | Mark Bins Collected (0kg) |
                    +-------------+-------------+
                                  |
                                  v
                    +---------------------------+
                    | Save Persistent State to  |
                    | File System (No DBMS)     |
                    +---------------------------+
```

### Module Descriptions
1. **Module 1: Bin Management:** Tracks Bin ID, location, capacity, current waste, fill percentage, fill rate, and collection status. Supports dynamic updates, QuickSort, and binary search.
2. **Module 2: Vehicle Management:** Tracks vehicle fleet, computes effective capacity (including hydraulic compaction for compactor trucks), checks payload limits, and assigns vehicles based on load.
3. **Module 3: Road Network & Graph:** Represents university campus / city topology as an adjacency list graph. Allows blocking and unblocking roads dynamically.
4. **Module 4: Smart Route Planning:** Extracts high-priority bins using a Max-Heap Priority Queue, solves multi-stop collection tour using Dijkstra's shortest path algorithm.
5. **Module 5: Dynamic Re-routing:** Detects when a planned route encounters a blocked road (due to construction, maintenance, or waterlogging) and re-computes an optimal alternative bypass.
6. **Module 6: Fixed vs Smart Comparison:** Benchmarks the intelligent system against the traditional municipal fixed-route system across distance, time, fuel, vehicle utilization, and overflow risk.
7. **Module 7: File Handling:** Handles non-DBMS text file persistence for bins, vehicles, road configurations, and collection trip audits.

---

## 6. Mathematical Formulas & Priority Model

### Priority Determination:
Each bin's fill percentage is computed as:
$$\text{Fill Percentage} = \left(\frac{\text{Current Waste}}{\text{Capacity}}\right) \times 100$$

Priority Categories:
- **CRITICAL:** $\ge 90\%$ (Immediate overflow hazard)
- **HIGH:** $75\% \text{ to } 89\%$ (High generation risk)
- **MEDIUM:** $50\% \text{ to } 74\%$ (Moderate fill)
- **LOW:** $< 50\%$ (Routine, non-urgent)

### Urgency Score (Heap Key):
To prevent fast-filling bins from overflowing before collection, the Max-Heap prioritizes bins using an urgency formula:
$$\text{Urgency Score} = \text{Fill Percentage} + (\text{Daily Fill Rate} \times 1.5) + \text{Critical Boost}$$
*(where Critical Boost = 50 if $\text{Fill Percentage} \ge 90\%$)*

---

## 7. Ready-to-Demo Scenario (Section 26 Walkthrough)

When presenting in a college viva or lab evaluation, execute `SmartWaste.exe --demo` or select **Option 10** in the main menu:

1. **Initial Bin States:**
   - `B101`: 90% (CRITICAL) at GEU Gate
   - `B102`: 55% (MEDIUM) at Hostel
   - `B103`: 95% (CRITICAL) at Canteen
   - `B104`: 40% (LOW) at Library
   - `B105`: 80% (HIGH) at Academic Block
2. **Max-Heap Prioritization Order:**
   1. `B103` (95.0% - CRITICAL)
   2. `B108` (92.0% - CRITICAL)
   3. `B101` (90.0% - CRITICAL)
   4. `B106` (85.0% - HIGH)
   5. `B105` (80.0% - HIGH)
3. **Vehicle Assignment & Initial Route:**
   - Truck `V01` assigned (Effective Capacity: 625 kg).
   - Baseline Shortest Path: `Depot -> GEU Gate -> Market -> Hostel -> Canteen -> Academic Block -> Sports Complex -> Parking -> Depot` (**29.50 km**).
4. **Simulate Road Block:**
   - Road `Hostel <-> Canteen` is blocked due to water pipe maintenance.
5. **Dynamic Re-Routing:**
   - System detects obstruction on active route.
   - Recalculates alternate path via `Hostel -> Library -> Canteen`.
   - New Route Distance: **32.50 km** (Impact: +3.0 km bypass).
6. **Comparison vs Traditional Fixed Route:**
   - **Traditional Fixed Route:** 40.6 km | 137 mins | Overflow Risk HIGH
   - **Smart Optimized Route:** 31.5 km | 95 mins | Overflow Risk ZERO
   - **Distance Saved:** **9.1 km (22.4% efficiency improvement)**
   - **Fuel Saved:** **2.55 Liters**

---

## 8. Academic Test Cases (Section 25 Compliance)

All 12 test cases are automated and pass cleanly:

| Test # | Test Description | Condition Tested | Result |
| :---: | :--- | :--- | :---: |
| **1** | Empty bin handling | 0% fill level classified as LOW priority | **PASS** |
| **2** | 50% filled bin | 50% fill level classified as MEDIUM priority | **PASS** |
| **3** | 80% filled bin | 80% fill level classified as HIGH priority | **PASS** |
| **4** | 95% filled bin | 95% fill level classified as CRITICAL priority | **PASS** |
| **5** | Vehicle unavailable | Offline truck (`available=false`) rejects collection assignment | **PASS** |
| **6** | Insufficient capacity | Truck rejects collection when load exceeds effective capacity | **PASS** |
| **7** | Shortest path (Dijkstra) | Dijkstra finds shortest path $A \to B \to C$ (7 km) vs direct $A \to C$ (10 km) | **PASS** |
| **8** | Road blocked in graph | Road marked blocked in adjacency list correctly | **PASS** |
| **9** | Alternative route available | Dynamic re-routing successfully finds bypass via unblocked road | **PASS** |
| **10** | Disconnected graph | System detects when destination is unreachable (-1 km) | **PASS** |
| **11** | Bin collection state | Bin waste reset to 0 kg and status marked "Collected" | **PASS** |
| **12** | File persistence | Data loaded successfully from `data/bins.txt`, `vehicles.txt`, `roads.txt` | **PASS** |

---

## 9. Comprehensive Viva Questions & Answers (Section 27)

### Q1. Why did you use an Array / Vector?
> **Answer:** Arrays provide contiguous memory allocation with $O(1)$ random access by index. In our project, an array-based vector is used to store the vehicle fleet for fast lookup, and as the underlying backing storage for the binary heap in our Priority Queue (where parent-child relationships are computed via index math: $2i+1$ and $2i+2$).

### Q2. Why did you use a Linked List?
> **Answer:** A Linked List allocates memory dynamically node-by-node. Unlike static arrays, bins in a city or university campus are frequently added, removed, or reorganized during runtime. A singly linked list allows dynamic insertions and deletions without requiring array resizing or pre-allocating an arbitrary maximum buffer.

### Q3. Why did you use a Priority Queue? Why a Max-Heap?
> **Answer:** Standard FIFO queues treat all bins equally. However, in waste collection, a 95% overflowing bin near a dining canteen requires immediate pickup before a 30% bin. A Max-Heap ensures that the most urgent bin is always accessible at the root in $O(1)$ time, and extracting the most critical bin takes $O(\log N)$ time, preventing waste overflow hazards.

### Q4. Why did you use a Queue?
> **Answer:** A First-In-First-Out (FIFO) queue is used for dispatching routine maintenance tasks and queuing collection job orders where requests should be serviced in the chronological order in which they were logged.

### Q5. Why did you represent the road network as a Graph?
> **Answer:** Road networks consist of physical junctions (locations) connected by roads of varying lengths. This topological structure cannot be modeled using linear data structures. A Graph naturally models locations as **Vertices ($V$)** and bidirectional roads as **Edges ($E$)**, with edge weights representing physical distance in kilometers.

### Q6. Why did you choose an Adjacency List over an Adjacency Matrix?
> **Answer:** Campus and urban road networks are **sparse graphs** (each intersection typically connects to only 2 to 4 roads, so $|E| \ll |V|^2$). An Adjacency Matrix would waste $O(V^2)$ memory storing zeros for non-existent connections. An Adjacency List consumes only $O(V + E)$ space and allows Dijkstra's algorithm to iterate only over actual existing neighbors.

### Q7. How does Dijkstra's Algorithm work in this project?
> **Answer:** Dijkstra's algorithm finds the shortest path from a source location to all other reachable nodes. It maintains a distance table initialized to $\infty$ (source = 0) and uses a Min-Priority Queue to explore the closest unvisited node. As it relaxes each adjacent edge, our implementation explicitly checks `if (edge.isBlocked) continue;`. If an edge is blocked, it is skipped, ensuring the path never traverses blocked roads.

### Q8. What OOP principles are implemented in this project?
> **Answer:**
> - **Encapsulation:** All attributes (`id`, `capacity`, `currentWaste`, `isBlocked`) are private/protected and modified only through validated member functions.
> - **Inheritance:** `StandardCompactorTruck` and `ElectricMiniTruck` inherit from base class `Vehicle`.
> - **Polymorphism:** `getEffectiveCapacity()` is a virtual method overridden by `StandardCompactorTruck` to account for its 1.25x hydraulic compaction factor.
> - **Composition:** `WasteManager` owns instances of `Graph`, `LinkedList<Bin>`, and `FileHandler`.

### Q9. Why did you use File Handling instead of a DBMS (like MySQL or SQLite)?
> **Answer:**
> 1. To keep the project lightweight, self-contained, and completely free of external dependencies or database server setups.
> 2. To directly demonstrate core curriculum file I/O streams (`std::ifstream`, `std::ofstream`, `std::fstream`), parsing serialized CSV formats, and handling edge cases like comments and carriage returns.

### Q10. How does Dynamic Re-Routing work?
> **Answer:** When an edge $(u, v)$ is marked blocked, the system inspects the current active route's node sequence. If $(u, v)$ appears as a consecutive segment in the planned path, Dijkstra's algorithm is re-executed across the remaining targeted stops with $(u, v)$ disabled. The algorithm automatically finds the next shortest unblocked alternative (e.g. bypassing `Hostel <-> Canteen` via `Library`), updates total distance, and flags the route as re-routed.

### Q11. How is Vehicle Capacity checked?
> **Answer:** Before scheduling a collection, the system sums the waste of the prioritized bins. It queries each vehicle's polymorphic `getEffectiveCapacity()`. If the cumulative payload exceeds the vehicle's remaining capacity, the system caps the trip at maximum load and schedules the remaining bins for subsequent dispatch.

### Q12. How does the Smart Route outperform the Traditional Fixed Route?
> **Answer:** A fixed route forces a vehicle to visit every bin on campus regardless of fill level, travelling over 40 km and wasting fuel on nearly empty bins while critical bins sit overflowing. The Smart Route visits only bins meeting the urgency threshold ($\ge 75\%$), reducing travel distance by over 20-30%, cutting travel time by ~40 minutes, reducing fuel consumption, and completely eliminating overflow hazards.

---

## 10. Conclusion

This project delivers a complete, runnable, and educational system that rigorously applies:
1. **Data Structures:** Arrays, Singly Linked Lists, Max-Heap Priority Queues, FIFO Queues, Adjacency List Graphs, Dijkstra's Algorithm, QuickSort, and Searching.
2. **C++ OOP:** Clean class hierarchies, virtual methods, runtime polymorphism, and encapsulation.
3. **File Handling:** Zero external database engines, persisting all data through readable text files.
4. **Dual Interface:** An ANSI-colored terminal dashboard and a modern browser-based canvas dashboard.
