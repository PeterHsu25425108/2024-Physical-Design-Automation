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

// define a data structure to store the consecutive intervals
struct Interval
{
    int x_left;
    int width;
    // records the index of the highest row this interval has gone through
    int topIdx;

    Interval(int x_left, int width, int topIdx) : x_left(x_left), width(width), topIdx(topIdx) {};
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
    PlaceRow(int startY) : startY(startY)
    {
        // define the entire row as free space
        free_sites[startX] = numSites * siteWidth;
    }
    friend ostream &operator<<(ostream &os, const PlaceRow &placeRow);

    bool operator<(const PlaceRow &placeRow) const
    {
        return startY < placeRow.startY;
    }

    void setStartX(int startX) { this->startX = startX; }
    void setStartY(int startY) { this->startY = startY; }
    void setSiteWidth(int siteWidth) { this->siteWidth = siteWidth; }
    void setSiteHeight(int siteHeight) { this->siteHeight = siteHeight; }
    void setNumSites(int numSites) { this->numSites = numSites; }
    // void setRowIdx(int rowIdx) { this->rowIdx = rowIdx; }
    void setLowestPlaceRowY(int lowest_placerowY) { this->lowest_placerowY = lowest_placerowY; }

    int getStartX() const { return startX; }
    int getLowestPlaceRowY() const { return lowest_placerowY; }
    int getStartY() const { return startY; }
    int getSiteWidth() const { return siteWidth; }
    int getSiteHeight() const { return siteHeight; }
    int getNumSites() const { return numSites; }
    map<int, int> getFreeSites() const { return free_sites; }

    int getRowIdx() const
    {
        return (int)((startY - lowest_placerowY) / siteHeight);
    }

    void insertInst(Inst *inst, int siteIdx);
    void removeFF(const Inst *ff);
    // search for the site that can accommodate the ff
    // if found, return the LL corner of the site which is on this row
    // else return(-1, -1)
    pair<int, int> searchFFLL(Inst *ff, vector<PlaceRow> &placeRows);

    // public static variables
    static int startX;
    static int numSites, siteWidth, siteHeight;
    // the y coordinate of the lowest placerow,
    // 1. used to calculate the row index of a ff
    // 2. with the x,y coordinate of a ff, the row index can be calculated
    static int lowest_placerowY;

private:
    // int rowIdx;
    int startY;

    // first: x coordinate,
    // second: max allowable width
    map<int, int> free_sites;

    // key: the x coordinate of the LL corner of a cell placed on the row
    // value : ptr to the cell instance
    // unordered_map<int, Inst *> ff_xPos2Inst;
    // unordered_map<int, Inst *> gate_xPos2Inst;
};

#endif
