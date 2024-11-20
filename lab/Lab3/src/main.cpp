#include "Solver.h"
#include "DEBUG.h"
#include "Inst.h"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[])
{
    ifstream lg_file(argv[1]);
    ifstream opt_file(argv[2]);
    ofstream output_file(argv[3]);

    Solver solver;
    solver.readlg(lg_file);
    solver.solve(opt_file, output_file);

    lg_file.close();
    opt_file.close();
    output_file.close();

    return 0;
}