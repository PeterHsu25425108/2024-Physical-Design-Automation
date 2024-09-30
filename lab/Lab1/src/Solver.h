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
    // The number of blocks in the outline,
    // including both space and cell blocks,
    // used to detect infinite loop in PointFinding
    int num_block_outline;
    int outlineWidth;
    int outlineHeight;

    // The linked list that stores the blocks in the outline,
    // both space and cell blocks
    list<Block> blocks;

    // An unordered map that stores the cell blocks in the outline,
    // u can use the block_id as the key to access the block
    unordered_map<int, list<Block>::iterator> Id2CellBlockPtr;

    // An unordered map that stores the space blocks in the outline,
    // u can use the block_id as the key to access the block
    unordered_map<int, list<Block>::iterator> Id2SpaceBlockPtr;

    // If the neighbor pointer points out of the outline,
    // it points to Void
    Block Void;

    // Stores the solution of the point finding instruction,
    // which is the LL corner of the block that contains the point
    vector<pair<int, int>> PointFindSol;

    // Stores the pointer to the cell blocks in ascending order of their block_id,
    // used for outputting the cell blocks in the correct order
    vector<Block *> CellBlockPtr;

    // accumulate the number of space blocks created throughout the process,
    // used to assign an unique block_id to every space block created,
    // including those that are later deleted
    int space_block_accumulate = 0;

public:
    Solver() : Void(make_pair(-1, -1), -1, -1, VOID, 0)
    {
        space_block_accumulate = 0;
        num_block_outline = 0;
        outlineWidth = -1;
        outlineHeight = -1;
    }

    ~Solver()
    {
    }

    int getCellBlockNum() const
    {
        return CellBlockPtr.size();
    }

    void SplitSpace_Hori(Block &block, int split_Y, bool original_on_top);
    void SplitSpace_Vert(Block &cell_block, Block &Top_Blcok, Block &Bottom_Block);

    void MergeSpace(vector<Block *> &space_blocks);

    void deleteSpaceBlock(Block *block);

    // Add a block(space or cell) to the outline.
    // The block is added to the end of the blocks list, O(1) time.
    // The block_id of the cell block is stored in the corresponding unordered map.
    Block *addBlock(const Block &block);

    // Main function to parse the input file and solve the problem
    void CMDParser(ifstream &in_file);

    // Write the output to the out_file
    void writeOutput(ostream &out);

    // Return the LL corner of the block that contains the point
    Block &PointFinding(pair<int, int> point /*, bool ignore_new_block = false */) const;

    // Insert a cell block into the outline,
    // update the neighbors of the blocks, and perform merge/split of the space block if necessary
    void InsertCellBlock(Block &block);

    // output the info for plotting
    void outputPlot(string filename)
    {
        ofstream out_file(filename);
        out_file << num_block_outline << endl;
        out_file << outlineWidth << " " << outlineHeight << endl;
        for (Block &b : blocks)
        {
            // cerr << b.block_id << " " << b.LL.first << " " << b.LL.second << " " << b.width << " " << b.height << endl;
            out_file << b.block_id << " " << b.LL.first << " " << b.LL.second << " " << b.width << " " << b.height << " ";
            // output the neighbor pointers
            vector<Block *> neighbor_ptr;
            neighbor_ptr.push_back(b.LL_Left);
            neighbor_ptr.push_back(b.LL_Bottom);
            neighbor_ptr.push_back(b.UR_Top);
            neighbor_ptr.push_back(b.UR_Right);

            for (int i = 0; i < neighbor_ptr.size(); i++)
            {
                Block *b_ptr = neighbor_ptr[i];
                if (b_ptr == nullptr)
                {
                    out_file << "n" << " ";
                }
                else if (b_ptr == &Void)
                {
                    out_file << "v" << " ";
                }
                else
                {
                    out_file << b_ptr->block_id << " ";
                }
            }
            out_file << endl;
        }
        out_file.close();
    }

    // Find the neighbors of the block
    vector<Block *> findNeighbors(const Block &block) const;
};

#endif