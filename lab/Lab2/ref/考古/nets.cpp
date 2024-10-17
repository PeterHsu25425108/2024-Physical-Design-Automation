//===========================================================================
//
// NYCU MSEDA Lab       
//
// File Name   : nets.cpp
// Function    : Nets Class Function Def. ( 2022 Fall PDA Lab02 )
// Author      : Tracelementx ( Wei-Liang Chen )
// Version     : v1.0
//
//===========================================================================

#include "nets.h"

nets::nets( const int &degree )
    : nets_degree( degree ), hpwl( 0 )
{

}

//////////////////////////////////////////////////

void nets::setConnectPins( ifstream &input_nets )
{
    string in_string;

    for( int k = 0; k < nets_degree; k++ )
    {
        input_nets >> in_string;
        connect_pins.push_back( in_string );
    }
}

void nets::setHPWL( bsTree *tree )
{
    double min_x = INT64_MAX;
    double max_x = 0;
    double min_y = INT64_MAX;
    double max_y = 0;
    double curr_x = 0;
    double curr_y = 0;

    vector< string >::iterator pins_it;
    for( pins_it = connect_pins.begin(); pins_it != connect_pins.end(); pins_it++ )
    {      
        curr_x = tree->pins[*pins_it].x;
        curr_y = tree->pins[*pins_it].y;

        if( curr_x < min_x )
            min_x = curr_x;

        if( curr_x > max_x )
            max_x = curr_x;

        if( curr_y < min_y )
            min_y = curr_y;

        if( curr_y > max_y )
            max_y = curr_y;
    }
    
    hpwl = (int)max_x - (int)min_x + (int)max_y - (int)min_y;
}