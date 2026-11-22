#include "../include/Bin.h"
#include <cstdio>

using namespace std;

Bin::Bin() {
    id = "";
    location = "";
    capacity = 100;
    currentWaste = 0;
    fillRate = 5;
    status = "Pending";
    calculatePriority();
}

Bin::Bin(string id, string location, double capacity,
         double currentWaste, double fillRate, string status) {
    this->id = cleanText(id);
    this->location = cleanText(location);
    this->capacity = capacity;
    this->fillRate = fillRate;
    this->status = status;

    // a bin can never hold more than its capacity
    if (currentWaste < 0) currentWaste = 0;
    if (currentWaste > capacity) currentWaste = capacity;
    this->currentWaste = currentWaste;

    calculatePriority();
}

void Bin::calculatePriority() {
    if (capacity <= 0)
        fillPercent = 0;
    else
        fillPercent = (currentWaste / capacity) * 100;

    if (fillPercent > 100) fillPercent = 100;
    if (fillPercent < 0) fillPercent = 0;

    if (fillPercent >= 90)      priority = CRITICAL;
    else if (fillPercent >= 75) priority = HIGH;
    else if (fillPercent >= 50) priority = MEDIUM;
    else                        priority = LOW;

    // A bin that fills up fast becomes urgent sooner, so the fill rate is
    // added to the score. Critical bins get an extra boost so that they
    // always come out of the priority queue first.
    urgencyScore = fillPercent + (fillRate * 1.5);
    if (priority == CRITICAL) urgencyScore = urgencyScore + 50;
}

void Bin::setCurrentWaste(double waste) {
    if (waste < 0) waste = 0;
    if (waste > capacity) waste = capacity;
    currentWaste = waste;
    calculatePriority();
}

void Bin::markCollected() {
    currentWaste = 0;
    status = "Collected";
    calculatePriority();
}

// printf is used for the tables because lining up columns with it is
// much shorter than setw() from iomanip.
void Bin::displayRow() const {
    printf("| %-6s| %-17s| %7.1f | %7.1f | %5.1f%% | %-9s| %-10s |\n",
           id.c_str(), location.c_str(), capacity, currentWaste, fillPercent,
           priorityToString(priority).c_str(), status.c_str());
}

void Bin::displayDetails() const {
    printf("------------------------------------------\n");
    printf("Bin ID        : %s\n", id.c_str());
    printf("Location      : %s\n", location.c_str());
    printf("Capacity      : %.1f kg\n", capacity);
    printf("Current Waste : %.1f kg (%.1f%%)\n", currentWaste, fillPercent);
    printf("Fill Rate     : %.1f kg/day\n", fillRate);
    printf("Priority      : %s\n", priorityToString(priority).c_str());
    printf("Urgency Score : %.1f\n", urgencyScore);
    printf("Status        : %s\n", status.c_str());
    printf("------------------------------------------\n");
}

string Bin::toCSV() const {
    return id + "," + location + "," + numberToText(capacity) + "," +
           numberToText(currentWaste) + "," + numberToText(fillRate) + "," + status;
}

// Reads one line like: B101,GEU Gate,100,90,8,Pending
Bin Bin::fromCSV(string line) {
    string parts[6];
    int count = splitLine(line, parts, 6);
    if (count < 6) return Bin();

    double capacity, waste, rate;
    if (parts[0] == "") return Bin();
    if (!textToNumber(parts[2], capacity)) return Bin();
    if (!textToNumber(parts[3], waste)) return Bin();
    if (!textToNumber(parts[4], rate)) return Bin();
    if (capacity <= 0) return Bin();

    string status = parts[5];
    if (status == "") status = "Pending";

    return Bin(parts[0], parts[1], capacity, waste, rate, status);
}
