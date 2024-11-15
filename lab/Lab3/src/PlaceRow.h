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
#include <map>
using namespace std;

/*struct EmptySite
{
    double x;
    double width;
    EmptySite(double x, double width) : x(x), width(width) { ; }
    bool operator<(const EmptySite &emptySite) const
    {
        return x < emptySite.x;
    }
};*/

class PlaceRow
{
public:
    PlaceRow() { ; }
    ~PlaceRow() { ; }
    PlaceRow(double startY) : startY(startY)
    {
        // define the entire row as free space
        free_sites[startX] = numSites * siteWidth;
    }
    friend ostream &operator<<(ostream &os, const PlaceRow &placeRow);

    bool operator<(const PlaceRow &placeRow) const
    {
        return startY < placeRow.startY;
    }

    void setStartX(double startX) { this->startX = startX; }
    void setStartY(double startY) { this->startY = startY; }
    void setSiteWidth(int siteWidth) { this->siteWidth = siteWidth; }
    void setSiteHeight(int siteHeight) { this->siteHeight = siteHeight; }
    void setNumSites(int numSites) { this->numSites = numSites; }
    void setRowIdx(int rowIdx) { this->rowIdx = rowIdx; }
    void setLowestPlaceRowY(double lowest_placerowY) { this->lowest_placerowY = lowest_placerowY; }

    double getStartX() const { return startX; }
    double getStartY() const { return startY; }
    int getSiteWidth() const { return siteWidth; }
    int getSiteHeight() const { return siteHeight; }
    int getNumSites() const { return numSites; }

    int getRowIdx() const { return rowIdx; }

    void insertFF(Inst *ff, int siteIdx);
    void removeFF(const Inst *ff);

private:
    static double startX;
    static int numSites, siteWidth, siteHeight;
    // the y coordinate of the lowest placerow,
    // 1. used to calculate the row index of a ff
    // 2. with the x,y coordinate of a ff, the row index can be calculated
    static double lowest_placerowY;

    int rowIdx;
    double startY;

    // first: x coordinate,
    // second: max allowable width
    map<double, int> free_sites;
};

#endif