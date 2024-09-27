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

    solver.writeOutput(out_file);

    if (DEBUG_MAIN)
    {
        cout << endl;
        solver.writeOutput(cout);
    }

    in_file.close();
    out_file.close();

    return 0;
}