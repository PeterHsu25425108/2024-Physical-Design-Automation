#ifndef SA_H
#define SA_H
#include <iostream>
#include <fstream>
#include "DEBUG.h"
#include "struct.h"
#include "BSTree.h"
using namespace std;

class SA
{
private:
    BSTree bs_tree;
    double alpha;
    int outlineWidth, outlineHeight;
    int numBlocks, numNets, numTerms;

    // Initial temp
    double T;
    // Final temp to stop the iteration
    double T_min;
    // The decay rate of the temp
    double t_decay;

public:
    SA() { ; }
    SA(double alpha, double T = 1000, double T_min = 0.01, double t_decay = 0.9) : alpha(alpha), T(T), T_min(T_min), t_decay(t_decay) { ; }
    ~SA() { ; }

    double getAlpha() const { return alpha; }
    double finalCost() { return alpha * bs_tree.getBoundingArea() + (1 - alpha) * bs_tree.getTotHPWL(); }

    void parseBlock(ifstream &block_file);
    void parseNet(ifstream &net_file);
    void solve();
    void writeOutput(ostream &out, double time_taken);
};

#endif