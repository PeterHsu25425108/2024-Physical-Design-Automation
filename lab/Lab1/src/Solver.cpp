#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "Block.h"
#include "Solver.h"
#include <cctype>
#include <stdexcept>

using namespace std;

void Solver::InsertCellBlock(Block &block)
{
    // Find the neighbors of the block
    vector<int> neighbors = block.findNeighbors();

    // Check if the block is already in the outline
    // If it is, update the neighbors of the block
    // If it is not, insert the block into the outline
    // and update the neighbors of the block
}

pair<int, int> Solver::PointFinding(pair<int, int> point)
{
    // Find the block that contains the point
    // Return the LL corner of the block
}

void Solver::CMDParser(ifstream &in_file)
{
    in_file >> outlineWidth >> outlineHeight;
    // Create the first space block
    Block outline(-1, make_pair(0, 0), outlineWidth, outlineHeight, SPACE);

    string temp;
    while (getline(in_file, temp))
    {
        // Identify the type of instruction,
        // either "P" or the block_id
        stringstream ss(temp);
        string instruct_type;
        ss >> instruct_type;

        if (instruct_type == "P")
        {
            // Point findoing instruction
            int x, y;
            ss >> x >> y;

            // Find the block that contains the point (x, y)
            pair<int, int> LL_corner = PointFinding(make_pair(x, y));
        }
        else // block_id
        {
            // Parse the block_id, x, y, width, and height
            // and create a new block
            try
            {
                int block_id = stoi(instruct_type);
                int x, y, width, height;
                ss >> x >> y >> width >> height;
                Block block(block_id, make_pair(x, y), width, height, CELL);

                InsertCellBlock(block);
            }
            catch (const invalid_argument &e)
            {
                cerr << "Invalid block_id: " << instruct_type << endl;
            }
        }
    }
}

void Solver::writeOutput(ofstream &out_file)
{
    // The total number of cell blocks
    out_file << CellBlockPtr.size() << endl;

    // Sort CellBlockPtr in ascending order of block_id
    sort(CellBlockPtr.begin(), CellBlockPtr.end(), [](Block *a, Block *b)
         { return a->block_id < b->block_id; });

    // Output the cell blocks in the correct order
    for (Block *block : CellBlockPtr)
    {
        // block index, the number of adjacent block tiles, the number of adjacent space tiles
        int adjCell = 0, adjSpace = 0;

        // Perform Neighbor finding to calculate adjCell and adjSpace

        out_file << block->block_id << " " << adjCell << " " << adjSpace << endl;
    }
}