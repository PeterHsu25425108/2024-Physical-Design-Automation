#ifndef SA_H
#define SA_H
#include <iostream>
#include <fstream>
#include "DEBUG.h"
#include "struct.h"
#include "BSTree.h"
#include <vector>
#include <chrono>
#include <ctime>
#include <cmath>
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

    // Monitoring runtime
    // std::chrono::time_point<std::chrono::high_resolution_clock> start;
    /// std::chrono::time_point<std::chrono::high_resolution_clock> end;

public:
    SA() { ; }
    SA(double alpha, double T = 10000, double T_min = 0.000001, double t_decay = 0.9) : alpha(alpha), T(T), T_min(T_min), t_decay(t_decay) { ; }
    ~SA() { ; }

    double getAlpha() const { return alpha; }
    double finalCost(const BSTree &tree) { return alpha * tree.getBoundaryWidth() * tree.getBoundaryHeight() + (1 - alpha) * tree.getTotHPWL(); }
    double SA_cost(const BSTree &tree)
    {
        double objective = finalCost(tree);
        double x_error = max(0.0, double(tree.getBoundaryWidth() - outlineWidth));
        double y_error = max(0.0, double(tree.getBoundaryHeight() - outlineHeight));

        if (x_error + y_error == 0)
        {
            return -outlineHeight * outlineWidth * 4 / objective;
        }
        else
        {
            return 10 * (x_error + y_error);
        }

        // return finalCost(tree) * tree.getTotHPWL() * (1 + abs(tree.getAspectRatio() - double(outlineWidth) / outlineHeight) / (max(tree.getAspectRatio(), double(outlineWidth) / outlineHeight)));
    }

    void parseBlock(ifstream &block_file);
    void parseNet(ifstream &net_file);
    void solve();
    void writeOutput(ostream &out);
    bool fitInOutline(const BSTree &tree) const { return outlineWidth >= tree.getBoundaryWidth() && outlineHeight >= tree.getBoundaryHeight(); }
};

#endif