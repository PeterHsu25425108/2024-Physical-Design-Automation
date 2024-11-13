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
using namespace std;

class Solver
{
public:
    Solver() {};
    ~Solver() {};

    void readlg(ifstream &lg_file);
    void solve();
    void writeOutput(ofstream &output_file);

private:
    double Alpha, Beta;
    double DieLLX, DieLLY, DieURX, DieURY;

    // for storing the result of the moved cell for each banked ff
    // key: new ff name
    // value: vec of old ff name
    vector<pair<string, vector<string>>> result;

    // dict for all ff ever appeared in the layout
    // key: ff name, value: ptr to the ff instance
    unordered_map<string, Inst *> ff_dict;

    // dict for ffs that are placed on the same row
    // key: ff name, value: pair.first: row index in placeRows, pair.second: index in row.insts
    unordered_map<string, pair<int, int>> ff_posOnPLaceRow;
    // names of ffs that are placed on the die
    // if a ff gets banked, it will be removed from this vector
    vector<string> ff_onDie;

    // dict for all gate ever appeared in the layout
    // key: gate name, value: ptr to the gate instance
    unordered_map<string, Inst *> gate_dict;

    // store the placement rows
    // will be sorted by y coordinate after parsing
    vector<PlaceRow> placeRows;
};

#endif