#ifndef STRUCT_H
#define STRUCT_H
#include"DEBUG.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
using namespace std;

enum MetalLayer
{ 
    M1, M2,
};

struct Bump
{
    int bump_idx, x, y;
    Bump(int bump_idx, int x, int y): bump_idx(bump_idx), x(x), y(y) {}
};

struct WireSeg
{
    int x1, y1, x2, y2;
    MetalLayer metal_layer;
    WireSeg(int x1, int y1, int x2, int y2, MetalLayer metal_layer): x1(x1), y1(y1), x2(x2), y2(y2), metal_layer(metal_layer) {}
};

struct Chip
{
    int llx, lly, width, height;
    vector<Bump> bumps;
    Chip(int llx, int lly, int width, int height): llx(llx), lly(lly), width(width), height(height) {}
};

struct Net
{
    int net_id;
    vector<WireSeg> wire_segs;
    Net(int net_id): net_id(net_id) {}
    double cost;
    void calNetCost()const;
};

#endif