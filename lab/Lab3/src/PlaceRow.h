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

// define a data structure to store the consecutive intervals
struct Interval
{
    double x_left;
    double width;
    // records the index of the highest row this interval has gone through
    int topIdx;

    Interval(double x_left, double width, int topIdx) : x_left(x_left), width(width), topIdx(topIdx) {};
    Interval() : x_left(0), width(0), topIdx(0) {};
    ~Interval() {};
};

class PlaceRow
{
public:
    PlaceRow() { free_sites[startX] = numSites * siteWidth; }
    ~PlaceRow()
    {
        free_sites.clear();
        // ff_xPos2Inst.clear();
        // gate_xPos2Inst.clear();
    }
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
    // void setRowIdx(int rowIdx) { this->rowIdx = rowIdx; }
    void setLowestPlaceRowY(double lowest_placerowY) { this->lowest_placerowY = lowest_placerowY; }

    double getStartX() const { return startX; }
    double getLowestPlaceRowY() const { return lowest_placerowY; }
    double getStartY() const { return startY; }
    int getSiteWidth() const { return siteWidth; }
    int getSiteHeight() const { return siteHeight; }
    int getNumSites() const { return numSites; }
    map<double, int> getFreeSites() const { return free_sites; }

    int getRowIdx() const
    {
        return (int)((startY - lowest_placerowY) / siteHeight);
    }

    void insertInst(Inst *inst, int siteIdx);
    void removeFF(const Inst *ff);
    // search for the site that can accommodate the ff
    // if found, return the LL corner of the site which is on this row
    // else return(-1, -1)
    pair<double, double> searchFFLL(Inst *ff, vector<PlaceRow> &placeRows);

    // public static variables
    static double startX;
    static int numSites, siteWidth, siteHeight;
    // the y coordinate of the lowest placerow,
    // 1. used to calculate the row index of a ff
    // 2. with the x,y coordinate of a ff, the row index can be calculated
    static double lowest_placerowY;

private:
    // int rowIdx;
    double startY;

    // first: x coordinate,
    // second: max allowable width
    map<double, int> free_sites;

    // key: the x coordinate of the LL corner of a cell placed on the row
    // value : ptr to the cell instance
    // unordered_map<double, Inst *> ff_xPos2Inst;
    // unordered_map<double, Inst *> gate_xPos2Inst;
};

#endif
