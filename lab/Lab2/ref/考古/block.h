//===========================================================================
//
// NYCU MSEDA Lab       
//
// File Name   : block.h
// Function    : Block Class Delcaration ( 2022 Fall PDA Lab02 )
// Author      : Tracelementx ( Wei-Liang Chen )
// Version     : v1.0
//
//===========================================================================

#ifndef BLOCK_H
#define BLOCK_H

#include <string>
using namespace std;

class bsTree;
class nets;

class block
{
    friend class bsTree;
    friend class nets;

    public:
        block( string, int, int );
        ~block() {}

        void setName( string );
        void setSize( int, int );
        void setCooLB( int, int );
        void setCooRT( int, int );
        void setCooM();
        void setLPtr( block * ); 
        void setRPtr( block * ); 
        void setPPtr( block * ); 
        void setBlk( block * );

        string getName() const { return name; }
        int getW() const { return width; }
        int getH() const { return height; }
        int getXLB() const { return coo_xlb; }
        int getYLB() const { return coo_ylb; }
        int getXRT() const { return coo_xrt; }
        int getYRT() const { return coo_yrt; }
        block * getLPtr() const { return left_ptr; }
        block * getRPtr() const { return right_ptr; }
        block * getPPtr() const { return parent_ptr; }
        
    private:
        string name;
        int width;
        int height;
        int coo_xlb;
        int coo_ylb;
        int coo_xrt;
        int coo_yrt;
        block *left_ptr;
        block *right_ptr;
        block *parent_ptr;
};

#endif