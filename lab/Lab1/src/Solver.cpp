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
    if (block.block_type != CELL)
    {
        cerr << "The block is " << block.block_type << ", not a cell block" << endl;
        cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
        exit(1);
    }

    int block_TopY = block.getTopY();
    int block_BottomY = block.getBottomY();
    int block_middleX = (block.getTopY() + block.getBottomY()) / 2;

    pair<int, int> Top_CheckPoint = make_pair(block_middleX, block_TopY);
    pair<int, int> Bottom_CheckPoint = make_pair(block_middleX, block_BottomY);

    Block *Top_Block = &PointFinding(Top_CheckPoint);
    Block *Bottom_Block = &PointFinding(Bottom_CheckPoint);

    bool split_top, split_bottom;

    // if block_TopY == outlineHeight, the block is on the top edge of the outline, no need to split the block on top
    if (block_TopY == outlineHeight)
    {
        split_top = false;
    }
    // if Top_CheckPoint is not found in any block,
    //  or Top_CheckPoint.second is on the bottom edge of the block that contains the point,
    // then no need to split the top block
    else if (Top_Block == &Void || Top_Block->getBottomY() == block_TopY)
    {
        split_top = false;
    }
    else
    {
        split_top = true;
    }

    // if block_BottomY == 0, the block is on the bottom edge of the outline, no need to split the block on bottom
    if (block_BottomY == 0)
    {
        split_bottom = false;
    }
    // if Bottom_CheckPoint is not found in any block,
    //  or Bottom_CheckPoint.second is on the bottom edge of the block that contains the point,
    // then no need to split the bottom block
    else if (Bottom_Block == &Void || Bottom_Block->getBottomY() == block_BottomY)
    {
        split_bottom = false;
    }
    else
    {
        split_bottom = true;
    }

    // split the block on top

    // split the block on bottom

    // find the left and right neighbors of the block

    // merge the block with the left neighbor if possible
}

// Find the neighbors of the block
vector<Block *> Solver::findNeighbors(const Block &block) const
{
    vector<Block *> neighbors;

    // Top edge search
    if (block.UR_Top == nullptr)
    {
        cerr << "The UR_Top pointer of the block is a nullptr" << endl;
        cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
        exit(1);
    }
    else if (block.UR_Top != &Void)
    {
        // Search the top edge of the block, go along the LL_Left pointer
        // Stop when the b->RightX <= block.LeftX
        Block *b = block.UR_Top;
        while (b->getRightX() > block.getLeftX() && b != &Void)
        {
            neighbors.push_back(b);
            b = b->LL_Left;
        }
    }

    // Right edge search
    if (block.UR_Right == nullptr)
    {
        cerr << "The UR_Right pointer of the block is a nullptr" << endl;
        cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
        exit(1);
    }
    else if (block.UR_Right != &Void)
    {
        // Search the right edge of the block, go along the LL_Bottom pointer
        // Stop when the b->TopY <= block.BottomY
        Block *b = block.UR_Right;
        while (b->getTopY() > block.getBottomY() && b != &Void)
        {
            if (b == &Void)
            {
                cerr << "The right edge search is out of the outline" << endl;
                cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
                exit(1);
            }
            neighbors.push_back(b);
            b = b->LL_Bottom;
        }
    }

    // Bottom edge search
    if (block.LL_Bottom == nullptr)
    {
        cerr << "The LL_Bottom pointer of the block is a nullptr" << endl;
        cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
        exit(1);
    }
    else if (block.LL_Bottom != &Void)
    {
        // Search the bottom edge of the block, go along the UR_Right pointer
        // Stop when the b->LeftX >= block.RightX
        Block *b = block.LL_Bottom;
        while (b->getLeftX() < block.getRightX() && b != &Void)
        {
            if (b == &Void)
            {
                cerr << "The bottom edge search is out of the outline" << endl;
                cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
                exit(1);
            }
            neighbors.push_back(b);
            b = b->UR_Right;
        }
    }

    // Left edge search
    if (block.LL_Left == nullptr)
    {
        cerr << "The LL_Left pointer of the block is a nullptr" << endl;
        cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
        exit(1);
    }
    else if (block.LL_Left != &Void)
    {
        // Search the left edge of the block, go along the UR_Top pointer
        // Stop when the b->BottomY >= block.TopY
        Block *b = block.LL_Left;
        while (b->getBottomY() < block.getTopY() && b != &Void)
        {
            if (b == &Void)
            {
                cerr << "The left edge search is out of the outline" << endl;
                cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
                exit(1);
            }
            neighbors.push_back(b);
            b = b->UR_Top;
        }
    }

    return neighbors;
}

// Find the block that contains the point, if not found, return the Void block
Block &Solver::PointFinding(pair<int, int> point) const
{
    // Find the block that contains the point
    // Return the LL corner of the block

    // Pick the front block in the outline
    Block *block = const_cast<Block *>(&blocks.front());
    Block *prev_block = nullptr;

    if (block == nullptr)
    {
        // The UR_Top pointer is a nullptr, which means it is not properly initialized
        cerr << "The block pointer is a nullptr" << endl;
        exit(1);
    }
    else if (block == &Void)
    {
        // The UR_Top pointer is a nullptr, which means it is not properly initialized
        cerr << "The UR_Top pointer is Void" << endl;
        exit(1);
    }

    if (DEBUG_POINTFINDING)
    {
        cout << "PointFinding Start" << endl;
        cout << "Point: " << point.first << " " << point.second << endl;
        cout << "Block: " << block->LL.first << " " << block->LL.second << endl;
    }

    // Traverse the outline to find the block that contains the point
    bool horizontal = false;
    int iter = 0;

    while (block != &Void && block != nullptr)
    {

        if (DEBUG_POINTFINDING)
        {
            cout << "PointFinding While Iter: " << iter + 1 << endl;
            cout << "Block: " << block->LL.first << " " << block->LL.second << endl;
        }

        if (block->PointInBlock(point))
        {
            // The point is found
            return *block;
        }
        // If the point is on the top or right edge of the block,
        // chances are the point is in the next block
        else if (point.second == block->getTopY() && block->UR_Top != &Void && block->UR_Right != nullptr)
        {
            if (block->UR_Top->PointInBlock(point))
            {
                return *(block->UR_Top);
            }

            prev_block = block;
            block = block->UR_Top;
        }
        else if (point.first == block->getRightX() && block->UR_Right != &Void && block->UR_Right != nullptr)
        {
            if (block->UR_Right->PointInBlock(point))
            {
                return *(block->UR_Right);
            }

            prev_block = block;
            block = block->UR_Right;
        }
        else
        {

            // Move to the next block
            // Determine the direction of the next block
            horizontal = (point.second >= block->getBottomY() && point.second <= block->getTopY());

            if (horizontal)
            {
                bool right = (point.first >= block->getRightX());

                if (right)
                {
                    prev_block = block;
                    block = block->UR_Right;
                }
                else
                {
                    prev_block = block;
                    block = block->LL_Left;
                }
            }
            else
            {
                bool up = (point.second >= block->getTopY());

                if (up)
                {
                    prev_block = block;
                    block = block->UR_Top;
                }
                else
                {
                    prev_block = block;
                    block = block->LL_Bottom;
                }
            }
        }
        iter++;

        // The number of iterations should not exceed the number of blocks in the outline
        if (iter > num_block_outline)
        {
            cerr << "PointFinding stuck in an infinite loop" << endl;
        }
    }

    // Error handling
    if (block == &Void)
    {
        // The point isn't found
        /*cerr << "The point is not in the block" << endl;
        cerr << "Previous Block x range: " << prev_block->getLeftX() << "<= x <" << prev_block->getRightX() << endl;
        cerr << "Previous Block y range: " << prev_block->getBottomY() << "<= y <" << prev_block->getTopY() << endl;
        exit(1);*/

        return const_cast<Block &>(Void);
    }
    else if (block == nullptr)
    {
        // The UR_Top pointer is a nullptr, which means it is not properly initialized
        cerr << "The UR_Top pointer is a nullptr" << endl;
        cerr << "Previous Block x range: " << prev_block->getLeftX() << "<= x <" << prev_block->getRightX() << endl;
        cerr << "Previous Block y range: " << prev_block->getBottomY() << "<= y <" << prev_block->getTopY() << endl;
        exit(1);
    }

    cerr << "PointFinding: The block pointer is uninitialized" << endl;
    exit(1);
}

void Solver::CMDParser(ifstream &in_file)
{
    in_file >> outlineWidth >> outlineHeight;
    // Create the first space block
    Block outline(make_pair(0, 0), outlineWidth, outlineHeight, SPACE, -1, &Void, &Void, &Void, &Void);
    addBlock(outline);

    if (DEBUG_PARSER)
    {
        cout << "Outline Width: " << outlineWidth << endl;
        cout << "Outline Height: " << outlineHeight << endl;

        cout << "Outline Block: " << endl;
        cout << "LL: " << blocks.front().LL.first << " " << blocks.front().LL.second << endl;
    }

    string instruct_type;
    while (in_file >> instruct_type)
    {
        // Identify the type of instruction,
        // either "P" or the block_id

        if (DEBUG_PARSER)
        {
            cout << "Instruction Type: " << instruct_type << endl;
        }

        if (instruct_type == "P")
        {
            // Point finding instruction
            int x, y;
            in_file >> x >> y;

            if (DEBUG_PARSER)
            {
                cout << "PointFinding input: " << x << " " << y << endl;
            }

            // Find the block that contains the point (x, y)
            Block block_found = PointFinding(make_pair(x, y));

            if (block_found == Void)
            {
                cerr << "PointFinding output Void" << endl;
                cerr << "Point: " << x << " " << y << endl;

                exit(1);
            }

            pair<int, int> LL_corner = block_found.LL;

            if (DEBUG_POINTFINDING)
            {
                cout << "Point: " << x << " " << y << endl;
                cout << "LL_corner: " << LL_corner.first << " " << LL_corner.second << endl;
            }

            PointFindSol.push_back(LL_corner);
        }
        else // block_id
        {
            // Parse the block_id, x, y, width, and height
            // and create a new block
            try
            {
                if (DEBUG_PARSER)
                {
                    cout << "block_id: " << instruct_type << endl;
                }

                int block_id = stoi(instruct_type);
                int x, y, width, height;
                in_file >> x >> y >> width >> height;

                if (DEBUG_PARSER)
                {
                    cout << "Insert Block input: " << block_id << " " << x << " " << y << " " << width << " " << height << endl;
                }

                Block block(make_pair(x, y), width, height, CELL, block_id, &Void, &Void, &Void, &Void);

                // InsertCellBlock(block);
            }
            catch (const invalid_argument &e)
            {
                cerr << "Invalid block_id: " << instruct_type << endl;
            }
        }
    }
}

void Solver::addBlock(const Block &block)
{
    blocks.push_back(block);

    // Store the pointer to the cell blocks in ascending order of their block_id
    if (block.block_type == CELL)
    {
        Id2CellBlockPtr[block.block_id] = &blocks.back();
        CellBlockPtr.push_back(&blocks.back());
    }

    // Update the number of blocks
    num_block_outline++;
}

void Solver::writeOutput(ostream &out)
{
    // The total number of cell blocks
    out << CellBlockPtr.size() << endl;

    // Sort CellBlockPtr in ascending order of block_id
    sort(CellBlockPtr.begin(), CellBlockPtr.end(), [](Block *a, Block *b)
         { return a->block_id < b->block_id; });

    // Output the cell blocks in the correct order
    for (Block *block : CellBlockPtr)
    {
        // block index, the number of adjacent block tiles, the number of adjacent space tiles
        int adjCell = 0, adjSpace = 0;

        // Perform Neighbor finding to calculate adjCell and adjSpace
        vector<Block *> neighbors = findNeighbors(*block);
        for (Block *neighbor : neighbors)
        {
            if (neighbor->block_type == CELL)
            {
                adjCell++;
            }
            else if (neighbor->block_type == SPACE)
            {
                adjSpace++;
            }
            else
            {
                cerr << "Invalid block type in writeOutput" << endl;
                exit(1);
            }
        }

        out << block->block_id << " " << adjCell << " " << adjSpace << endl;
    }

    // Output the solution of the point finding instruction
    for (pair<int, int> point : PointFindSol)
    {
        out << point.first << " " << point.second << endl;
    }
}