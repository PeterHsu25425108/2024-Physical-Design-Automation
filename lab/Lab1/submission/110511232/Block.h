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
    UNDEFINED,
    VOID
};

struct Block
{
    BlockType block_type;
    // The block_id of the cell block,
    // Positive integer: cell block
    // 0: Void
    // Negative integer: space block
    int block_id;
    pair<int, int> LL;
    int width, height;
    // The neighbor pointers of the block
    Block *LL_Bottom, *LL_Left, *UR_Top, *UR_Right;

    Block(pair<int, int> LL, int width, int height, BlockType block_type, int block_id = -1, Block *LL_Bottom = nullptr, Block *LL_Left = nullptr, Block *UR_Top = nullptr, Block *UR_Right = nullptr);

    Block()
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

    bool PointInBlock(pair<int, int> point, bool allow_right = false, bool allow_top = false) const;

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

    pair<int, int> getUR() const
    {
        return make_pair(LL.first + width, LL.second + height);
    }

    pair<int, int> getUL() const
    {
        return make_pair(LL.first, LL.second + height);
    }

    pair<int, int> getLR() const
    {
        return make_pair(LL.first + width, LL.second);
    }

    friend ostream &operator<<(ostream &os, const Block &b)
    {
        os << "Block id: " << b.block_id << " LL: " << b.LL.first << " " << b.LL.second << " width: " << b.width << " height: " << b.height;
        if (b.UR_Top == nullptr)
        {
            os << " UR_Top: nullptr";
        }
        else if (b.UR_Top->block_id == 0)
        {
            os << " UR_Top: Void";
        }
        else
        {
            os << " UR_Top: " << b.UR_Top->block_id;
        }

        if (b.LL_Bottom == nullptr)
        {
            os << " LL_Bottom: nullptr";
        }
        else if (b.LL_Bottom->block_id == 0)
        {
            os << " LL_Bottom: Void";
        }
        else
        {
            os << " LL_Bottom: " << b.LL_Bottom->block_id;
        }

        if (b.LL_Left == nullptr)
        {
            os << " LL_Left: nullptr";
        }
        else if (b.LL_Left->block_id == 0)
        {
            os << " LL_Left: Void";
        }
        else
        {
            os << " LL_Left: " << b.LL_Left->block_id;
        }

        if (b.UR_Right == nullptr)
        {
            os << " UR_Right: nullptr";
        }
        else if (b.UR_Right->block_id == 0)
        {
            os << " UR_Right: Void";
        }
        else
        {
            os << " UR_Right: " << b.UR_Right->block_id;
        }

        return os;
    }
};

#endif