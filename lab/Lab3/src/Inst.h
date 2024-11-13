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
    Inst(string name, double x, double y, double width, double height, bool fixed) : name(name), x(x), y(y), width(width), height(height), fixed(fixed) {};
    Inst(const Inst &inst) : name(inst.name), x(inst.x), y(inst.y), width(inst.width), height(inst.height), fixed(inst.fixed) {};

    void setX(double x) { this->x = x; }
    void setY(double y) { this->y = y; }
    void setWidth(double width) { this->width = width; }
    void setHeight(double height) { this->height = height; }
    void setFixed(bool fixed) { this->fixed = fixed; }

    string getName() const { return name; }
    double getX() const { return x; }
    double getY() const { return y; }
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    bool getFixed() const { return fixed; }

    friend ostream &operator<<(ostream &os, const Inst &inst);

private:
    string name;
    double x, y;
    double width, height;
    bool fixed;
};

#endif