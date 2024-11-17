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
#include "Solver.h"
using namespace std;

int Solver::ff_placeRowIdx(const Inst *ff)
{
    // find the row that the ff is placed
    double ff_y = ff->getY();
    return (int)((ff_y - placeRows[0].getStartY()) / placeRows[0].getSiteHeight());
}

void Solver::addFF_PlaceRows(Inst *ff)
{
    // find the rows ff occupies, could be more than one row
    // this is where the LL corner of the ff is
    int LL_rowIdx = ff_placeRowIdx(ff);
    // the number of rows the ff occupies
    int occRowCount = ceil(ff->getHeight() / placeRows[0].getSiteHeight());
    // the row index of the highest row the ff occupies
    int top_rowIdx = LL_rowIdx + occRowCount - 1;

    if (DEBUG_addFF)
    {
        // output the number of rows the ff occupies
        cout << "Solver::addFF_PlaceRows: " << ff->getName() << " occupies " << occRowCount << " rows." << endl;
    }

    // add the ff to the placeRows
    for (int i = LL_rowIdx; i <= top_rowIdx; i++)
    {
        if (DEBUG_addFF)
        {
            cout << "Solver::addFF_PlaceRows: Adding " << ff->getName() << " to row " << i << endl;
        }

        placeRows[i].insertFF(ff, i);
    }
}

void Solver::removeFF_PlaceRows(const Inst *ff)
{

    if (DEBUG_removeFF)
    {
        cout << "Solver::removeFF_PlaceRows: Removing " << ff->getName() << " from placeRows" << endl;
    }

    // find the rows ff occupies, could be more than one row
    // this is where the LL corner of the ff is
    int LL_rowIdx = ff_placeRowIdx(ff);

    if (DEBUG_removeFF)
    {
        cout << "Solver::removeFF_PlaceRows: " << "LL_rowIdx: " << LL_rowIdx << endl;
    }

    // the number of rows the ff occupies
    int occRowCount = ceil(ff->getHeight() / placeRows[0].getSiteHeight());

    if (DEBUG_removeFF)
    {
        cout << "Solver::removeFF_PlaceRows: " << ff->getName() << " occupies " << occRowCount << " rows." << endl;
    }

    // the row index of the highest row the ff occupies
    int top_rowIdx = LL_rowIdx + occRowCount - 1;

    if (DEBUG_removeFF)
    {
        cout << "Solver::removeFF_PlaceRows: " << "top_rowIdx: " << top_rowIdx << endl;
    }

    // remove the ff from the placeRows
    for (int i = LL_rowIdx; i <= top_rowIdx; i++)
    {
        placeRows[i].removeFF(ff);

        if (DEBUG_removeFF)
        {
            cout << "Solver::removeFF_PlaceRows: Removed " << ff->getName() << " from row " << i << endl;
        }
    }
}