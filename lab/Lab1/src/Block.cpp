#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "Block.h"

using namespace std;

Block::Block(std::pair<int, int> LL, int width, int height, BlockType block_type, int block_id, Block *LL_Bottom, Block *LL_Left, Block *UR_Top, Block *UR_Right)
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
        this->LL_Bottom = LL_Bottom;
        this->LL_Left = LL_Left;
        this->UR_Top = UR_Top;
        this->UR_Right = UR_Right;
        this->block_type = block_type;
    }
    else
    {
        this->LL = LL;
        this->width = width;
        this->height = height;
        this->LL_Bottom = LL_Bottom;
        this->LL_Left = LL_Left;
        this->UR_Top = UR_Top;
        this->UR_Right = UR_Right;
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

bool Block::PointInBlock(pair<int, int> point) const
{
    return (point.first >= LL.first && point.first < LL.first + width && point.second >= LL.second && point.second < LL.second + height);
}