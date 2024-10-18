#include <iostream>
#include <vector>
#include "DEBUG.h"
#include "BSTree.h"
#include "struct.h"
#include <unordered_map>
#include <limits>
using namespace std;

void Point::operator=(const Point &p)
{
    x = p.x;
    y = p.y;
}

ostream &operator<<(ostream &os, const Point &p)
{
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

Block::Block()
{
    parent = left = right = nullptr;
    name = "UNKNOWN";
}

Block::Block(string name, int width, int height)
{
    parent = left = right = nullptr;
    this->name = name;
    this->width = width;
    this->height = height;
}

void Block::Block::setParent(Block *parent)
{
    this->parent = parent;
}

void Block::Block::setLeft(Block *left)
{
    this->left = left;
}

void Block::Block::setRight(Block *right)
{
    this->right = right;
}

void Block::Block::setBL(Point BL)
{
    this->BL = BL;
}

void Block::setWidth(int width)
{
    this->width = width;
}

void Block::setHeight(int height)
{
    this->height = height;
}

void Block::Rotate(Block *block)
{
    int temp = block->width;
    block->width = block->height;
    block->height = temp;
}

CHILD_TYPE Block::getChildType() const
{
    if (parent == nullptr)
    {
        return ROOT;
    }
    else if (parent->left == this)
    {
        return LEFT_CHILD;
    }
    else if (parent->right == this)
    {
        return RIGHT_CHILD;
    }
    else
    {
        // print block name
        cerr << "ERROR: Block::getChildType() failed" << endl;
        cerr << "Block name: " << name << endl;
        exit(1);
    }
}

string Block::getName() const
{
    return name;
}

Block *Block::getParent() const
{
    return parent;
}

Block *Block::getLeft() const
{
    return left;
}

Block *Block::getRight() const
{
    return right;
}

Point Block::Block::getBL() const
{
    return BL;
}

int Block::getWidth() const
{
    return width;
}

int Block::getHeight() const
{
    return height;
}

int Block::getArea() const
{
    return width * height;
}

Point Block::Block::getTR() const
{
    return {BL.x + width, BL.y + height};
}

Point Block::Block::getPinLoc() const
{
    return {BL.x + width / 2, BL.y + height / 2};
}

void Net::calcHPWL()
{
    int x_min = numeric_limits<int>::max();
    int x_max = numeric_limits<int>::min();
    int y_min = numeric_limits<int>::max();
    int y_max = numeric_limits<int>::min();

    for (Pin &pin : pins)
    {
        Point loc;
        if (pin.pin_type == BLOCKPIN)
        {
            // get the block from the BSTree
            Block *block = BSTreePtr->findBlock(pin.name);
            if (block == nullptr)
            {
                cerr << "ERROR: Net::calcHPWL() failed because BSTreePtr->findBlock(" << pin.name << ") returns nullptr" << endl;
                exit(1);
            }
            loc = block->getPinLoc();
        }
        else if (pin.pin_type == TERMINAL)
        {
            loc = BSTreePtr->getTermLoc(pin.name);
        }
        else
        {
            cerr << "ERROR: Net::calcHPWL() failed becase of invalid pin type of pin: " << pin.name << endl;
            exit(1);
        }

        x_max = max(x_max, loc.x);
        x_min = min(x_min, loc.x);
        y_max = max(y_max, loc.y);
        y_min = min(y_min, loc.y);
    }

    HPWL = (x_max - x_min) + (y_max - y_min);
}