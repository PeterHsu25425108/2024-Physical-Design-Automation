#ifndef SOLVER_H
#define SOLVER_H
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
#include <list>
using namespace std;

class Solver
{
public:
    Solver() {};
    ~Solver()
    {
        // delete all the ffs
        for (auto &it : ff_dict)
        {
            delete it.second;
        }

        // delete all the gates
        for (auto &it : gate_dict)
        {
            delete it.second;
        }

        ff_dict.clear();
        gate_dict.clear();
        placeRows.clear();
    }

    // diffrent legalization methods, called in solve(), return the names of the moved cells
    vector<string> BruteFindInsertion(Inst *ff);

    // parse the lg file and initialize data structures
    void readlg(ifstream &lg_file);
    // parse the opt file and legalize the layout
    void solve(ifstream &opt_file, ofstream &output_file);
    // write the output to the output file
    void writeOutput(ofstream &output_file, Inst *new_ff, vector<string> moved_ff);

    // ** helper functions for low level operations **
    // These stupid operations will be encapsulated

    // calculate the index of the row on which the ff is placed
    int Inst_placeRowIdx(const Inst *inst);

    // add a Inst to the placementrow, all related data structures will be updated
    void addInst_PlaceRows(Inst *inst);

    // remove a ff from the placementrow, all related data structures will be updated
    void removeFF_PlaceRows(const Inst *ff);

private:
    double Alpha, Beta;
    int DieLLX, DieLLY, DieURX, DieURY;

    // for storing the result of the moved cell for each banked ff
    // key: new ff name
    // value: vec of old ff name
    vector<pair<string, vector<string>>> result;

    // dict for all ff ever appeared in the layout
    // key: ff name, value: ptr to the ff instance
    unordered_map<string, Inst *> ff_dict;

    // dict for ffs that are placed on the same row
    // key: ff name, value: pair.first: row index in placeRows, pair.second: index in row.insts
    // unordered_map<string, pair<int, int>> ff_posOnPLaceRow;

    // names of ffs that are placed on the die
    // if a ff gets banked, it will be removed from this list
    // list<string> ff_onDie;

    // dict for all gate ever appeared in the layout
    // key: gate name, value: ptr to the gate instance
    unordered_map<string, Inst *> gate_dict;

    // store the placement rows
    // will be sorted by y coordinate after parsing
    vector<PlaceRow> placeRows;

    // write the text file for plotting
    void writePlotFile(ofstream &plot_file, vector<string> moved_ff);

    void writePlaceRow(ofstream &plot_file);

    // Find the cell whose LL corner is at the given coordinate on the layout
    // key: the coordinate of LL corner of the cell
    // value: ptr to the cell instance
    // unordered_map<pair<int, int>, Inst *> cellPos2InstPtr;
};

#endif
