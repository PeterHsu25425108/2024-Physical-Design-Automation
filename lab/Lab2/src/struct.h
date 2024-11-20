#ifndef STRUCT_H
#define STRUCT_H
#include <iostream>
#include <vector>
#include "DEBUG.h"
#include <unordered_map>
using namespace std;

struct Point
{
    int x;
    int y;
    void operator=(const Point &p);
    friend ostream &operator<<(ostream &os, const Point &p);
};

enum CHILD_TYPE
{
    LEFT_CHILD,
    RIGHT_CHILD,
    ROOT
};

class Block
{
private:
    string name;
    Point BL;
    int width, height;
    Block *parent, *left, *right;

public:
    Block();

    Block(const Block &block);

    Block(string name, int width, int height);
    ~Block() { ; }
    // Block(const Block &);

    void Rotate(Block *block);

    void setParent(Block *parent);

    void setLeft(Block *left);

    void setRight(Block *right);

    void setBL(Point BL);

    void setBLX(int x) { BL.x = x; }
    void setBLY(int y) { BL.y = y; }

    void setWidth(int width);

    void setHeight(int height);

    CHILD_TYPE getChildType() const;

    string getName() const;

    Block *getParent() const;

    Block *getLeft() const;

    Block *getRight() const;

    Point getBL() const;

    int getWidth() const;

    int getHeight() const;

    int getArea() const;

    Point getTR() const;

    // Return the location of the pin at
    // the center of the block
    Point getPinLoc() const;
    pair<double, double> getPinLocDouble() const;
};

enum PIN_TYPE
{
    BLOCKPIN,
    TERMINAL
};

struct Pin
{
    PIN_TYPE pin_type;
    string name;
};

class BSTree;

class Net
{
private:
    vector<Pin> pins;
    BSTree *BSTreePtr;
    int HPWL;

public:
    Net() { BSTreePtr = nullptr; }
    ~Net() { ; }
    Net(BSTree *BSTreePtr) : BSTreePtr(BSTreePtr) { ; }
    int getHPWL() const { return HPWL; }
    int getNetSize() const { return pins.size(); }
    vector<Pin> getPins() const { return pins; }

    void addPin(string pin_name);

    void calcHPWL();
};

#endif