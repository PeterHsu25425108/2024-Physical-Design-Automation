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

struct Block
{
    int block_id;
    pair<int, int> LL;
    int width, height;
    // Store the block_id of the 4 neighbors
    Block *LL_Bottom, *LL_Left, *UR_Top, *UR_Right;

    Block(int block_id, pair<int, int> LL, int width, int height);

    Block();

    void operator=(const Block &b);

    Block(const Block &b)
    {
        *this = b;
    }

    bool operator==(const Block &b) const
    {
        return block_id == b.block_id;
    }

    vector<int> findNeighbors();

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