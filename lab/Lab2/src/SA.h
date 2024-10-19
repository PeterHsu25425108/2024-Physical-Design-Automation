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

public:
    SA() { ; }
    SA(double alpha) : alpha(alpha) { ; }
    ~SA() { ; }

    double getAlpha() const { return alpha; }
    double finalCost() { return alpha * bs_tree.getBoundingArea() + (1 - alpha) * bs_tree.getTotHPWL(); }

    void parseBlock(ifstream &block_file);
    void parseNet(ifstream &net_file);
    void solve();
    void writeOutput(ostream &out, double time_taken);
};

#endif