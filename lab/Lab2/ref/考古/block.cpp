//===========================================================================
//
// NYCU MSEDA Lab       
//
// File Name   : block.cpp
// Function    : Block Class Function Def. ( 2022 Fall PDA Lab02 )
// Author      : Tracelementx ( Wei-Liang Chen )
// Version     : v1.0
//
//===========================================================================

#include "block.h"

block::block( string n, int w, int h )
    : name( n ), width( w ), height( h ), 
      coo_xlb( 0 ), coo_ylb( 0 ), coo_xrt( 0 ), coo_yrt( 0 ), 
      left_ptr( 0 ), right_ptr( 0 ), parent_ptr( 0 )
{

}

//////////////////////////////////////////////////

void block::setName( string n )
{
    name = n;
}

void block::setSize( int w, int h )
{
    width = w;
    height = h;
}

void block::setCooLB( int xlb, int ylb )
{
    coo_xlb = xlb;
    coo_ylb = ylb;
}

void block::setCooRT( int xrt, int yrt )
{
    coo_xrt = xrt;
    coo_yrt = yrt;
}

void block::setLPtr( block *l_ptr )
{
    left_ptr = l_ptr;
}

void block::setRPtr( block *r_ptr )
{
    right_ptr = r_ptr;
}
void block::setPPtr( block *p_ptr )
{
    parent_ptr = p_ptr;
}

void block::setBlk( block *target )
{   
    left_ptr = target->left_ptr;
    right_ptr = target->right_ptr;
    parent_ptr = target->parent_ptr;

    if( target->parent_ptr != 0 )
    {
        if( target == target->parent_ptr->left_ptr )
            target->parent_ptr->left_ptr = this;
        else if( target == target->parent_ptr->right_ptr )
            target->parent_ptr->right_ptr = this;
    }
    if( target->left_ptr != 0 )
        target->left_ptr->parent_ptr = this;
        
    if( target->right_ptr != 0 )
        target->right_ptr->parent_ptr = this;
}