#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "Common.h"
#include "Bin.h"
#include "Vehicle.h"
#include "Graph.h"
#include "DataStructures.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

// ============================================================================
// CLASS: FileHandler
// Demonstrates:
// - C++ File I/O using ifstream, ofstream, and fstream
// - Persistent storage without external databases (No SQLite, MySQL, etc.)
// - Robust line parsing and serialization
// ============================================================================
class FileHandler {
public:
    // Load Bins from text file into LinkedList<Bin>
    static bool loadBins(const std::string& filepath, LinkedList<Bin>& binList) {
        std::ifstream inFile(filepath);
        if (!inFile.is_open()) {
            return false;
        }

        binList.clear();
        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') continue; // Skip comments and blank lines
            Bin b = Bin::fromCSV(line);
            if (!b.getId().empty()) {
                binList.insertTail(b);
            }
        }
        inFile.close();
        return true;
    }

    // Save Bins from LinkedList<Bin> into text file
    static bool saveBins(const std::string& filepath, const LinkedList<Bin>& binList) {
        std::ofstream outFile(filepath);
        if (!outFile.is_open()) {
            return false;
        }

        outFile << "# BinID,Location,Capacity(kg),CurrentWaste(kg),FillRate(kg/day),Status\n";
        binList.forEach([&outFile](const Bin& b) {
            outFile << b.toCSV() << "\n";
        });
        outFile.close();
        return true;
    }

    // Load Vehicles from text file
    static bool loadVehicles(const std::string& filepath, std::vector<Vehicle*>& vehicles) {
        std::ifstream inFile(filepath);
        if (!inFile.is_open()) {
            return false;
        }

        // Clean up any existing vehicles in vector
        for (auto v : vehicles) delete v;
        vehicles.clear();

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') continue;
            Vehicle* v = Vehicle::fromCSV(line);
            if (v) {
                vehicles.push_back(v);
            }
        }
        inFile.close();
        return true;
    }

    // Save Vehicles to text file
    static bool saveVehicles(const std::string& filepath, const std::vector<Vehicle*>& vehicles) {
        std::ofstream outFile(filepath);
        if (!outFile.is_open()) {
            return false;
        }

        outFile << "# VehicleID,Type,Capacity,CurrentLoad,Available,Location,Status,Extra\n";
        for (const auto v : vehicles) {
            outFile << v->toCSV() << "\n";
        }
        outFile.close();
        return true;
    }

    // Load Roads from text file into Graph
    static bool loadRoads(const std::string& filepath, Graph& graph) {
        std::ifstream inFile(filepath);
        if (!inFile.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(inFile, line)) {
            if (line.empty() || line[0] == '#') continue;
            Graph::loadRoadLine(line, graph);
        }
        inFile.close();
        return true;
    }

    // Save Roads from Graph to text file
    static bool saveRoads(const std::string& filepath, const Graph& graph) {
        std::ofstream outFile(filepath);
        if (!outFile.is_open()) {
            return false;
        }

        outFile << "# From,To,Distance(km),Blocked,RoadName\n";
        for (const auto& line : graph.toCSVLines()) {
            outFile << line << "\n";
        }
        outFile.close();
        return true;
    }

    // Append collection trip to collections history log
    static bool logCollection(const std::string& filepath, const std::string& record) {
        std::ofstream outFile(filepath, std::ios::app);
        if (!outFile.is_open()) {
            return false;
        }
        outFile << record << "\n";
        outFile.close();
        return true;
    }

    // Read collection history
    static std::vector<std::string> loadCollectionHistory(const std::string& filepath) {
        std::vector<std::string> history;
        std::ifstream inFile(filepath);
        if (!inFile.is_open()) {
            return history;
        }
        std::string line;
        while (std::getline(inFile, line)) {
            if (!line.empty() && line[0] != '#') {
                history.push_back(line);
            }
        }
        inFile.close();
        return history;
    }
};

#endif // FILE_HANDLER_H
