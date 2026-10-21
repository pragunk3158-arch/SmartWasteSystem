#ifndef GRAPH_H
#define GRAPH_H

#include "Common.h"
#include <string>
#include <vector>
#include <map>
#include <utility>

// ============================================================================
// STRUCT: Edge
// Represents a road between two locations with distance and blocked status
// ============================================================================
struct Edge {
    std::string destination;
    double distance;    // in kilometers (weight)
    bool isBlocked;     // true if road is blocked/under construction
    std::string roadName;

    Edge(const std::string& dest, double dist, bool blocked = false, const std::string& name = "")
        : destination(dest), distance(dist), isBlocked(blocked), roadName(name) {}
};

// ============================================================================
// CLASS: Graph
// Demonstrates:
// - Graph representation using Adjacency List
// - Dijkstra's Shortest Path Algorithm
// - Dynamic edge blocking / unblocking for dynamic re-routing
// ============================================================================
class Graph {
private:
    // Adjacency List: Map from location name to vector of outgoing edges
    std::map<std::string, std::vector<Edge>> adjList;

public:
    Graph();

    // Graph Construction
    void addLocation(const std::string& location);
    void addRoad(const std::string& from, const std::string& to, double distance,
                 const std::string& roadName = "", bool bidirectional = true);

    // Road Condition Control (Dynamic Re-routing)
    bool blockRoad(const std::string& from, const std::string& to);
    bool unblockRoad(const std::string& from, const std::string& to);
    bool isRoadBlocked(const std::string& from, const std::string& to) const;
    std::vector<std::pair<std::string, std::string>> getBlockedRoads() const;

    // Dijkstra's Shortest Path Algorithm
    // Returns pair of: (total distance, vector of location names in path order)
    // If no path exists, returns (-1.0, empty vector)
    std::pair<double, std::vector<std::string>> dijkstra(
        const std::string& start, const std::string& destination) const;

    // Multi-stop route calculation: visits stops in order using Dijkstra
    // Returns (total distance, complete expanded node path)
    std::pair<double, std::vector<std::string>> calculateTour(
        const std::vector<std::string>& stops) const;

    // Inspection
    bool hasLocation(const std::string& location) const;
    std::vector<std::string> getAllLocations() const;
    const std::map<std::string, std::vector<Edge>>& getAdjList() const { return adjList; }
    int getLocationCount() const { return static_cast<int>(adjList.size()); }
    int getRoadCount() const;

    // Display
    void displayGraph() const;

    // Serialization
    std::vector<std::string> toCSVLines() const;
    static void loadRoadLine(const std::string& line, Graph& g);
};

#endif // GRAPH_H
