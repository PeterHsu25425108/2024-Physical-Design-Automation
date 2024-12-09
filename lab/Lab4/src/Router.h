#ifndef ROUTER_H
#define ROUTER_H
#include"DEBUG.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include"struct.h"
using namespace std;

class Router
{
public:
    Router(){;}
    ~Router(){;}
    void parseGMP(ifstream& gmp_file);
    void parseGCL(ifstream& gcl_file);
    void parseCST(ifstream& cst_file);
    void writeOutput(ofstream& out_file, const Net& net);

    // solve the routing problem
    // each net are printed after being routed
    void solve(ofstream& out_file);

    // perform the A star algorithm for a net and save the result to the net.wire_segs
    void A_star(int net_id);
    // track back the path from the parent and save the wire segments to wire_segs
    // update the wire_segs and grid.verNetCount and grid.horNetCount
    void backTrack(const vector<vector<pair<int,int>>>& parent, pair<int, int> src_idx, pair<int,int> tar_idx, vector<WireSeg>& wire_segs);
    // the estimated cost function
    double h_est(pair<int, int> src_idx, pair<int, int> tar_idx);
    // the exact cost to propagate from the start node to a adjacent node
    double exact_cost(pair<int, int> src_idx, pair<int, int> tar_idx, const vector<vector<pair<int,int>>>& parent);

    // convert the grid ll coordinate to the grid index
    pair<int, int> LLcoor2Idx(int x, int y)
    {
        return { (x - GMP_LLX) / GRID_W, (y - GMP_LLY) / GRID_H };
    }

    // convert the grid index to the grid ll coordinate
    pair<int, int> Idx2LLcoor(int i, int j)
    {
        return { GMP_LLX + i * GRID_W, GMP_LLY + j * GRID_H };
    }

private:
    Chip chip1, chip2;
    //vector<vector<double>> grid_costM1, grid_costM2;

    // M1 and M2 cost and capacity can be found in grid
    // dim: GRID_DIM_VER x GRID_DIM_HOR
    vector<vector<GCell>> grid;

    vector<Net> nets;

    double Alpha, Beta, Gamma, Delta;
    double ViaCost;
    int GMP_LLX, GMP_LLY, GMP_WIDTH, GMP_HEIGHT;
    int GRID_W, GRID_H;
    int GRID_DIM_HOR, GRID_DIM_VER;
};

#endif