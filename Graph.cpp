#include "../include/Graph.h"
#include <queue>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <sstream>

Graph::Graph() {}

void Graph::addLocation(const std::string& location) {
    if (adjList.find(location) == adjList.end()) {
        adjList[location] = std::vector<Edge>();
    }
}

void Graph::addRoad(const std::string& from, const std::string& to, double distance,
                    const std::string& roadName, bool bidirectional) {
    addLocation(from);
    addLocation(to);

    // Check if edge already exists
    bool exists = false;
    for (auto& edge : adjList[from]) {
        if (edge.destination == to) {
            edge.distance = distance;
            edge.roadName = roadName;
            exists = true;
            break;
        }
    }
    if (!exists) {
        adjList[from].push_back(Edge(to, distance, false, roadName));
    }

    if (bidirectional) {
        bool revExists = false;
        for (auto& edge : adjList[to]) {
            if (edge.destination == from) {
                edge.distance = distance;
                edge.roadName = roadName;
                revExists = true;
                break;
            }
        }
        if (!revExists) {
            adjList[to].push_back(Edge(from, distance, false, roadName));
        }
    }
}

bool Graph::blockRoad(const std::string& from, const std::string& to) {
    bool found = false;
    if (adjList.find(from) != adjList.end()) {
        for (auto& edge : adjList[from]) {
            if (edge.destination == to) {
                edge.isBlocked = true;
                found = true;
            }
        }
    }
    if (adjList.find(to) != adjList.end()) {
        for (auto& edge : adjList[to]) {
            if (edge.destination == from) {
                edge.isBlocked = true;
                found = true;
            }
        }
    }
    return found;
}

bool Graph::unblockRoad(const std::string& from, const std::string& to) {
    bool found = false;
    if (adjList.find(from) != adjList.end()) {
        for (auto& edge : adjList[from]) {
            if (edge.destination == to) {
                edge.isBlocked = false;
                found = true;
            }
        }
    }
    if (adjList.find(to) != adjList.end()) {
        for (auto& edge : adjList[to]) {
            if (edge.destination == from) {
                edge.isBlocked = false;
                found = true;
            }
        }
    }
    return found;
}

bool Graph::isRoadBlocked(const std::string& from, const std::string& to) const {
    auto it = adjList.find(from);
    if (it != adjList.end()) {
        for (const auto& edge : it->second) {
            if (edge.destination == to) {
                return edge.isBlocked;
            }
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> Graph::getBlockedRoads() const {
    std::vector<std::pair<std::string, std::string>> blocked;
    for (const auto& pair : adjList) {
        for (const auto& edge : pair.second) {
            if (edge.isBlocked && pair.first < edge.destination) {
                blocked.push_back({pair.first, edge.destination});
            }
        }
    }
    return blocked;
}

bool Graph::hasLocation(const std::string& location) const {
    return adjList.find(location) != adjList.end();
}

std::vector<std::string> Graph::getAllLocations() const {
    std::vector<std::string> locs;
    for (const auto& pair : adjList) {
        locs.push_back(pair.first);
    }
    return locs;
}

int Graph::getRoadCount() const {
    int total = 0;
    for (const auto& pair : adjList) {
        total += static_cast<int>(pair.second.size());
    }
    return total / 2; // bidirectional pairs
}

// ============================================================================
// DIJKSTRA'S SHORTEST PATH ALGORITHM
// - Uses a min-priority queue storing (distance, location)
// - Maintains dist map and parent map
// - Disregards blocked edges
// ============================================================================
std::pair<double, std::vector<std::string>> Graph::dijkstra(
    const std::string& start, const std::string& destination) const {

    if (!hasLocation(start) || !hasLocation(destination)) {
        return {-1.0, {}};
    }

    if (start == destination) {
        return {0.0, {start}};
    }

    const double INF = std::numeric_limits<double>::infinity();
    std::map<std::string, double> dist;
    std::map<std::string, std::string> parent;

    for (const auto& pair : adjList) {
        dist[pair.first] = INF;
    }

    // Min-heap Priority Queue of (distance, node)
    typedef std::pair<double, std::string> DistNode;
    std::priority_queue<DistNode, std::vector<DistNode>, std::greater<DistNode>> pq;

    dist[start] = 0.0;
    pq.push({0.0, start});

    while (!pq.empty()) {
        auto current = pq.top();
        pq.pop();

        double d = current.first;
        std::string u = current.second;

        // If we reached destination with optimal distance
        if (u == destination) break;

        if (d > dist[u]) continue;

        auto it = adjList.find(u);
        if (it == adjList.end()) continue;

        for (const auto& edge : it->second) {
            // CRITICAL FOR DYNAMIC RE-ROUTING:
            // Skip road if marked blocked!
            if (edge.isBlocked) continue;

            std::string v = edge.destination;
            double weight = edge.distance;

            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                parent[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    if (dist[destination] == INF) {
        // No path exists (e.g. road network partitioned or blocked)
        return {-1.0, {}};
    }

    // Reconstruct path backwards from destination to start
    std::vector<std::string> path;
    std::string curr = destination;
    while (curr != start) {
        path.push_back(curr);
        curr = parent[curr];
    }
    path.push_back(start);
    std::reverse(path.begin(), path.end());

    return {dist[destination], path};
}

// Multi-stop tour visiting stops in sequence
std::pair<double, std::vector<std::string>> Graph::calculateTour(
    const std::vector<std::string>& stops) const {

    if (stops.size() < 2) return {0.0, stops};

    double totalDist = 0.0;
    std::vector<std::string> fullPath;

    for (size_t i = 0; i < stops.size() - 1; ++i) {
        auto stepResult = dijkstra(stops[i], stops[i + 1]);
        if (stepResult.first < 0) {
            // Path broken!
            return {-1.0, {}};
        }
        totalDist += stepResult.first;

        // Append to full path without duplicating joining nodes
        if (i == 0) {
            fullPath = stepResult.second;
        } else {
            for (size_t j = 1; j < stepResult.second.size(); ++j) {
                fullPath.push_back(stepResult.second[j]);
            }
        }
    }

    return {totalDist, fullPath};
}

void Graph::displayGraph() const {
    std::cout << "\n======================================================\n";
    std::cout << "                 CAMPUS ROAD NETWORK GRAPH            \n";
    std::cout << "======================================================\n";
    std::cout << "Total Locations: " << getLocationCount()
              << " | Total Roads: " << getRoadCount() << "\n\n";

    for (const auto& pair : adjList) {
        std::cout << Color::CYAN << "[" << pair.first << "]" << Color::RESET << "\n";
        for (const auto& edge : pair.second) {
            std::cout << "   --> " << std::left << std::setw(18) << edge.destination
                      << " (" << std::fixed << std::setprecision(1) << edge.distance << " km)";
            if (edge.isBlocked) {
                std::cout << "  " << Color::RED << "[BLOCKED]" << Color::RESET;
            } else {
                std::cout << "  " << Color::GREEN << "[OPEN]" << Color::RESET;
            }
            if (!edge.roadName.empty()) {
                std::cout << " - " << edge.roadName;
            }
            std::cout << "\n";
        }
    }
    std::cout << "======================================================\n";
}

std::vector<std::string> Graph::toCSVLines() const {
    std::vector<std::string> lines;
    for (const auto& pair : adjList) {
        for (const auto& edge : pair.second) {
            // Only output one direction to avoid duplication in file
            if (pair.first < edge.destination) {
                std::ostringstream oss;
                oss << pair.first << "," << edge.destination << ","
                    << edge.distance << "," << (edge.isBlocked ? 1 : 0) << ","
                    << (edge.roadName.empty() ? "Road" : edge.roadName);
                lines.push_back(oss.str());
            }
        }
    }
    return lines;
}

void Graph::loadRoadLine(const std::string& line, Graph& g) {
    std::stringstream ss(line);
    std::string from, to, distStr, blockedStr, roadName;

    if (std::getline(ss, from, ',') &&
        std::getline(ss, to, ',') &&
        std::getline(ss, distStr, ',') &&
        std::getline(ss, blockedStr, ',')) {

        std::getline(ss, roadName);
        if (!roadName.empty() && roadName.back() == '\r') roadName.pop_back();

        double dist = std::stod(distStr);
        bool blocked = (blockedStr == "1" || blockedStr == "true");

        g.addRoad(from, to, dist, roadName, true);
        if (blocked) {
            g.blockRoad(from, to);
        }
    }
}
