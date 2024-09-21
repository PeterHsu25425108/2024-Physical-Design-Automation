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
    int outlineWidth;
    int outlineHeight;

    // The linked list that stores the blocks in the outline,
    // both space and cell blocks
    list<Block> blocks;

    // An unordered map that stores the cell blocks in the outline,
    // u can use the block_id as the key to access the block
    unordered_map<int, Block> cell_blocks;

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
    Solver()
    {
        outlineWidth = -1;
        outlineHeight = -1;
        Void = Block(-1, make_pair(-1, -1), -1, -1, SPACE);
    }

    ~Solver()
    {
    }

    void CMDParser(ifstream &in_file);

    // Write the output to the out_file
    void writeOutput(ofstream &out_file);

    // Return the LL corner of the block that contains the point
    pair<int, int> PointFinding(pair<int, int> point);

    void InsertBlock(Block &block)
    {
        if (block.block_type == CELL)
        {
            InsertCellBlock(block);
        }
        else if (block.block_type == SPACE)
        {
            InsertSpaceBlock(block);
        }
        else
        {
            cerr << "Block type is UNDEFINED" << endl;
            exit(1);
        }
    }

    // Insert a cell block into the outline,
    // update the neighbors of the blocks, and perform merge/split of the space block if necessary
    void InsertCellBlock(Block &block);

    // Insert a space block into the outline,
    void InsertSpaceBlock(Block &block);
};

#endif