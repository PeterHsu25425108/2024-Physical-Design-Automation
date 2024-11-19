#ifndef INST_H
#define INST_H
#include "DEBUG.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <iomanip>
#include <unordered_map>
using namespace std;

class Inst
{
public:
    Inst() {};
    ~Inst() {};
    Inst(string name, int x, int y, int width, int height, bool fixed) : name(name), x(x), y(y), width(width), height(height), fixed(fixed) {};
    Inst(const Inst &inst) : name(inst.name), x(inst.x), y(inst.y), width(inst.width), height(inst.height), fixed(inst.fixed) {};

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setWidth(int width) { this->width = width; }
    void setHeight(int height) { this->height = height; }
    void setFixed(bool fixed) { this->fixed = fixed; }

    string getName() const { return name; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool getFixed() const { return fixed; }

    friend ostream &operator<<(ostream &os, const Inst &inst);

private:
    string name;
    int x, y;
    int width, height;
    bool fixed;
};

#endif
