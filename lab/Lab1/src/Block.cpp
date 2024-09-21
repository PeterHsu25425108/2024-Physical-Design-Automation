#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "Block.h"

using namespace std;

vector<int> Block::findNeighbors()
{
    vector<int> neighbors;

    // LL_Bottom, sweep rightward until the rightmost point of the block > getRightX()
    // or the block is at the rightmost of the outline

    // LL_Left

    // UR_Top

    // UR_Right

    return neighbors;
}

Block::Block(int block_id, pair<int, int> LL, int width, int height)
{
    this->block_id = block_id;
    this->LL = LL;
    this->width = width;
    this->height = height;
    this->LL_Bottom = nullptr;
    this->LL_Left = nullptr;
    this->UR_Top = nullptr;
    this->UR_Right = nullptr;
}

Block::Block()
{
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
    this->block_id = b.block_id;
    this->LL = b.LL;
    this->width = b.width;
    this->height = b.height;
    this->LL_Bottom = b.LL_Bottom;
    this->LL_Left = b.LL_Left;
    this->UR_Top = b.UR_Top;
    this->UR_Right = b.UR_Right;
}