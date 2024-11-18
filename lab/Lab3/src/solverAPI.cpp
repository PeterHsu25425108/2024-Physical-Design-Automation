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

int Solver::Inst_placeRowIdx(const Inst *inst)
{
    // find the row that the inst is placed
    double inst_y = inst->getY();
    return (int)((inst_y - placeRows[0].getStartY()) / placeRows[0].getSiteHeight());
}

void Solver::addInst_PlaceRows(Inst *inst)
{
    // find the rows inst occupies, could be more than one row
    // this is where the LL corner of the inst is
    int LL_rowIdx = Inst_placeRowIdx(inst);
    // the number of rows the inst occupies
    int occRowCount = ceil(inst->getHeight() / placeRows[0].getSiteHeight());
    // the row index of the highest row the inst occupies
    int top_rowIdx = LL_rowIdx + occRowCount - 1;

    if (inst->getName() == "FF_2_3")
    {
        cout << "Solver::addinst_PlaceRows: " << "LL_rowIdx: " << LL_rowIdx << " top_rowIdx: " << top_rowIdx << endl;
    }

    if (DEBUG_addInst)
    {
        // output the number of rows the inst occupies
        cout << "Solver::addinst_PlaceRows: " << inst->getName() << " occupies " << occRowCount << " rows." << endl;
    }

    // add the inst to the placeRows
    for (int i = LL_rowIdx; i <= top_rowIdx; i++)
    {
        if (DEBUG_addInst)
        {
            cout << "Solver::addinst_PlaceRows: Adding " << inst->getName() << " to row " << i << endl;
        }

        placeRows[i].insertInst(inst, i);
    }
}

void Solver::removeFF_PlaceRows(const Inst *ff)
{

    if (ff->getFixed())
    {
        cerr << "Solver::removeFF_PlaceRows: removing a fixed ff " << ff->getName() << endl;
        exit(1);
    }

    if (DEBUG_removeFF)
    {
        cout << "Solver::removeFF_PlaceRows: Removing " << ff->getName() << " from placeRows" << endl;
    }

    // find the rows ff occupies, could be more than one row
    // this is where the LL corner of the ff is
    int LL_rowIdx = Inst_placeRowIdx(ff);

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