//===========================================================================
//
// NYCU MSEDA Lab       
//
// File Name   : bsTree.h
// Function    : B*-Tree Class Delcaration ( 2022 Fall PDA Lab02 )
// Author      : Tracelementx ( Wei-Liang Chen )
// Version     : v2.0
//
//===========================================================================

#ifndef BSTREE_H
#define BSTREE_H

#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "block.h"
#include "nets.h"
using namespace std;

struct coordinate
{
    double x;
    double y;
};

class bsTree
{
    friend class nets;

    public:
        bsTree();
        ~bsTree() {}

        void insertBlk( const string &, const int &, const int & );
        void copyTree( const bsTree & );
        
        void perturb( const int &, const int &, const int &, map< int, string >, bool & );
        block * find( const string & );
        void rotaBlk( block * );
        void moveBlk( block *, block * );
        void swapBlk( block *, block * );

        void setTour( block *, bool );
        void setCost( double );
        void setCostOut( int, int );
        void setPins( string, double, double );
        void setNetlist( nets * );
        
        block * getRoot() const { return root_ptr; }
        int getW() const { return width; }
        int getH() const { return height; }
        int getArea() const { return area; }
        int getTotHPWL() const { return tot_hpwl; }
        int getCost() const { return cost; }
        int getCostOut() const { return cost_out; }
        map< string, coordinate > getPins() const { return pins; }
        vector< nets * > getNetlist() const { return netlist; }
        void print_tree( block *, ofstream & );

    private:
        void insertBlkH( block **, block **, const string &, const int &, const int & );
        block * copyTreeNode( block *, block * );

        block *root_ptr;
        map< string, block * > blk;
        map< string, coordinate > pins;
        vector< nets * > netlist;
        vector< int > contour;
        int width;
        int height;
        int area;
        int tot_hpwl;
        int cost;
        int cost_out;
};

#endif
