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

// Split the block on left and right, slice them vertically
void Solver::SplitSpace_Vert(Block &cell_block, Block &Top_Block, Block &Bottom_Block)
{
    // check if the block is a cell block
    if (cell_block.block_type != CELL)
    {
        cerr << "The block is " << cell_block.block_type << ", not a cell block" << endl;
        cerr << "Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << endl;
        exit(1);
    }

    // Find the space block in between the top and bottom blocks
    if (&Top_Block == &Bottom_Block)
    {
        cerr << "The top and bottom blocks are the same" << endl;
        cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << endl;
        exit(1);
    }

    // Store the space blocks in between the top and bottom blocks
    // But not including the top and bottom blocks
    vector<Block *> space_blocks;
    Block *b = &Bottom_Block;
    // contains the new space blocks that are created after the split of the
    // space block below the current space block considered(b)
    pair<Block *, Block *> new_space_blocks = make_pair(nullptr, nullptr);

    while (b != &Top_Block && b->getBottomY() < cell_block.getTopY())
    {
        // check if b's x coordinate overlaps with the cell block
        int b_leftX = b->getLeftX();
        int b_RightX = b->getRightX();

        bool go_up = (b_leftX < cell_block.getRightX() && b_RightX > cell_block.getLeftX());

        // update b
        if (go_up)
        {
            // whatever block entering this if statement should be a space block
            // since it overlaps with the cell block
            if (b->block_type != SPACE)
            {
                cerr << "The block is " << b->block_type << ", not a space block" << endl;
                cerr << "Block id: " << b->block_id << " LL: " << b->LL.first << " " << b->LL.second << endl;
                exit(1);
            }

            space_blocks.push_back(b);
            b = b->UR_Top;
        }
        else
        {
            b = b->LL_Left;
        }

        if (b != &Top_Block && b == &Void)
        {
            cerr << "The space block is out of the outline" << endl;
            cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << endl;
            exit(1);
        }
        else if (b != &Top_Block && b == nullptr)
        {
            cerr << "The UR_Top pointer is a nullptr" << endl;
            cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << endl;
            exit(1);
        }
    }

    // Split the space blocks on left and right, slice them vertically, UNDONE
    // Q: how to update the neighbor ptr of the neighboring blocks of the split space blocks
}

// split the block on top and bottom, slice them horizontally
void Solver::SplitSpace_Hori(Block &space_block, int split_Y)
{
    // check if the block is a space block
    if (space_block.block_type != SPACE)
    {
        cerr << "The block is " << space_block.block_type << ", not a space block" << endl;
        cerr << " LL: " << space_block.LL.first << " " << space_block.LL.second << endl;
        exit(1);
    }

    space_block.height = split_Y - space_block.getBottomY();

    int b_new_bottomY = split_Y;
    int b_new_topY = space_block.getTopY();
    int b_new_height = b_new_topY - b_new_bottomY;

    Block *b_new_LL_Bottom = &space_block;
    Block *b_new_LL_Left = space_block.LL_Left;
    Block *b_new_UR_Top = space_block.UR_Top;
    Block *b_new_UR_Right = space_block.UR_Right;

    addBlock(Block(make_pair(space_block.LL.first, b_new_bottomY), space_block.width, b_new_height, SPACE, -1, b_new_LL_Bottom, b_new_LL_Left, b_new_UR_Top, b_new_UR_Right));
    Block *b_new = &blocks.back();

    space_block.UR_Top = b_new;
}

// The input is a vector of space blocks on the same side(left or right) of the cell block,
// merge the space blocks in the vector into one space block,
// the space block instance in the first position of the vector becomes the merged block
void Solver::MergeSpace(vector<Block *> &space_blocks)
{
    if (space_blocks.size() < 2)
    {
        return;
    }

    vector<Block *>::iterator it = space_blocks.begin() + 1;
    vector<Block *> merge_group;
    merge_group.push_back(*space_blocks.begin());

    while (it != space_blocks.end())
    {
        // if *it->width == merged_block->width, merge the blocks
        bool edge_aligned = ((*it)->getLeftX() == merge_group.back()->getLeftX() && (*it)->getRightX() == merge_group.back()->getRightX());
        bool common_neighPtr = ((*it)->LL_Left == merge_group.back()->LL_Left && (*it)->UR_Right == merge_group.back()->UR_Right);
        bool mergeable = edge_aligned && common_neighPtr;

        if (mergeable)
        {
            // add the block to the merge_group
            merge_group.push_back(*it);
        }
        else
        {
            // merge the blocks in the merge_group into bd
            if (merge_group.size() > 1)
            {
                // The block at the bottom of the merge_group
                Block *bd = merge_group[0];
                // The block at the top of the merge_group
                Block *bu = merge_group.back();

                Block *LL_Left_neighbor = bd->LL_Left;
                Block *UR_Right_neighbor = bu->UR_Right;

                if (LL_Left_neighbor != &Void && LL_Left_neighbor->UR_Right == bu)
                {
                    LL_Left_neighbor->UR_Right = bd;
                }

                // collect the top edge neighbors whose LL_Bottom is bu
                vector<Block *> top_edge_neighbors;
                Block *b = bu->UR_Top;
                while (b != &Void && b->LL_Bottom == bu)
                {
                    top_edge_neighbors.push_back(b);
                    b = b->LL_Left;
                }

                // collect the bottom edge neighbors whose UR_Top is bd
                vector<Block *> bottom_edge_neighbors;
                b = bd->LL_Bottom;
                while (b != &Void && b->UR_Top == bd)
                {
                    bottom_edge_neighbors.push_back(b);
                    b = b->LL_Left;
                }

                // point the UR_Top of the bottom edge neighbors to bd
                for (Block *b : bottom_edge_neighbors)
                {
                    b->UR_Top = bd;
                }

                // point the LL_Bottom of the top edge neighbors to bd
                for (Block *b : top_edge_neighbors)
                {
                    b->LL_Bottom = bd;
                }

                // update the UR_Top and UR_Right of bd
                bd->UR_Top = bu->UR_Top;
                bd->UR_Right = bu->UR_Right;

                // update the height of bd
                bd->height = bu->getTopY() - bd->getBottomY();

                // remove the blocks in the merge_group except bd from the outline
                for (Block *b : merge_group)
                {
                    if (b != bd)
                    {
                        // remove the block from the outline
                        deleteSpaceBlock(b);
                    }
                }
            }
            else
            {
                merge_group.clear();
                merge_group.push_back(*it);
            }
        }

        it++;
    }
}

// delete a space block from the outline, assuming the neighbors of the deleted block have been updated
// affect blocks, Id2SpaceBlockPtr
void Solver::deleteSpaceBlock(Block *block)
{
    if (block->block_type != SPACE)
    {
        cerr << "The block is " << block->block_type << ", not a space block" << endl;
        cerr << "Block id: " << block->block_id << " LL: " << block->LL.first << " " << block->LL.second << endl;
        exit(1);
    }

    // remove the block from the blocks list in O(1) time
    blocks.erase(Id2SpaceBlockPtr[block->block_id]);

    // remove the block from the Id2SpaceBlockPtr
    Id2SpaceBlockPtr.erase(block->block_id);

    // delete the block
    delete block;
}

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

    Block &Top_Block = PointFinding(Top_CheckPoint);
    Block &Bottom_Block = PointFinding(Bottom_CheckPoint);

    bool split_top, split_bottom;

    // if block_TopY == outlineHeight, the block is on the top edge of the outline, no need to split the block on top
    if (block_TopY == outlineHeight)
    {
        split_top = false;
    }
    // if Top_CheckPoint is not found in any block,
    //  or Top_CheckPoint.second is on the bottom edge of the block that contains the point,
    // then no need to split the top block
    else if (Top_Block == Void || Top_Block.getBottomY() == block_TopY)
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
    else if (Bottom_Block == Void)
    {
        cerr << "The Bottom_CheckPoint is not found in any block, but it should be" << endl;
        cerr << "while inserting Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
        exit(1);
    }
    else if (/*Bottom_Block == Void ||*/ Bottom_Block.getBottomY() == block_BottomY)
    {
        split_bottom = false;
    }
    else
    {
        split_bottom = true;
    }

    // split the block on top and bottom, slice them horizontally
    if (split_top)
    {
        SplitSpace_Hori(Top_Block, block_TopY);

        // update Top_Block to the block whose bottom edge
        // is the same as the top edge of the block
        Top_Block = *Top_Block.UR_Top;
    }

    if (split_bottom)
    {
        SplitSpace_Hori(Bottom_Block, block_BottomY);
    }

    // find the space blocks stacked in between the top and bottom blocks
    // split the block on left and right, slice them vertically
    // merge the block with the neighbors if possible
    SplitSpace_Vert(block, Top_Block, Bottom_Block);
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

                InsertCellBlock(block);
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
        Id2CellBlockPtr[block.block_id] = prev(blocks.end());
        CellBlockPtr.push_back(&blocks.back());
    }
    else if (block.block_type == SPACE)
    {
        blocks.back().block_id = -space_block_accumulate - 1;
        Id2SpaceBlockPtr[-space_block_accumulate - 1] = prev(blocks.end());
        space_block_accumulate++;
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