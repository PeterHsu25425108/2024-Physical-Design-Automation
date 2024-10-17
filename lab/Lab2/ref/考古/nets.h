//===========================================================================
//
// NYCU MSEDA Lab       
//
// File Name   : nets.h
// Function    : Nets Class Delcaration ( 2022 Fall PDA Lab02 )
// Author      : Tracelementx ( Wei-Liang Chen )
// Version     : v1.0
//
//===========================================================================

#ifndef NETS_H
#define NETS_H

#include <string>
#include <vector>
#include <limits>
#include "bsTree.h"
using namespace std;

class nets
{
    public:
        nets( const int & );
        ~nets() {}

        void setConnectPins( ifstream & );
        void setHPWL( bsTree * );

        int getHPWL() { return hpwl; }

    private:
        int nets_degree;
        int hpwl;
        vector< string > connect_pins;
};

#endif