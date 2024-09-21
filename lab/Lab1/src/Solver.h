#ifndef SOLVER_H
#define SOLVER_H
#include "DEBUG.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "Block.h"

using namespace std;

class Solver
{
private:
    int outlineWidth;
    int outlineHeight;

public:
    void CMDParser(ifstream &in_file);
};

#endif