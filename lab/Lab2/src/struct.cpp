#include <iostream>
#include <vector>
#include "DEBUG.h"
#include "BSTree.h"
#include "struct.h"
#include <unordered_map>
#include <limits>
#include <cmath>
#include <algorithm>
#include <climits>
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

void Block::setParent(Block *parent)
{
    if (DEBUG_INSERT)
    {
        cout << "Setting parent of " << name << " to " << parent->name << endl;
    }
    this->parent = parent;
}

void Block::setLeft(Block *left)
{
    if (DEBUG_INSERT)
    {
        cout << "Setting left child of " << name << " to " << left->name << endl;
    }
    this->left = left;
}

void Block::setRight(Block *right)
{
    if (DEBUG_INSERT)
    {
        cout << "Setting right child of " << name << " to " << right->name << endl;
    }
    this->right = right;
}

void Block::setBL(Point BL)
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
        // print the parent, left, right of the block
        cerr << name << ".parent: ";
        if (parent == nullptr)
        {
            cerr << "nullptr" << endl;
        }
        else
        {
            cerr << parent->name << endl;
        }
        cerr << name << ".left: ";
        if (left == nullptr)
        {
            cerr << "nullptr" << endl;
        }
        else
        {
            cerr << left->name << endl;
        }
        cerr << name << ".right: ";
        if (right == nullptr)
        {
            cerr << "nullptr" << endl;
        }
        else
        {
            cerr << right->name << endl;
        }

        cerr << endl
             << "Parent name: ";
        if (parent == nullptr)
        {
            cerr << "nullptr" << endl;
        }
        else
        {
            cerr << parent->name << endl;
            cerr << parent->name << ".parent: ";
            if (parent->parent == nullptr)
            {
                cerr << "nullptr" << endl;
            }
            else
            {
                cerr << parent->parent->name << endl;
            }
            cerr << parent->name << ".left: ";
            if (parent->left == nullptr)
            {
                cerr << "nullptr" << endl;
            }
            else
            {
                cerr << parent->left->name << endl;
            }
            cerr << parent->name << ".right: ";
            if (parent->right == nullptr)
            {
                cerr << "nullptr" << endl;
            }
            else
            {
                cerr << parent->right->name << endl;
            }
        }
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

Point Block::getBL() const
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

Point Block::getTR() const
{
    return {BL.x + width, BL.y + height};
}

Point Block::getPinLoc() const
{
    /*double x_coor = double(BL.x + getTR().x) / 2.0;
    double y_coor = double(BL.y + getTR().y) / 2.0;*/
    // round the coordinate to the nearest integer
    /*int x = int(x_coor);
    int y = int(y_coor);*/

    int x = BL.x + width / 2; //(BL.x + getTR().x) / 2;
    int y = BL.y + height / 2; //(BL.y + getTR().y) / 2;

    return {x, y};
}

pair<double, double> Block::getPinLocDouble() const
{
    double x_coor = double(BL.x + getTR().x) / 2.0;
    double y_coor = double(BL.y + getTR().y) / 2.0;

    return {x_coor, y_coor};
}

void Net::calcHPWL()
{
    /*int x_min = numeric_limits<int>::max();
    int x_max = numeric_limits<int>::min();
    int y_min = numeric_limits<int>::max();
    int y_max = numeric_limits<int>::min();*/

    /*double x_min = numeric_limits<double>::max();
    double x_max = numeric_limits<double>::min();
    double y_min = numeric_limits<double>::max();
    double y_max = numeric_limits<double>::min();*/

    int x_min = INT_MAX;
    int x_max = INT_MIN;
    int y_min = INT_MAX;
    int y_max = INT_MIN;

    for (Pin &pin : pins)
    {
        Point loc;
        // pair<double, double> loc;
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
            // loc = block->getPinLocDouble();

            if (DEBUG_HPWL)
            {
                cout << "Block pin: " << pin.name << " loc: " << loc << endl;
                // cout << "Block pin: " << pin.name << " loc: " << loc.first << " " << loc.second << endl;
            }
        }
        else if (pin.pin_type == TERMINAL)
        {
            loc = BSTreePtr->getTermLoc(pin.name);
            // loc.first = BSTreePtr->getTermLoc(pin.name).x;
            // loc.second = BSTreePtr->getTermLoc(pin.name).y;
            if (DEBUG_HPWL)
            {
                cout << "Terminal pin: " << pin.name << " loc: " << loc << endl;
                // cout << "Terminal pin: " << pin.name << " loc: " << loc.first << " " << loc.second << endl;
            }
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

        /*x_max = max(x_max, loc.first);
        x_min = min(x_min, loc.first);
        y_max = max(y_max, loc.second);
        y_min = min(y_min, loc.second);*/
    }

    if (DEBUG_HPWL)
    {
        cout << "Net HPWL: " << (x_max - x_min) + (y_max - y_min) << endl;
    }

    HPWL = (int)((x_max - x_min) + (y_max - y_min));
}

void Net::addPin(string pin_name)
{
    if (DEBUG_PARSE)
    {
        if (BSTreePtr == nullptr)
        {
            cout << "BSTreePtr is nullptr" << endl;
        }
        cout << "Adding pin: " << pin_name << endl;
        BSTreePtr->findBlock(pin_name);
        cout << "test findBlock" << endl;
        BSTreePtr->getTermLoc(pin_name);
        cout << "test getTermLoc" << endl;
    }

    // check if the pin is a block pin or a terminal pin
    if (BSTreePtr->findBlock(pin_name) != nullptr)
    {
        pins.push_back({BLOCKPIN, pin_name});
    }
    else if (BSTreePtr->getTermLoc(pin_name).x != -1)
    {
        pins.push_back({TERMINAL, pin_name});
    }
    else
    {
        cerr << "ERROR: Net::addPin(" << pin_name << ") failed because " << pin_name << " is neither a block nor a terminal" << endl;
        exit(1);
    }
}