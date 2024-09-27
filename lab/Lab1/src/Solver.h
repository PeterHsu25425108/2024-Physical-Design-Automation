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
#include <list>
#include <unordered_map>

using namespace std;

class Solver
{
private:
    int num_block_outline;
    int outlineWidth;
    int outlineHeight;

    // The linked list that stores the blocks in the outline,
    // both space and cell blocks
    list<Block> blocks;

    // An unordered map that stores the cell blocks in the outline,
    // u can use the block_id as the key to access the block
    unordered_map<int, Block *> Id2CellBlockPtr;

    // If the neighbor pointer points out of the outline,
    // it points to Void
    Block Void;

    // Stores the solution of the point finding instruction,
    // which is the LL corner of the block that contains the point
    vector<pair<int, int>> PointFindSol;

    // Stores the pointer to the cell blocks in ascending order of their block_id,
    // used for outputting the cell blocks in the correct order
    vector<Block *> CellBlockPtr;

public:
    Solver() : Void(make_pair(-1, -1), -1, -1, SPACE, -2)
    {
        num_block_outline = 0;
        outlineWidth = -1;
        outlineHeight = -1;
    }

    ~Solver()
    {
    }

    void addBlock(const Block &block);

    void CMDParser(ifstream &in_file);

    // Write the output to the out_file
    void writeOutput(ostream &out);

    // Return the LL corner of the block that contains the point
    pair<int, int> PointFinding(pair<int, int> point);

    // Insert a cell block into the outline,
    // update the neighbors of the blocks, and perform merge/split of the space block if necessary
    void InsertCellBlock(Block &block);

    // Find the neighbors of the block
    vector<Block *> findNeighbors(const Block &block) const;
};

#endif