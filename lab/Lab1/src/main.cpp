#include <iostream>
#include <fstream>
#include <vector>
#include "Solver.h"

using namespace std;

int main(int argc, char *argv[])
{
    ifstream in_file(argv[1]);
    ofstream out_file(argv[2]);

    Solver solver;
    solver.CMDParser(in_file);

    in_file.close();
    out_file.close();

    return 0;
}