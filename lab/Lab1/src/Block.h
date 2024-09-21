#ifndef BLOCK_H
#define BLOCK_H
#include "DEBUG.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

enum BlockType
{
    CELL,
    SPACE,
    UNDEFINED
};

struct Block
{
    BlockType block_type;
    int block_id;
    pair<int, int> LL;
    int width, height;
    // The neighbor pointers of the block
    Block *LL_Bottom, *LL_Left, *UR_Top, *UR_Right;

    Block(int block_id, pair<int, int> LL, int width, int height, BlockType block_type);

    Block();

    void operator=(const Block &b);

    Block(const Block &b)
    {
        *this = b;
    }

    bool operator==(const Block &b) const
    {
        if (block_type == CELL && b.block_type == CELL)
            return block_id == b.block_id;
        else if (block_type == SPACE && b.block_type == SPACE)
            return (LL == b.LL) && (width == b.width) && (height == b.height);
        else
            return false;
    }

    bool adjacent(const Block &b) const;

    int getLeftX() const
    {
        return LL.first;
    }

    int getRightX() const
    {
        return LL.first + width;
    }

    int getBottomY() const
    {
        return LL.second;
    }

    int getTopY() const
    {
        return LL.second + height;
    }
};

#endif