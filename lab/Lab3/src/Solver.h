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
    // vector<pair<string, vector<FF>>> result;
};

#endif