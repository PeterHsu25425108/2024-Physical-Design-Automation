#include <iostream>
#include <fstream>
#include "DEBUG.h"
#include "struct.h"
#include "BSTree.h"
#include "SA.h"
using namespace std;

int main(int argc, char *argv[])
{
    int alpha = stoi(argv[1]);
    ifstream block_file(argv[2]);
    ifstream net_file(argv[3]);
    ofstream out_file(argv[4]);

    SA sa_solver(alpha);

    sa_solver.parseBlock(block_file);
    sa_solver.parseNet(net_file);

    sa_solver.solve();
    sa_solver.writeOutput(out_file);

    block_file.close();
    net_file.close();
    out_file.close();

    return 0;
}