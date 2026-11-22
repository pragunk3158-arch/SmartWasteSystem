#ifndef GRAPH_H
#define GRAPH_H

#include "Common.h"
#include <string>
#include <vector>

// Biggest map the program can hold. 11 locations are used, the rest is
// spare room so that the user can add a few more from the menu.
const int MAX_LOCATIONS = 50;

// ===========================================================================
// GRAPH STORED AS AN ADJACENCY MATRIX
//
// Every location gets a number (0, 1, 2 ...). The roads are kept in a
// square table:
//     distance[a][b] = length of the road from a to b, 0 means no road
//     blocked[a][b]  = true if that road is closed right now
//
// A matrix is used instead of pointers because it is only a 2D array, and
// reading distance[a][b] is as simple as reading any other array.
// The cost is memory: a big map with few roads still needs the full table.
// ===========================================================================
class Graph {
private:
    std::string names[MAX_LOCATIONS];
    double distance[MAX_LOCATIONS][MAX_LOCATIONS];
    bool blocked[MAX_LOCATIONS][MAX_LOCATIONS];
    std::string roadNames[MAX_LOCATIONS][MAX_LOCATIONS];
    int locationCount;

public:
    Graph();

    // Returns the number of a location, or -1 if the map does not have it
    int findLocation(std::string name) const;
    void addLocation(std::string name);
    void addRoad(std::string from, std::string to, double roadLength, std::string name);

    // Used for dynamic re-routing
    bool blockRoad(std::string from, std::string to);
    bool unblockRoad(std::string from, std::string to);
    bool isRoadBlocked(std::string from, std::string to) const;

    // Dijkstra's shortest path.
    // Fills path with the locations to travel through and returns the
    // total length. Returns -1 if there is no open path.
    double shortestPath(std::string start, std::string end,
                        std::vector<std::string>& path) const;

    // Visits the given stops one after another using shortestPath().
    // Returns the total length, or -1 if some part has no open path.
    double buildTour(const std::vector<std::string>& stops,
                     std::vector<std::string>& fullPath) const;

    bool hasLocation(std::string name) const { return findLocation(name) >= 0; }
    int getLocationCount() const { return locationCount; }
    std::string getLocationName(int index) const { return names[index]; }
    int getRoadCount() const;

    // Used when writing the roads to a file or to JSON
    double getDistance(int a, int b) const { return distance[a][b]; }
    bool getBlocked(int a, int b) const { return blocked[a][b]; }
    std::string getRoadName(int a, int b) const { return roadNames[a][b]; }

    void display() const;

    std::vector<std::string> toCSVLines() const;
    void addRoadFromCSV(std::string line);
};

#endif
