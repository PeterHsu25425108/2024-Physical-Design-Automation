#include <iostream>
#include <fstream>
#include "DEBUG.h"
#include "struct.h"
#include "BSTree.h"
#include "SA.h"
#include <chrono>
using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[])
{
    auto start = high_resolution_clock::now();
    double alpha = stod(argv[1]);
    ifstream block_file(argv[2]);
    ifstream net_file(argv[3]);
    ofstream out_file(argv[4]);

    SA sa_solver(alpha);

    sa_solver.parseBlock(block_file);
    sa_solver.parseNet(net_file);

    /*sa_solver.solve();
    auto end = high_resolution_clock::now();
    // calculate the elapsed time in seconds
    double time_taken = duration_cast<seconds>(end - start).count();
    sa_solver.writeOutput(out_file, time_taken);*/

    block_file.close();
    net_file.close();
    out_file.close();

    return 0;
}