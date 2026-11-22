#include "../include/Graph.h"
#include <cstdio>

using namespace std;

const double BIG = 1000000.0;   // stands for "infinity"

Graph::Graph() {
    locationCount = 0;
    for (int i = 0; i < MAX_LOCATIONS; i++) {
        for (int j = 0; j < MAX_LOCATIONS; j++) {
            distance[i][j] = 0;      // 0 means there is no road
            blocked[i][j] = false;
        }
    }
}

int Graph::findLocation(string name) const {
    for (int i = 0; i < locationCount; i++) {
        if (names[i] == name) return i;
    }
    return -1;
}

void Graph::addLocation(string name) {
    if (name == "") return;
    if (findLocation(name) >= 0) return;         // already on the map
    if (locationCount >= MAX_LOCATIONS) return;  // table is full

    names[locationCount] = name;
    locationCount++;
}

// Roads are two way, so the table is filled on both sides.
// A road from a place to itself, or with a length of zero or less, makes
// no sense on a map and would confuse Dijkstra, so it is refused.
void Graph::addRoad(string from, string to, double roadLength, string name) {
    from = cleanText(from);
    to = cleanText(to);
    name = cleanText(name);

    if (from == "" || to == "" || from == to) return;
    if (roadLength <= 0) return;

    addLocation(from);
    addLocation(to);

    int a = findLocation(from);
    int b = findLocation(to);
    if (a < 0 || b < 0) return;       // the map was already full

    distance[a][b] = roadLength;
    distance[b][a] = roadLength;
    roadNames[a][b] = name;
    roadNames[b][a] = name;
}

bool Graph::blockRoad(string from, string to) {
    int a = findLocation(from);
    int b = findLocation(to);
    if (a < 0 || b < 0) return false;
    if (distance[a][b] <= 0) return false;      // there is no such road

    blocked[a][b] = true;
    blocked[b][a] = true;
    return true;
}

bool Graph::unblockRoad(string from, string to) {
    int a = findLocation(from);
    int b = findLocation(to);
    if (a < 0 || b < 0) return false;
    if (distance[a][b] <= 0) return false;

    blocked[a][b] = false;
    blocked[b][a] = false;
    return true;
}

bool Graph::isRoadBlocked(string from, string to) const {
    int a = findLocation(from);
    int b = findLocation(to);
    if (a < 0 || b < 0) return false;
    return blocked[a][b];
}

int Graph::getRoadCount() const {
    int total = 0;
    for (int i = 0; i < locationCount; i++) {
        for (int j = i + 1; j < locationCount; j++) {
            if (distance[i][j] > 0) total++;
        }
    }
    return total;
}

// ===========================================================================
// DIJKSTRA'S SHORTEST PATH ALGORITHM
//
// best[x]   = shortest length found so far from start to x
// parent[x] = the location we came from, used to rebuild the path
// done[x]   = true once the answer for x is final
//
// In every round the unfinished location with the smallest best value is
// picked, and all its open roads are checked to see if they give a shorter
// way to a neighbour. Blocked roads are skipped, and that one line is what
// makes dynamic re-routing work.
// ===========================================================================
double Graph::shortestPath(string start, string end, vector<string>& path) const {
    path.clear();

    int s = findLocation(start);
    int e = findLocation(end);
    if (s < 0 || e < 0) return -1;

    if (s == e) {
        path.push_back(start);
        return 0;
    }

    double best[MAX_LOCATIONS];
    int parent[MAX_LOCATIONS];
    bool done[MAX_LOCATIONS];

    for (int i = 0; i < locationCount; i++) {
        best[i] = BIG;
        parent[i] = -1;
        done[i] = false;
    }
    best[s] = 0;

    for (int round = 0; round < locationCount; round++) {
        // 1. find the nearest location that is not finished yet
        int u = -1;
        double smallest = BIG;
        for (int i = 0; i < locationCount; i++) {
            if (!done[i] && best[i] < smallest) {
                smallest = best[i];
                u = i;
            }
        }
        if (u == -1) break;        // the rest of the map cannot be reached
        done[u] = true;
        if (u == e) break;         // the answer for the end point is final

        // 2. try to reach the neighbours of u through a shorter way
        for (int v = 0; v < locationCount; v++) {
            if (distance[u][v] <= 0) continue;    // no road between u and v
            if (blocked[u][v]) continue;          // road is closed

            if (best[u] + distance[u][v] < best[v]) {
                best[v] = best[u] + distance[u][v];
                parent[v] = u;
            }
        }
    }

    if (best[e] >= BIG) return -1;    // no open path

    // 3. walk back from the end to the start to build the path
    vector<string> backwards;
    int current = e;
    while (current != -1) {
        backwards.push_back(names[current]);
        current = parent[current];
    }

    for (int i = (int)backwards.size() - 1; i >= 0; i--)
        path.push_back(backwards[i]);

    return best[e];
}

double Graph::buildTour(const vector<string>& stops, vector<string>& fullPath) const {
    fullPath.clear();
    if (stops.size() < 2) {
        fullPath = stops;
        return 0;
    }

    double total = 0;
    for (int i = 0; i < (int)stops.size() - 1; i++) {
        vector<string> part;
        double length = shortestPath(stops[i], stops[i + 1], part);
        if (length < 0) {
            fullPath.clear();
            return -1;
        }
        total = total + length;

        // the first location of this part is the last of the previous one,
        // so it is skipped to avoid writing it twice
        int start = (i == 0) ? 0 : 1;
        for (int j = start; j < (int)part.size(); j++)
            fullPath.push_back(part[j]);
    }
    return total;
}

void Graph::display() const {
    printf("\n======================================================\n");
    printf("                  ROAD NETWORK GRAPH                  \n");
    printf("======================================================\n");
    printf("Locations: %d | Roads: %d\n\n", locationCount, getRoadCount());

    for (int i = 0; i < locationCount; i++) {
        printf("[%s]\n", names[i].c_str());
        for (int j = 0; j < locationCount; j++) {
            if (distance[i][j] <= 0) continue;
            printf("   --> %-18s (%.1f km) %-10s %s\n",
                   names[j].c_str(), distance[i][j],
                   blocked[i][j] ? "[BLOCKED]" : "[OPEN]",
                   roadNames[i][j].c_str());
        }
    }
    printf("======================================================\n");
}

// Writes every road only once (j starts after i) so the file has no duplicates
vector<string> Graph::toCSVLines() const {
    vector<string> lines;
    for (int i = 0; i < locationCount; i++) {
        for (int j = i + 1; j < locationCount; j++) {
            if (distance[i][j] <= 0) continue;
            lines.push_back(names[i] + "," + names[j] + "," +
                            numberToText(distance[i][j]) + "," +
                            (blocked[i][j] ? "1" : "0") + "," + roadNames[i][j]);
        }
    }
    return lines;
}

// Reads: Depot,GEU Gate,3.5,0,North Link
void Graph::addRoadFromCSV(string line) {
    string parts[5];
    int count = splitLine(line, parts, 5);
    if (count < 5) return;

    double roadLength;
    if (!textToNumber(parts[2], roadLength)) return;

    addRoad(parts[0], parts[1], roadLength, parts[4]);
    if (parts[3] == "1") blockRoad(parts[0], parts[1]);
}
