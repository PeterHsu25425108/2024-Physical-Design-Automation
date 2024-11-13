#ifndef PLACE_ROW_H
#define PLACE_ROW_H
#include "DEBUG.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <iomanip>
#include <unordered_map>
#include "Inst.h"
using namespace std;

class PlaceRow
{
public:
    PlaceRow() { ; }
    ~PlaceRow() { ; }
    PlaceRow(double startY) : startY(startY)
    {
        // initialize insts as a vector of size numSites whose elements are nullptr
        insts = vector<Inst *>(numSites, nullptr);
    }
    friend ostream &operator<<(ostream &os, const PlaceRow &placeRow);

    bool operator<(const PlaceRow &placeRow) const
    {
        return startY < placeRow.startY;
    }

    void setStartX(double startX) { this->startX = startX; }
    void setStartY(double startY) { this->startY = startY; }
    void setSiteWidth(double siteWidth) { this->siteWidth = siteWidth; }
    void setSiteHeight(double siteHeight) { this->siteHeight = siteHeight; }
    void setNumSites(int numSites) { this->numSites = numSites; }

    double getStartX() const { return startX; }
    double getStartY() const { return startY; }
    double getSiteWidth() const { return siteWidth; }
    double getSiteHeight() const { return siteHeight; }
    int getNumSites() const { return numSites; }

    void insertFF(Inst *ff, int siteIdx);
    void removeFF(int siteIdx);

private:
    static double startX, siteWidth, siteHeight;
    static int numSites;

    double startY;

    vector<Inst *> insts;
};

#endif