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
void Solver::SplitSpace_Vert(Block &cell_block_in, Block &Top_Block, Block &Bottom_Block)
{

    if (DEBUG_INSERT)
    {
        cout << endl;
        cout << "SplitSpace_Vert: Splitting the space blocks vertically" << endl;
        cout << "Cell Block id: " << cell_block_in.block_id << " LL: " << cell_block_in.LL.first << " " << cell_block_in.LL.second << " width: " << cell_block_in.width << " height: " << cell_block_in.height << endl;
        if (Top_Block == Void)
        {
            cout << "Top Block is Void" << endl;
        }
        else
            cout << "Top Block id: " << Top_Block.block_id << " LL: " << Top_Block.LL.first << " " << Top_Block.LL.second << " width: " << Top_Block.width << " height: " << Top_Block.height << endl;
        cout << "Bottom Block id: " << Bottom_Block.block_id << " LL: " << Bottom_Block.LL.first << " " << Bottom_Block.LL.second << " width: " << Bottom_Block.width << " height: " << Bottom_Block.height << endl;
    }

    if (DEBUG_FINDNEIGH)
    {
        cout << "--- blocks before inserting cell block ---" << endl;
        for (Block &b : blocks)
        {
            cout << b;

            cout << endl;
        }

        cout << "-----------------------------------------" << endl;
    }

    // Add the cell block to the outline
    Block &cell_block = *addBlock(cell_block_in);

    if (DEBUG_FINDNEIGH)
    {
        cout << "--- blocks after inserting cell block ---" << endl;
        for (Block &b : blocks)
        {
            cout << b;

            cout << endl;
        }

        cout << "-----------------------------------------" << endl;
    }

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
        cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << " width: " << cell_block.width << " height: " << cell_block.height << endl;
        cerr << "Top Block id: " << Top_Block.block_id << " LL: " << Top_Block.LL.first << " " << Top_Block.LL.second << " width: " << Top_Block.width << " height: " << Top_Block.height << endl;
        cerr << "Bottom Block id: " << Bottom_Block.block_id << " LL: " << Bottom_Block.LL.first << " " << Bottom_Block.LL.second << " width: " << Bottom_Block.width << " height: " << Bottom_Block.height << endl;

        // print all the blocks
        for (Block &b : blocks)
        {
            cerr << "Block id: " << b.block_id << " LL: " << b.LL.first << " " << b.LL.second << " width: " << b.width << " height: " << b.height << endl;
        }

        exit(1);
    }

    if (DEBUG_FINDNEIGH)
    {
        cout << "--- blocks before space block finding ---" << endl;
        for (Block &b : blocks)
        {
            cout << b;

            cout << endl;
        }

        cout << "-----------------------------------------" << endl;
    }

    // Store the space blocks in between the top and bottom blocks
    vector<Block *> space_blocks;
    // For looking up the space blocks by their block_id
    unordered_map<int, Block *> space_blocks_map;

    Block *b = &Bottom_Block;

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
                cerr << "Split_ver: The block is " << (b->block_type == CELL ? "CELL" : "Void") << ", not a space block" << endl;
                cerr << "Block id: " << b->block_id << " LL: " << b->LL.first << " " << b->LL.second << endl;
                exit(1);
            }

            // check if b's bottom edge is the top edge of the cell block
            if (space_blocks.size() > 0 && b->getBottomY() != space_blocks.back()->getTopY())
            {
                cerr << "The bottom edge of the space block is not the top edge of the prev space block" << endl;
                cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << " width: " << cell_block.width << " height: " << cell_block.height << endl;
                // print the space blocks
                for (Block *sb : space_blocks)
                {
                    cerr << "Space Block id: " << sb->block_id << " LL: " << sb->LL.first << " " << sb->LL.second << " width: " << sb->width << " height: " << sb->height << endl;
                }
                exit(1);
            }

            space_blocks.push_back(b);
            space_blocks_map[b->block_id] = b;
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

    if (DEBUG_FINDNEIGH)
    {
        cout << "--- blocks before neighbor finding ---" << endl;
        for (Block &b : blocks)
        {
            cout << b;
            cout << endl;
        }

        cout << "-----------------------------------------" << endl;
    }

    vector<vector<Block *>> neighbors(space_blocks.size());

    // Find the neighbors of the space blocks
    for (int i = 0; i < space_blocks.size(); i++)
    {
        neighbors[i] = findNeighbors(*space_blocks[i]);
        // check if cell block is in the neighbors
        if (find(neighbors[i].begin(), neighbors[i].end(), &cell_block) != neighbors[i].end())
        {
            cerr << "The cell block is in the neighbors of the space block" << endl;
            cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << " width: " << cell_block.width << " height: " << cell_block.height << endl;
            cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
        }
    }

    if (DEBUG_INSERT)
    {
        cout << "Split_vert: Space blocks in between the top and bottom blocks" << endl;
        for (Block *sb : space_blocks)
        {
            cout << "Space Block id: " << sb->block_id << " LL: " << sb->LL.first << " " << sb->LL.second << " width: " << sb->width << " height: " << sb->height << endl;
        }
    }

    if (DEBUG_FINDNEIGH)
    {
        cout << "--- blocks after neighbor finding ---" << endl;
        for (Block &b : blocks)
        {
            cout << b;

            cout << endl;
        }

        cout << "-----------------------------------------" << endl;
    }

    // Split the space blocks on left and right, slice them vertically

    // Stores the space blocks created after the split of the space block on the left and right
    // first: left space block, second: right space block
    // The LL_Left,  UR_Right is assign when initializing the new space block
    vector<pair<Block *, Block *>> new_space_blocks(space_blocks.size(), make_pair(nullptr, nullptr));

    // 1. update the neighbor pointers of the space blocks so that they point to the new space blocks
    // or the cell block
    // 2. update the neighbor pointers of the cell block
    // 3. The new space block will be created and added to the outline
    for (int i = 0; i < space_blocks.size(); i++)
    {
        // Create the space blocks
        // case1: the space block is completely on the left of the cell block
        int leftX = space_blocks[i]->getLeftX();
        int rightX = space_blocks[i]->getRightX();

        if (leftX < cell_block.getLeftX())
        {
            // Create the new space block on the left
            int new_width = cell_block.getLeftX() - leftX;
            Block b_new(space_blocks[i]->LL, new_width, space_blocks[i]->height, SPACE, -1, &Void, space_blocks[i]->LL_Left, &Void, &cell_block);
            new_space_blocks[i].first = addBlock(b_new);
        }
        else if (leftX > cell_block.getLeftX())
        {
            // This should not happen
            cerr << "Abnormal space block on the left of the cell block" << endl;
            cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
            cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
            exit(1);
        }
        else // leftX == cell_block.getLeftX()
        {
            new_space_blocks[i].first = nullptr;
        }

        if (rightX > cell_block.getRightX())
        {
            // Create the new space block on the right
            int new_width = rightX - cell_block.getRightX();

            // The neighbor pointers of the new space block are not yet updated
            Block b_new(make_pair(cell_block.getRightX(), space_blocks[i]->LL.second), new_width, space_blocks[i]->height, SPACE, -1, &Void, &cell_block, &Void, space_blocks[i]->UR_Right);
            new_space_blocks[i].second = addBlock(b_new);
        }
        else if (rightX < cell_block.getRightX())
        {
            // This should not happen
            cerr << "Abnormal space block on the right of the cell block" << endl;
            cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
            cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
            exit(1);
        }
        else // rightX == cell_block.getRightX()
        {
            new_space_blocks[i].second = nullptr;
        }
    }

    if (DEBUG_INSERT)
    {
        cout << "Split_vert: New space blocks created" << endl;
        for (int i = 0; i < new_space_blocks.size(); i++)
        {
            cout << "Space Block " << *space_blocks[i] << endl;
            if (new_space_blocks[i].first != nullptr)
            {
                cout << "New Space Block on the left: " << *new_space_blocks[i].first << endl;
            }
            if (new_space_blocks[i].second != nullptr)
            {
                cout << "New Space Block on the right: " << *new_space_blocks[i].second << endl;
            }
        }
    }

    // update the LL_Left and UR_Right of the cell block
    if (new_space_blocks.size() != 0 && new_space_blocks[0].first != nullptr)
    {
        // cell block's LL_Left should be new_space_blocks[0].first
        cell_block.LL_Left = new_space_blocks[0].first;
    }
    else
    {
        // cell block's LL_Left should be space_blocks[0]->LL_Left
        cell_block.LL_Left = space_blocks[0]->LL_Left;
    }

    if (new_space_blocks.size() != 0 && new_space_blocks.back().second != nullptr)
    {
        // cell block's UR_Right should be new_space_blocks.back().second
        cell_block.UR_Right = new_space_blocks.back().second;
    }
    else
    {
        // cell block's UR_Right should be space_blocks.back()->UR_Right
        cell_block.UR_Right = space_blocks.back()->UR_Right;
    }

    // check if the UR_Top of the cell blcoks should be void
    if (cell_block.getTopY() == outlineHeight)
    {
        cell_block.UR_Top = &Void;
    }
    // check if the LL_Bottom of the cell block should be void
    if (cell_block.getBottomY() == 0)
    {
        cell_block.LL_Bottom = &Void;
    }

    for (int i = 0; i < space_blocks.size(); i++)
    {
        bool new_left = (new_space_blocks[i].first != nullptr);
        bool new_right = (new_space_blocks[i].second != nullptr);

        // Find the neighbors of the space block
        // vector<Block *> neighbors = findNeighbors(*space_blocks[i]);

        if (DEBUG_INSERT)
        {
            cout << endl;
            cout << "Split_vert: Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
            cout << "Split_vert: Neighbors of the space block" << endl;
            for (Block *neighbor : neighbors[i])
            {
                cout << *neighbor << endl;
            }
            cout << endl;
        }

        // Update the neighbor pointers of the space block
        for (Block *neighbor : neighbors[i])
        {

            if (neighbor == &Void)
            {
                cerr << "The neighbor is Void" << endl;
                cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                exit(1);
            }
            else if (neighbor == nullptr)
            {
                cerr << "The neighbor is nullptr" << endl;
                cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                exit(1);
            }

            // check if b is one of the space block to be split
            bool neighbor_split = space_blocks_map.find(neighbor->block_id) != space_blocks_map.end();

            // check which edge of the space block b is adjacent to space_blocks[i]
            bool left_edge = (neighbor->getRightX() == space_blocks[i]->getLeftX());
            bool right_edge = (neighbor->getLeftX() == space_blocks[i]->getRightX());
            bool top_edge = (neighbor->getBottomY() == space_blocks[i]->getTopY());
            bool bottom_edge = (neighbor->getTopY() == space_blocks[i]->getBottomY());

            // there should be only one edge that is adjacent to the cell block
            if ((int)left_edge + (int)right_edge + (int)top_edge + (int)bottom_edge != 1)
            {
                if ((int)left_edge + (int)right_edge + (int)top_edge + (int)bottom_edge > 1)
                {
                    cerr << "The space block has more than one edge adjacent to the space block" << endl;
                    cerr << "Adj edges: ";
                    if (left_edge)
                        cerr << "left ";
                    if (right_edge)
                        cerr << "right ";
                    if (top_edge)
                        cerr << "top ";
                    if (bottom_edge)
                        cerr << "bottom ";
                }
                else
                {
                    cerr << "The space block has no edge adjacent to the space block" << endl;
                }

                cerr << endl;
                cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                exit(1);
            }

            if (left_edge)
            {

                if (neighbor->UR_Right == space_blocks[i])
                {
                    neighbor->UR_Right = new_left ? new_space_blocks[i].first : &cell_block;
                }
            }
            else if (right_edge)
            {

                if (neighbor->LL_Left == space_blocks[i])
                {
                    neighbor->LL_Left = new_right ? new_space_blocks[i].second : &cell_block;
                }
            }
            else if (top_edge)
            {
                // check for update of the UR_Top of cell block
                if (neighbor->getLeftX() < cell_block.getRightX() && neighbor->getRightX() >= cell_block.getRightX() && neighbor->getBottomY() == cell_block.getTopY())
                {
                    cell_block.UR_Top = neighbor;
                }

                if (neighbor_split)
                {
                    if (i == new_space_blocks.size() - 1)
                    {
                        // not possible
                        cerr << "The highest space block to be split vertically have to_be_split top edge neighbor" << endl;
                        cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                        cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                        cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                        exit(1);
                    }

                    bool neighbor_new_left = new_left ? new_space_blocks[i + 1].first : &cell_block;
                    bool neighbor_new_right = new_right ? new_space_blocks[i + 1].second : &cell_block;

                    // check for update of the LL_Bottom of new space block
                    if (neighbor_new_left && new_left)
                    {
                        if (neighbor->LL_Bottom == space_blocks[i])
                        {
                            new_space_blocks[i + 1].first->LL_Bottom = new_space_blocks[i].first;
                        }

                        new_space_blocks[i].first->UR_Top = new_space_blocks[i + 1].first;
                    }

                    // check for update of the UR_Top of new space block
                    if (neighbor_new_right && new_right)
                    {
                        if (space_blocks[i]->UR_Top == neighbor)
                        {
                            new_space_blocks[i].second->UR_Top = new_space_blocks[i + 1].second;
                        }
                        new_space_blocks[i + 1].second->LL_Bottom = new_space_blocks[i].second;
                    }
                }
                else
                {

                    if (DEBUG_INSERT)
                    {
                        cout << "top edge non split neighbor" << neighbor->block_id << endl;
                        cout << "space block id: " << space_blocks[i]->block_id << endl;
                    }

                    // check which side of the cell block the neighbor is on
                    bool left_side = neighbor->getRightX() <= cell_block.getLeftX();
                    bool right_side = neighbor->getLeftX() >= cell_block.getRightX();

                    // check if the non split neighbor overlaps with the cell block
                    if (!left_side && !right_side && (i < space_blocks.size() - 1))
                    {
                        cerr << "Top edge: The non split neighbor overlaps with the cell block" << endl;
                        cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                        cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                        cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                        exit(1);
                    }
                    else if (left_side && right_side)
                    {
                        cerr << "The non split neighbor is on both side of the cell block" << endl;
                        cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                        cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                        cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                        exit(1);
                    }

                    else if (left_side)
                    {
                        if (!new_left)
                        {
                            cerr << "The non split neighbor is on the left side of the cell block, but the space block has no left space block" << endl;
                            cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                            cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                            cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                            exit(1);
                        }

                        // check for update of the LL_Bottom of the neighbor
                        if (neighbor->LL_Bottom == space_blocks[i])
                        {
                            neighbor->LL_Bottom = new_space_blocks[i].first;
                        }

                        // check for update of the UR_Top of new left space block
                        if (neighbor->getRightX() == cell_block.getLeftX())
                        {
                            new_space_blocks[i].first->UR_Top = neighbor;
                        }
                    }
                    else if (right_side)
                    {
                        if (!new_right)
                        {
                            cerr << "The non split neighbor is on the right side of the cell block, but the space block has no right space block" << endl;
                            cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                            cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                            cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                            exit(1);
                        }

                        // check for update of the LL_Bottom of the neighbor
                        if (neighbor->LL_Bottom == space_blocks[i])
                        {
                            neighbor->LL_Bottom = new_space_blocks[i].second;
                        }

                        // check for update of the UR_Top of new space block
                        if (space_blocks[i]->UR_Top == neighbor)
                        {
                            new_space_blocks[i].second->UR_Top = neighbor;
                        }
                    }
                    else
                    {
                        // check for update of the LL_Bottom of neighbor pointing to the cell block
                        if (neighbor->getLeftX() >= cell_block.getLeftX() && neighbor->getLeftX() < cell_block.getRightX())
                        {
                            neighbor->LL_Bottom = &cell_block;
                        }
                        else if (neighbor->getLeftX() < cell_block.getLeftX())
                        {
                            if (new_left && neighbor->LL_Bottom == space_blocks[i])
                                neighbor->LL_Bottom = new_space_blocks[i].first;
                        }
                        else
                        {
                            if (new_right && neighbor->LL_Bottom == space_blocks[i])
                                neighbor->LL_Bottom = new_space_blocks[i].second;
                        }

                        // check for update of the UR_Top of the new space block
                        if (new_left && neighbor->getLeftX() <= new_space_blocks[i].first->getRightX())
                        {
                            new_space_blocks[i].first->UR_Top = neighbor;
                        }

                        if (new_right && neighbor->getRightX() >= new_space_blocks[i].second->getRightX())
                        {
                            if (DEBUG_INSERT)
                            {
                                cout << endl
                                     << "new space block UR_Top update" << endl;
                                cout << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                                cout << "New Space Block id: " << new_space_blocks[i].second->block_id << " LL: " << new_space_blocks[i].second->LL.first << " " << new_space_blocks[i].second->LL.second << " width: " << new_space_blocks[i].second->width << " height: " << new_space_blocks[i].second->height << endl;
                                cout << endl;
                            }

                            new_space_blocks[i].second->UR_Top = neighbor;
                        }
                    }
                }
            }
            else // bottom_edge
            {
                // check for update of the LL_Bottom of cell block
                if (neighbor->getLeftX() <= cell_block.getLeftX() && neighbor->getRightX() > cell_block.getLeftX() && neighbor->getTopY() == cell_block.getBottomY())
                {
                    cell_block.LL_Bottom = neighbor;
                }

                if (neighbor_split)
                {
                    // not possible
                    if (i == 0)
                    {
                        cerr << "The lowest space block to be split vertically have to_be_split bottom edge neighbor" << endl;
                        cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                        cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                        cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                        exit(1);
                    }

                    bool neighbor_new_left = new_left ? new_space_blocks[i - 1].first : &cell_block;
                    bool neighbor_new_right = new_right ? new_space_blocks[i - 1].second : &cell_block;

                    if (neighbor_new_left && new_left)
                    {
                        if (space_blocks[i]->LL_Bottom == neighbor)
                        {
                            new_space_blocks[i].first->LL_Bottom = new_space_blocks[i - 1].first;
                        }

                        new_space_blocks[i - 1].first->UR_Top = new_space_blocks[i].first;
                    }

                    if (neighbor_new_right && new_right)
                    {
                        if (neighbor->UR_Top == space_blocks[i])
                        {
                            new_space_blocks[i - 1].second->UR_Top = new_space_blocks[i].second;
                        }
                        new_space_blocks[i].second->LL_Bottom = new_space_blocks[i - 1].second;
                    }
                }
                else
                {
                    // check which side of the cell block the neighbor is on
                    bool left_side = neighbor->getRightX() <= cell_block.getLeftX();
                    bool right_side = neighbor->getLeftX() >= cell_block.getRightX();

                    // check if the non split neighbor overlaps with the cell block
                    if (!left_side && !right_side && (i > 0))
                    {
                        cerr << "Bottom edge: The non split neighbor overlaps with the cell block" << endl;
                        cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                        cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                        cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                        exit(1);
                    }
                    else if (left_side && right_side)
                    {
                        cerr << "The non split neighbor is on both side of the cell block" << endl;
                        cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                        cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                        cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                        exit(1);
                    }

                    else if (left_side)
                    {
                        if (!new_left)
                        {
                            cerr << "The non split neighbor is on the left side of the cell block, but the space block has no left space block" << endl;
                            cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                            cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                            cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                            exit(1);
                        }

                        // check for update of the UR_Top of the neighbor
                        if (neighbor->UR_Top == space_blocks[i])
                        {
                            neighbor->UR_Top = new_space_blocks[i].first;
                        }

                        // check for update of the LL_Bottom of new left space block
                        if (space_blocks[i]->LL_Bottom == neighbor)
                        {
                            new_space_blocks[i].first->LL_Bottom = neighbor;
                        }
                    }
                    else if (right_side)
                    {
                        if (!new_right)
                        {
                            cerr << "The non split neighbor is on the right side of the cell block, but the space block has no right space block" << endl;
                            cerr << "Cell Block id: " << cell_block.block_id << " LL: " << cell_block.LL.first << " " << cell_block.LL.second << "width: " << cell_block.width << " height: " << cell_block.height << endl;
                            cerr << "Neighbor Block id: " << neighbor->block_id << " LL: " << neighbor->LL.first << " " << neighbor->LL.second << " width: " << neighbor->width << " height: " << neighbor->height << endl;
                            cerr << "Space Block id: " << space_blocks[i]->block_id << " LL: " << space_blocks[i]->LL.first << " " << space_blocks[i]->LL.second << " width: " << space_blocks[i]->width << " height: " << space_blocks[i]->height << endl;
                            exit(1);
                        }

                        // check for update of the UR_Top of the neighbor
                        if (neighbor->UR_Top == space_blocks[i])
                        {
                            neighbor->UR_Top = new_space_blocks[i].second;
                        }

                        // check for update of the LL_Bottom of new right space block
                        if (neighbor->getLeftX() == cell_block.getRightX())
                        {
                            new_space_blocks[i].second->LL_Bottom = neighbor;
                        }
                    }
                    else
                    {
                        // check for update of the UR_Top of neighbor pointing to the cell block
                        if (neighbor->getRightX() > cell_block.getLeftX() && neighbor->getRightX() <= cell_block.getRightX())
                        {
                            neighbor->UR_Top = &cell_block;
                        }
                        else if (neighbor->getRightX() <= cell_block.getLeftX() && neighbor->UR_Top == space_blocks[i])
                        {
                            if (new_left)
                                neighbor->UR_Top = new_space_blocks[i].first;
                        }
                        else
                        {
                            if (new_right && neighbor->UR_Top == space_blocks[i])
                                neighbor->UR_Top = new_space_blocks[i].second;
                        }

                        // check for update of the LL_Bottom of the new space block
                        if (new_left && neighbor->getLeftX() <= new_space_blocks[i].first->getLeftX())
                        {
                            new_space_blocks[i].first->LL_Bottom = neighbor;
                        }
                        if (new_right && neighbor->getRightX() > new_space_blocks[i].second->getLeftX())
                        {
                            new_space_blocks[i].second->LL_Bottom = neighbor;
                        }
                    }
                }
            }
        }
    }

    // merge the space blocks on the left and right of the cell block
    list<Block *> left_merge_space_blocks;
    list<Block *> right_merge_space_blocks;
    for (int i = 0; i < new_space_blocks.size(); i++)
    {
        if (new_space_blocks[i].first != nullptr)
        {
            left_merge_space_blocks.push_back(new_space_blocks[i].first);
        }

        if (new_space_blocks[i].second != nullptr)
        {
            right_merge_space_blocks.push_back(new_space_blocks[i].second);
        }
    }

    // Include the space blocks that are on top of the last space block in the left and right merge space blocks
    // and have the same LeftX and RightX into the left and right merge space blocks
    if (!left_merge_space_blocks.empty())
    {
        Block *left_top = left_merge_space_blocks.back()->UR_Top;
        Block *left_bottom = left_merge_space_blocks.front()->LL_Bottom;

        while (left_top != &Void && left_top->getLeftX() == left_merge_space_blocks.back()->getLeftX() && left_top->getRightX() == left_merge_space_blocks.back()->getRightX() && left_top->block_type == SPACE)
        {
            left_merge_space_blocks.push_back(left_top);
            left_top = left_top->UR_Top;
        }

        while (left_bottom != &Void && left_bottom->getLeftX() == left_merge_space_blocks.front()->getLeftX() && left_bottom->getRightX() == left_merge_space_blocks.front()->getRightX() && left_bottom->block_type == SPACE)
        {
            left_merge_space_blocks.push_front(left_bottom);
            left_bottom = left_bottom->LL_Bottom;
        }
    }

    if (!right_merge_space_blocks.empty())
    {
        Block *right_bottom = right_merge_space_blocks.front()->LL_Bottom;
        Block *right_top = right_merge_space_blocks.back()->UR_Top;

        while (right_bottom != &Void && right_bottom->getLeftX() == right_merge_space_blocks.front()->getLeftX() && right_bottom->getRightX() == right_merge_space_blocks.front()->getRightX() && right_bottom->block_type == SPACE)
        {
            right_merge_space_blocks.push_front(right_bottom);
            right_bottom = right_bottom->LL_Bottom;
        }

        while (right_top != &Void && right_top->getLeftX() == right_merge_space_blocks.back()->getLeftX() && right_top->getRightX() == right_merge_space_blocks.back()->getRightX() && right_top->block_type == SPACE)
        {
            right_merge_space_blocks.push_back(right_top);
            right_top = right_top->UR_Top;
        }
    }

    // merge the space blocks on the left and right of the cell block
    if (DEBUG_INSERT)
    {
        cout << endl
             << "Merge left space blocks" << endl;
        cout << "Left merge space blocks" << endl;
        for (Block *b : left_merge_space_blocks)
        {
            cout << *b << endl;
        }
    }
    MergeSpace(left_merge_space_blocks);
    if (DEBUG_INSERT)
    {
        cout << endl
             << "Merge right space blocks" << endl;
        cout << "Right merge space blocks" << endl;
        for (Block *b : right_merge_space_blocks)
        {
            cout << *b << endl;
        }
    }
    MergeSpace(right_merge_space_blocks);

    // Delete the space blocks that are split
    for (Block *b_delete : space_blocks)
    {
        if (DEBUG_INSERT)
        {
            cout << "Delete space block: " << *b_delete << endl;
        }

        deleteSpaceBlock(b_delete);
    }

    // Add the new space blocks to the outline
    /*for (int i = 0; i < new_space_blocks.size(); i++)
    {
        if (new_space_blocks[i].first != nullptr)
        {
            addBlock(*new_space_blocks[i].first);
        }

        if (new_space_blocks[i].second != nullptr)
        {
            addBlock(*new_space_blocks[i].second);
        }
    }*/
}

// split the block on top and bottom, slice them horizontally,
// the input block becomes the bottom block after the split
void Solver::SplitSpace_Hori(Block &space_block, int split_Y, bool original_on_top)
{

    if (DEBUG_INSERT)
    {
        cout << "original space block: " << space_block.block_id << " LL: " << space_block.LL.first << " " << space_block.LL.second << " width: " << space_block.width << " height: " << space_block.height << endl;
        for (Block &b : blocks)
        {
            cout << b << endl;
        }
    }

    // check if the block is a space block
    if (space_block.block_type != SPACE)
    {
        cerr << "Split_hori: The block is " << (space_block.block_type == CELL ? "CELL" : "Void") << ", not a space block" << endl;
        cerr << " LL: " << space_block.LL.first << " " << space_block.LL.second << endl;
        exit(1);
    }

    // first collect the top edge neighbors whose LL_Bottom is space_block and
    // the bottom edge neighbors whose UR_Top is space_block
    vector<Block *> top_edge_neighbors;
    Block *b = space_block.UR_Top;
    while (b != &Void && b->LL_Bottom == &space_block)
    {
        top_edge_neighbors.push_back(b);
        b = b->LL_Left;
    }

    vector<Block *> bottom_edge_neighbors;
    b = space_block.LL_Bottom;
    while (b != &Void && b->UR_Top == &space_block)
    {
        bottom_edge_neighbors.push_back(b);
        b = b->UR_Right;
    }

    // collect the left and right edge neighbors
    vector<Block *> left_edge_neighbors;
    b = space_block.LL_Left;
    while (b != &Void && b->UR_Right == &space_block)
    {
        left_edge_neighbors.push_back(b);
        b = b->UR_Top;
    }

    vector<Block *> right_edge_neighbors;
    b = space_block.UR_Right;
    while (b != &Void && b->LL_Left == &space_block)
    {
        right_edge_neighbors.push_back(b);
        b = b->LL_Bottom;
    }

    if (DEBUG_INSERT)
    {
        cout << "split_hori: top edge neighbors: ";
        for (Block *b : top_edge_neighbors)
        {
            cout << b->block_id << " ";
        }
        cout << endl;

        cout << "split_hori: bottom edge neighbors: ";
        for (Block *b : bottom_edge_neighbors)
        {
            cout << b->block_id << " ";
        }
        cout << endl;
    }

    // if (original_on_top)
    // {

    int b_new_topY = split_Y;
    int b_new_bottomY = space_block.getBottomY();
    int b_new_height = b_new_topY - b_new_bottomY;
    pair<int, int> b_new_LL = make_pair(space_block.LL.first, b_new_bottomY);

    space_block.height = space_block.getTopY() - split_Y;
    space_block.LL.second = split_Y;

    Block *b_new_LL_Bottom = space_block.LL_Bottom;
    Block *b_new_UR_Top = &space_block;
    Block *b_new_LL_Left = space_block.LL_Left;

    // update the LL_Left space_block and UR_Right of the new space block
    // Block *b_new_UR_Right = space_block.UR_Right;
    b = space_block.UR_Right;
    while (b != &Void && b->getBottomY() >= split_Y)
    {
        b = b->LL_Bottom;
    }
    Block *b_new_UR_Right = b;

    b = space_block.LL_Left;
    while (b != &Void && b->getTopY() <= split_Y)
    {
        b = b->UR_Top;
    }
    space_block.LL_Left = b;

    Block *b_new = addBlock(Block(b_new_LL, space_block.width, b_new_height, SPACE, -1, b_new_LL_Bottom, b_new_LL_Left, b_new_UR_Top, b_new_UR_Right));

    space_block.LL_Bottom = b_new;

    // update the UR_Top of the bottom edge neighbors
    for (Block *b : bottom_edge_neighbors)
    {
        b->UR_Top = b_new;
    }

    // update the LL_Bottom of the top edge neighbors
    for (Block *b : top_edge_neighbors)
    {
        b->LL_Bottom = &space_block;
    }

    // update the LL_Left of the right edge neighbors
    for (Block *b : right_edge_neighbors)
    {
        b->LL_Left = (b->getBottomY() >= split_Y) ? &space_block : b_new;
    }

    // update the UR_Right of the left edge neighbors
    for (Block *b : left_edge_neighbors)
    {
        b->UR_Right = (b->getTopY() <= split_Y) ? b_new : &space_block;
    }

    /*Block *right_edge_neighbor = space_block.UR_Right;
    if (right_edge_neighbor != &Void && right_edge_neighbor->LL_Left == &space_block)
    {
        right_edge_neighbor->LL_Left = b_new;
    }*/
    //}
    // else
    // {

    //     int b_new_bottomY = split_Y;
    //     int b_new_topY = space_block.getTopY();
    //     int b_new_height = b_new_topY - b_new_bottomY;
    //     space_block.height = split_Y - space_block.getBottomY();

    //     Block *b_new_LL_Bottom = &space_block;
    //     Block *b_new_LL_Left = space_block.LL_Left;
    //     Block *b_new_UR_Top = space_block.UR_Top;
    //     Block *b_new_UR_Right = space_block.UR_Right;

    //     Block *b_new = addBlock(Block(make_pair(space_block.LL.first, b_new_bottomY), space_block.width, b_new_height, SPACE, -1, b_new_LL_Bottom, b_new_LL_Left, b_new_UR_Top, b_new_UR_Right));
    //     // Block *b_new = &blocks.back();

    //     space_block.UR_Top = b_new;

    //     // update the UR_Top of the bottom edge neighbors
    //     for (Block *b : bottom_edge_neighbors)
    //     {
    //         b->UR_Top = &space_block;
    //     }

    //     // update the LL_Bottom of the top edge neighbors
    //     for (Block *b : top_edge_neighbors)
    //     {
    //         b->LL_Bottom = b_new;
    //     }

    //     // update the UR_Right of the left edge neighbors
    //     /*Block *left_edge_neighbor = space_block.LL_Left;
    //     if (left_edge_neighbor != &Void && left_edge_neighbor->UR_Right == &space_block)
    //     {
    //         left_edge_neighbor->UR_Right = &space_block;
    //     }*/
    //     // update the LL_Left of the right edge neighbors
    //     for (Block *b : right_edge_neighbors)
    //     {
    //         b->LL_Left = (b->getBottomY() >= split_Y) ? b_new : &space_block;
    //     }

    //     // update the UR_Right of the left edge neighbors
    //     for (Block *b : left_edge_neighbors)
    //     {
    //         b->UR_Right = (b->getTopY() <= split_Y) ? &space_block : b_new;
    //     }
    // }

    if (DEBUG_INSERT)
    {
        cout << "original space block after split_hori: " << space_block.block_id << " LL: " << space_block.LL.first << " " << space_block.LL.second << " width: " << space_block.width << " height: " << space_block.height << endl;
    }
}

// The input is a vector of space blocks on the same side(left or right) of the cell block,
// merge the space blocks in the vector into one space block,
// the space block instance in the first position of the vector becomes the merged block
void Solver::MergeSpace(list<Block *> &space_blocks)
{
    if (space_blocks.size() < 2)
    {
        return;
    }

    list<Block *>::iterator it = space_blocks.begin();
    it++;

    vector<Block *> merge_group;
    merge_group.push_back(*space_blocks.begin());

    while (it != space_blocks.end())
    {
        // if *it->width == merged_block->width, merge the blocks
        bool edge_aligned = ((*it)->getLeftX() == merge_group.back()->getLeftX() && (*it)->getRightX() == merge_group.back()->getRightX());
        // bool common_neighPtr = ((*it)->LL_Left == merge_group.back()->LL_Left && (*it)->UR_Right == merge_group.back()->UR_Right);
        bool same_width = (*it)->width == merge_group.back()->width;
        bool mergeable = edge_aligned && same_width; // common_neighPtr;

        if (DEBUG_INSERT)
        {
            cout << "merge_group: ";
            for (Block *b : merge_group)
            {
                cout << b->block_id << " ";
            }
            cout << endl;
        }

        if (mergeable)
        {

            if (DEBUG_INSERT)
            {
                cout << (*it)->block_id << " is mergeable with " << merge_group.back()->block_id << endl;
            }

            // add the block to the merge_group
            merge_group.push_back(*it);

            // if it is the last block in the space_blocks, merge the blocks in the merge_group if merge_group.size() > 1s
        }

        if (!mergeable || it == prev(space_blocks.end()))
        {

            if (DEBUG_INSERT)
            {
                cout << (*it)->block_id << " is not mergeable with " << merge_group.back()->block_id << endl;
            }

            // merge the blocks in the merge_group into bd
            if (merge_group.size() > 1)
            {
                if (DEBUG_INSERT)
                {
                    cout << "Merge the blocks: " << endl;
                    for (Block *b : merge_group)
                    {
                        cout << *b << endl;
                    }
                }

                // The block at the bottom of the merge_group
                Block *bd = merge_group[0];
                // The block at the top of the merge_group
                Block *bu = merge_group.back();

                if (DEBUG_INSERT)
                {
                    cout << "Merge into block: " << bd->block_id << endl;
                }

                /*Block *LL_Left_neighbor = bd->LL_Left;
                Block *UR_Right_neighbor = bu->UR_Right;

                if (LL_Left_neighbor != &Void && LL_Left_neighbor->UR_Right == bu)
                {
                    LL_Left_neighbor->UR_Right = bd;
                }*/

                // collect the left edge neighbors whose UR_Right is in the merge_group
                vector<Block *> left_edge_neighbors;
                Block *b = bd->LL_Left;
                while (b != &Void /*&& b->getTopY() > bd->getBottomY() */ && b->getTopY() <= bu->getTopY())
                {
                    left_edge_neighbors.push_back(b);
                    b = b->UR_Top;
                }

                // collect the right edge neighbors whose LL_Left is in the merge_group
                vector<Block *> right_edge_neighbors;
                b = bu->UR_Right;
                while (b != &Void && b->getBottomY() >= bd->getBottomY() /*&& b->getBottomY() < bu->getTopY()*/)
                {
                    right_edge_neighbors.push_back(b);
                    b = b->LL_Bottom;
                }

                // collect the top edge neighbors whose LL_Bottom is bu
                vector<Block *> top_edge_neighbors;
                b = bu->UR_Top;
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

                // point the UR_Right of the left edge neighbors to bd
                for (Block *b : left_edge_neighbors)
                {
                    b->UR_Right = bd;
                }

                // point the LL_Left of the right edge neighbors to bd
                for (Block *b : right_edge_neighbors)
                {
                    b->LL_Left = bd;
                }

                if (DEBUG_INSERT)
                {
                    cout << "top edge neighbors: ";
                    for (Block *b : top_edge_neighbors)
                    {
                        cout << b->block_id << " ";
                    }
                    cout << endl;

                    cout << "bottom edge neighbors: ";
                    for (Block *b : bottom_edge_neighbors)
                    {
                        cout << b->block_id << " ";
                    }

                    cout << endl;

                    cout << "left edge neighbors: ";
                    for (Block *b : left_edge_neighbors)
                    {
                        cout << b->block_id << " ";
                    }
                    cout << endl;

                    cout << "right edge neighbors: ";
                    for (Block *b : right_edge_neighbors)
                    {
                        cout << b->block_id << " ";
                    }

                    cout << endl;
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

                // clear the merge_group
                merge_group.clear();
                merge_group.push_back(*it);
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
        cerr << "Merge: The block is " << (block->block_type == CELL ? "CELL" : "Void") << ", not a space block" << endl;
        cerr << "Block id: " << block->block_id << " LL: " << block->LL.first << " " << block->LL.second << endl;
        exit(1);
    }

    // remove the block from the blocks list in O(1) time
    auto it = Id2SpaceBlockPtr.find(block->block_id);

    if (it == Id2SpaceBlockPtr.end())
    {
        cerr << "The block is not found in the Id2SpaceBlockPtr" << endl;
        cerr << "Block id: " << block->block_id << " LL: " << block->LL.first << " " << block->LL.second << endl;
        exit(1);
    }

    blocks.erase(it->second);

    // remove the block from the Id2SpaceBlockPtr
    Id2SpaceBlockPtr.erase(it);

    num_block_outline--;
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
    int block_middleX = (block.getLeftX() + block.getRightX()) / 2;

    pair<int, int> Top_CheckPoint = make_pair(block_middleX, block_TopY);
    pair<int, int> Bottom_CheckPoint = make_pair(block_middleX, block_BottomY);

    Block *Top_Block = &PointFinding(Top_CheckPoint);
    Block *Bottom_Block = &PointFinding(Bottom_CheckPoint);
    // Block & New_Top_Block = Top_Block;

    if (DEBUG_INSERT)
    {
        cout << "Top_CheckPoint: " << Top_CheckPoint.first << " " << Top_CheckPoint.second << endl;
        cout << "Bottom_CheckPoint: " << Bottom_CheckPoint.first << " " << Bottom_CheckPoint.second << endl;
        cout << "Top_Block: " << Top_Block->block_id << " LL: " << Top_Block->LL.first << " " << Top_Block->LL.second << " width: " << Top_Block->width << " height: " << Top_Block->height << endl;
        cout << "Bottom_Block: " << Bottom_Block->block_id << " LL: " << Bottom_Block->LL.first << " " << Bottom_Block->LL.second << " width: " << Bottom_Block->width << " height: " << Bottom_Block->height << endl;
    }

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
    else if (Bottom_Block == &Void)
    {
        cerr << "The Bottom_CheckPoint (" << Bottom_CheckPoint.first << ", " << Bottom_CheckPoint.second << ") is not found in any block, but it should be" << endl;
        cerr << "while inserting Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
        exit(1);
    }
    else if (/*Bottom_Block == Void ||*/ Bottom_Block->getBottomY() == block_BottomY)
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

        if (DEBUG_INSERT)
        {
            cout << "Splitting the top block" << endl;
        }
        SplitSpace_Hori(*Top_Block, block_TopY, true);

        if (Top_Block == Bottom_Block)
        {
            Bottom_Block = Bottom_Block->LL_Bottom;
        }
    }

    if (DEBUG_INSERT)
    {
        cout << endl
             << "===== blocks after Splitting the top block ======" << endl;
        for (Block &b : blocks)
        {
            cout << b;
            if (b.block_id == Top_Block->block_id)
            {
                cout << " <-- Top_Block";
            }
            if (b.block_id == Bottom_Block->block_id)
            {
                cout << " <-- Bottom_Block";
            }
            cout << endl;
        }
        cout << "======================================" << endl;
    }

    if (split_bottom)
    {

        if (DEBUG_INSERT)
        {
            cout << "Splitting the bottom block" << endl;
        }
        SplitSpace_Hori(*Bottom_Block, block_BottomY, true);

        if (DEBUG_INSERT)
        {
            cout << endl
                 << "blocks after Splitting the bottom block" << endl;
            for (Block &b : blocks)
            {
                cout << b;
                if (b.block_id == Bottom_Block->block_id)
                {
                    cout << " <-- Bottom_Block";
                }
                if (b.block_id == Top_Block->block_id)
                {
                    cout << " <-- Top_Block";
                }
                cout << endl;
            }
            cout << "======================================" << endl;
        }
    }

    // find the space blocks stacked in between the top and bottom blocks
    // split the block on left and right, slice them vertically
    // merge the block with the neighbors if possible

    SplitSpace_Vert(block, *Top_Block, *Bottom_Block);

    if ((DEBUG_INSERT || DEBUG_INSERTSTEP) && block.block_id == 800)
    {
        cout << endl
             << "===== After Inserting Block: " << block.block_id << " ======" << endl;
        for (Block &b : blocks)
        {
            cout << "Block id: " << b.block_id << " LL: " << b.LL.first << " " << b.LL.second << " width: " << b.width << " height: " << b.height;
            if (b.UR_Top == &Void)
            {
                cout << " UR_Top: Void";
            }
            else if (b.UR_Top != nullptr)
            {
                cout << " UR_Top: " << b.UR_Top->block_id;
            }
            else
            {
                cout << " UR_Top: nullptr";
            }

            if (b.LL_Bottom == &Void)
            {
                cout << " LL_Bottom: Void";
            }
            else if (b.LL_Bottom != nullptr)
            {
                cout << " LL_Bottom: " << b.LL_Bottom->block_id;
            }
            else
            {
                cout << " LL_Bottom: nullptr";
            }

            if (b.LL_Left == &Void)
            {
                cout << " LL_Left: Void";
            }
            else if (b.LL_Left != nullptr)
            {
                cout << " LL_Left: " << b.LL_Left->block_id;
            }
            else
            {
                cout << " LL_Left: nullptr";
            }

            if (b.UR_Right == &Void)
            {
                cout << " UR_Right: Void";
            }
            else if (b.UR_Right != nullptr)
            {
                cout << " UR_Right: " << b.UR_Right->block_id;
            }
            else
            {
                cout << " UR_Right: nullptr";
            }

            cout << endl;
        }
        cout << "======================================" << endl;
    }

    // print the outline

    if (DEBUG_OUTPUTPLOT)
    {
        string plot_name = "../layout/Insert_" + to_string(block.block_id) + ".txt";
        outputPlot(plot_name);
    }
}

// Find the neighbors of the block
vector<Block *> Solver::findNeighbors(const Block &block) const
{
    vector<Block *> neighbors;
    vector<Block *> left_edge_neighbors;
    vector<Block *> right_edge_neighbors;
    vector<Block *> top_edge_neighbors;
    vector<Block *> bottom_edge_neighbors;

    if (DEBUG_FINDNEIGH)
    {
        cout << "Finding the neighbor of block: " << block << endl;
    }

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
        while (b != &Void && b->getRightX() > block.getLeftX())
        {

            top_edge_neighbors.push_back(b);
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
        while (b != &Void && b->getTopY() > block.getBottomY())
        {
            if (b == &Void)
            {
                cerr << "The right edge search is out of the outline" << endl;
                cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
                exit(1);
            }

            right_edge_neighbors.push_back(b);
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
        while (b != &Void && b->getLeftX() < block.getRightX())
        {
            if (b == &Void)
            {
                cerr << "The bottom edge search is out of the outline" << endl;
                cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
                exit(1);
            }

            bottom_edge_neighbors.push_back(b);
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
        while (b != &Void && b->getBottomY() < block.getTopY())
        {
            if (b == &Void)
            {
                cerr << "The left edge search is out of the outline" << endl;
                cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
                exit(1);
            }

            left_edge_neighbors.push_back(b);
            b = b->UR_Top;
        }
    }

    if (DEBUG_INSERT)
    {
        // check if there is any neighbor that is not in the outline
        for (Block *b : neighbors)
        {

            /*if (b->block_type == CELL)
            {

                cerr << "cell neighbor" << b->block_id << endl;
            }*/

            if (Id2CellBlockPtr.find(b->block_id) != Id2CellBlockPtr.end() && Id2SpaceBlockPtr.find(b->block_id) != Id2SpaceBlockPtr.end())
            {
                cerr << "The neighbor is not in the outline" << endl;
                cerr << "Block id: " << block.block_id << " LL: " << block.LL.first << " " << block.LL.second << endl;
                cerr << "Neighbor id: " << b->block_id << " LL: " << b->LL.first << " " << b->LL.second << endl;
                // exit(1);
            }
        }
    }

    if (DEBUG_FINDNEIGH)
    {
        cout << "***** left edge neighbors: *****" << endl;
        for (Block *b : left_edge_neighbors)
        {
            if (b == nullptr)
            {
                cout << "nullptr" << endl;
            }
            else if (b == &Void)
            {
                cout << "Void" << endl;
            }
            else
            {
                cout << *b << endl;
            }
        }
        cout << "***** right edge neighbors: *****" << endl;
        for (Block *b : right_edge_neighbors)
        {
            if (b == nullptr)
            {
                cout << "nullptr" << endl;
            }
            else if (b == &Void)
            {
                cout << "Void" << endl;
            }
            else
            {
                cout << *b << endl;
            }
        }
        cout << "***** top edge neighbors: *****" << endl;
        for (Block *b : top_edge_neighbors)
        {
            if (b == nullptr)
            {
                cout << "nullptr" << endl;
            }
            else if (b == &Void)
            {
                cout << "Void" << endl;
            }
            else
            {
                cout << *b << endl;
            }
        }
        cout << "***** bottom edge neighbors: *****" << endl;
        for (Block *b : bottom_edge_neighbors)
        {
            if (b == nullptr)
            {
                cout << "nullptr" << endl;
            }
            else if (b == &Void)
            {
                cout << "Void" << endl;
            }
            else
            {
                cout << *b << endl;
            }
        }
        cout << "********************************" << endl;
    }

    neighbors.insert(neighbors.end(), left_edge_neighbors.begin(), left_edge_neighbors.end());
    neighbors.insert(neighbors.end(), right_edge_neighbors.begin(), right_edge_neighbors.end());
    neighbors.insert(neighbors.end(), top_edge_neighbors.begin(), top_edge_neighbors.end());
    neighbors.insert(neighbors.end(), bottom_edge_neighbors.begin(), bottom_edge_neighbors.end());
    return neighbors;
}

// Find the block that contains the point, if not found, return the Void block
// ignore_new_block is used to ignore the new block that is just added to the outline
Block &Solver::PointFinding(pair<int, int> point /*, bool ignore_new_block*/) const
{
    // Find the block that contains the point
    // Return the LL corner of the block

    /*if (ignore_new_block && blocks.size() < 2)
    {
        if (blocks.size() == 1)
            cerr << "The outline has only one block, cannot ignore the new block" << endl;
        else
            cerr << "The outline has no block, cannot ignore the new block" << endl;
        exit(1);
    }*/

    /*bool allow_top = point.second >= outlineHeight;
    bool allow_right = point.first >= outlineWidth;

    allow_top = false;
    allow_right = false;*/

    if (point.second >= outlineHeight)
    {
        if (DEBUG_INSERT || DEBUG_POINTFINDING)
        {
            cout << "The point is on the top edge of the outline" << endl;
        }

        return const_cast<Block &>(Void);
    }

    const Block *last_element_ptr = /*(ignore_new_block) ? prev(&blocks.back()) : */ &blocks.back();

    // Pick the front block in the outline
    Block *block = rand() % 2 ? const_cast<Block *>(&blocks.front()) : const_cast<Block *>(last_element_ptr);
    Block *prev_block = nullptr;
    Block *nxt_block;

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
        cout << "Starting Block: " << block->block_id << "x range: " << block->getLeftX() << "<= x <" << block->getRightX() << " y range: " << block->getBottomY() << "<= y <" << block->getTopY() << endl;
    }

    // Traverse the outline to find the block that contains the point
    bool horizontal = false;
    int iter = 0;

    while (block != &Void && block != nullptr)
    {

        bool inf_loop = false;

        if (DEBUG_POINTFINDING)
        {
            cout << "PointFinding While Iter: " << iter + 1 << endl;
            cout << "Block id: " << block->block_id << "x range: " << block->getLeftX() << "<= x <" << block->getRightX() << " y range: " << block->getBottomY() << "<= y <" << block->getTopY() << endl;
        }

        if (block->PointInBlock(point /*, allow_right, allow_top)*/))
        {
            // The point is found
            return *block;
        }
        // If the point is on the top or right edge of the block,
        // chances are the point is in the next block
        else if (point.second == block->getTopY() && block->UR_Top != &Void && block->UR_Right != nullptr)
        {
            if (block->UR_Top->PointInBlock(point /*, allow_right, allow_top*/))
            {
                return *(block->UR_Top);
            }

            nxt_block = block->UR_Top;
            if (nxt_block == prev_block)
            {
                cerr << "1. Previous Block id: " << prev_block->block_id << endl;
                cerr << "Next Block id: " << nxt_block->block_id << endl;
                cerr << "Current Block id: " << block->block_id << endl;
                inf_loop = true;
            }

            prev_block = block;
            block = block->UR_Top;
        }
        else if (point.first == block->getRightX() && block->UR_Right != &Void && block->UR_Right != nullptr)
        {
            if (block->UR_Right->PointInBlock(point /*, allow_right, allow_top*/))
            {
                return *(block->UR_Right);
            }

            nxt_block = block->UR_Right;
            if (nxt_block == prev_block)
            {
                cerr << "2. Previous Block id: " << prev_block->block_id << endl;
                cerr << "Next Block id: " << nxt_block->block_id << endl;
                cerr << "Current Block id: " << block->block_id << endl;
                inf_loop = true;
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
                    nxt_block = block->UR_Right;
                    if (nxt_block == prev_block)
                    {
                        cerr << "3. Previous Block id: " << prev_block->block_id << endl;
                        cerr << "Next Block id: " << nxt_block->block_id << endl;
                        cerr << "Current Block id: " << block->block_id << endl;
                        inf_loop = true;
                    }

                    prev_block = block;
                    block = block->UR_Right;
                }
                else
                {
                    nxt_block = block->LL_Left;
                    if (nxt_block == prev_block)
                    {
                        cerr << "4. Previous Block id: " << prev_block->block_id << endl;
                        cerr << "Next Block id: " << nxt_block->block_id << endl;
                        cerr << "Current Block id: " << block->block_id << endl;
                        inf_loop = true;
                    }

                    prev_block = block;
                    block = block->LL_Left;
                }
            }
            else
            {
                bool up = (point.second >= block->getTopY());

                if (up)
                {
                    nxt_block = block->UR_Top;
                    if (nxt_block == prev_block)
                    {
                        cerr << "5. Previous Block id: " << prev_block->block_id << endl;
                        cerr << "Next Block id: " << nxt_block->block_id << endl;
                        cerr << "Current Block id: " << block->block_id << endl;
                        inf_loop = true;
                    }

                    prev_block = block;
                    block = block->UR_Top;
                }
                else
                {
                    nxt_block = block->LL_Bottom;
                    if (nxt_block == prev_block)
                    {
                        cerr << "6. Previous Block id: " << prev_block->block_id << endl;
                        cerr << "Next Block id: " << nxt_block->block_id << endl;
                        cerr << "Current Block id: " << block->block_id << endl;
                        inf_loop = true;
                    }

                    prev_block = block;
                    block = block->LL_Bottom;
                }
            }
        }
        iter++;

        // The number of iterations should not exceed the number of blocks in the outline
        if (iter > num_block_outline || inf_loop)
        {
            cerr << "PointFinding stuck in an infinite loop" << endl;
            cerr << "Point: " << point.first << " " << point.second << endl;
            /*cerr << "Previous Block id: " << prev_block->block_id << endl;
            cerr << "Next Block id: " << nxt_block->block_id << endl;
            cerr << "Current Block id: " << block->block_id << endl;*/
            exit(1);
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

        if (DEBUG_INSERT || DEBUG_POINTFINDING)
        {
            cout << "The point is not in the block" << endl;
            cout << "Previous Block: " << *prev_block << endl;
        }

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

Block *Solver::addBlock(const Block &block)
{
    // block.block_id = -space_block_accumulate - 1;

    /*if (DEBUG_INSERT)
    {
        cout << "blocks before addBlock" << endl;
        for (Block &b : blocks)
        {
            cout << "Block id: " << b.block_id << " LL: " << b.LL.first << " " << b.LL.second << " width: " << b.width << " height: " << b.height << endl;
        }
    }*/

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

    /*if (DEBUG_INSERT)
    {
        cout << "blocks after addBlock" << endl;
        for (Block &b : blocks)
        {
            cout << "Block id: " << b.block_id << " LL: " << b.LL.first << " " << b.LL.second << " width: " << b.width << " height: " << b.height << endl;
        }
    }*/

    return &blocks.back();
}

void Solver::writeOutput(ostream &out)
{
    // The total number of cell blocks
    out << num_block_outline << endl;

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

        if (DEBUG_OUTPUT)
        {
            cout << "Neighbor of block: " << block->block_id << endl;
            for (Block *b : neighbors)
            {
                cout << *b << endl;
            }
        }

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