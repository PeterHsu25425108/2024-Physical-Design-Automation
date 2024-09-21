#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "Block.h"

using namespace std;

Block::Block(int block_id, pair<int, int> LL, int width, int height, BlockType block_type)
{
    if (block_type == UNDEFINED)
    {
        cerr << "Block type is UNDEFINED" << endl;
        exit(1);
    }

    else if (block_type == CELL)
    {
        this->block_id = block_id;
        this->LL = LL;
        this->width = width;
        this->height = height;
        this->LL_Bottom = nullptr;
        this->LL_Left = nullptr;
        this->UR_Top = nullptr;
        this->UR_Right = nullptr;
        this->block_type = block_type;
    }
    else
    {
        this->block_id = -1;
        this->LL = LL;
        this->width = width;
        this->height = height;
        this->LL_Bottom = nullptr;
        this->LL_Left = nullptr;
        this->UR_Top = nullptr;
        this->UR_Right = nullptr;
        this->block_type = block_type;
    }
}

bool Block::adjacent(const Block &b) const
{
    // Return true only if the 2 blocks share an edge

    // case1: the blocks share a vertical edge
    if (this->LL.first == b.LL.first + b.width || this->LL.first + this->width == b.LL.first)
    {
        // check if the y coordinates overlap
        if (this->LL.second<b.LL.second + b.height &&this->LL.second + this->height> b.LL.second)
        {
            return true;
        }
    }
    // case2: the blocks share a horizontal edge
    else if (this->LL.second == b.LL.second + b.height || this->LL.second + this->height == b.LL.second)
    {
        // check if the x coordinates overlap
        if (this->LL.first<b.LL.first + b.width &&this->LL.first + this->width> b.LL.first)
        {
            return true;
        }
    }

    return false;
}

Block::Block()
{
    this->block_type = UNDEFINED;
    this->block_id = -1;
    this->LL = make_pair(-1, -1);
    this->width = -1;
    this->height = -1;
    this->LL_Bottom = nullptr;
    this->LL_Left = nullptr;
    this->UR_Top = nullptr;
    this->UR_Right = nullptr;
}

void Block::operator=(const Block &b)
{
    this->block_type = b.block_type;
    this->block_id = b.block_id;
    this->LL = b.LL;
    this->width = b.width;
    this->height = b.height;
    this->LL_Bottom = b.LL_Bottom;
    this->LL_Left = b.LL_Left;
    this->UR_Top = b.UR_Top;
    this->UR_Right = b.UR_Right;
}