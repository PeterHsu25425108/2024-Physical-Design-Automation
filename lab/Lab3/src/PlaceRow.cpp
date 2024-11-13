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
#include "PlaceRow.h"
using namespace std;

double PlaceRow::startX = 0.0;
double PlaceRow::siteWidth = 0.0;
double PlaceRow::siteHeight = 0.0;
int PlaceRow::numSites = 0;

void PlaceRow::insertFF(Inst *ff, int siteIdx)
{
    insts[siteIdx] = ff;
}

void PlaceRow::removeFF(int siteIdx)
{
    insts[siteIdx] = nullptr;
}