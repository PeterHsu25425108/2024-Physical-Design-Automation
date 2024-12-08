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
    Bump(){}
    ~Bump(){}
    Bump(int bump_idx, int x, int y): bump_idx(bump_idx), x(x), y(y) {}

    friend ostream& operator<<(ostream& os, const Bump& bump)
    {
        os << "bump "<< bump.bump_idx << " x: " << bump.x << " y: " << bump.y;
        return os;
    }
};

struct WireSeg
{
    int x1, y1, x2, y2;
    MetalLayer metal_layer;
    WireSeg(){}
    ~WireSeg(){}
    WireSeg(int x1, int y1, int x2, int y2, MetalLayer metal_layer): x1(x1), y1(y1), x2(x2), y2(y2), metal_layer(metal_layer) {}
    friend ostream& operator<<(ostream& os, const WireSeg& wire_seg)
    {
        os << "WireSeg: (" << wire_seg.x1 << ", " << wire_seg.y1 << ") -> (" << wire_seg.x2 << ", " << wire_seg.y2 << ") on " << (wire_seg.metal_layer==M1 ? "M1" : "M2");
        return os;
    }
};

struct Chip
{
    int llx, lly, width, height;
    vector<Bump> bumps;
    Chip(){}
    ~Chip(){}
    Chip(int llx, int lly, int width, int height): llx(llx), lly(lly), width(width), height(height) {}
    friend ostream& operator<<(ostream& os, const Chip& chip)
    {
        os << "Chip: llx: " << chip.llx << " lly: " << chip.lly << " width: " << chip.width << " height: " << chip.height;
        return os;
    }
};

struct GCell
{
    double M1_cost, M2_cost;
    int verCap, horCap;

    GCell(){}
    ~GCell(){}
    friend ostream& operator<<(ostream& os, const GCell& gcell)
    {
        os << "GCell: M1_cost: " << gcell.M1_cost << " M2_cost: " << gcell.M2_cost << " verCap: " << gcell.verCap << " horCap: " << gcell.horCap;
        return os;
    }
};

struct Net
{
    int net_id;
    vector<WireSeg> wire_segs;
    Net(int net_id): net_id(net_id) {}
    double cost;
    void calNetCost()const;

    Net(){}
    ~Net(){}

    friend ostream& operator<<(ostream& os, const Net& net)
    {
        os << "Net: " << net.net_id << " cost: " << net.cost << endl;
        os <<" ----- WireSegs ------ " << endl;
        for(auto wire_seg: net.wire_segs)
        {
            os << wire_seg << endl;
        }
        os <<"-----------------------" << endl;
        return os;
    }
};

#endif