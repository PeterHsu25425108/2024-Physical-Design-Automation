//===========================================================================
//
// NYCU MSEDA Lab       
//
// File Name   : main.cpp
// Function    : Fixed Outline Floorplanning Using B*-Tree ( 2022 Fall PDA Lab02 )
// Author      : Tracelementx ( Wei-Liang Chen )
// Version     : v2.0
//
//===========================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <map>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "block.h"
#include "bsTree.h"
#include "nets.h"
using namespace std;

int main( int argc, char *argv[] )
{   
    srand( time( 0 ) + 902 );
    
    double alpha;
    alpha = stold( argv[1] );
    ifstream input_blk( argv[2], ios::in );
    ifstream input_nets( argv[3], ios::in );
    ofstream output( argv[4], ios::out );

    bsTree chip;
    bsTree best_chip;
    bsTree proc_chip;

    int outline_w, outline_h;
    int block_num;
    int termi_num;
    string in_string, in_buff;
    int in_blk_w, in_blk_h;
    double in_termi_x, in_termi_y;
    map< int, string > idx_name;
    int nets_num;
    int nets_degree;

    int opt; //0 rotate, 1 move, 2 swap
    int pick_idx0;
    int pick_idx1;
    int cost_diff = 0;
    double tptre;
    double r_factor = 0.99;
    double r_factor_optimize = 0.99;
    double prob = 100.0;
    double accept_check;
    int opt_cnt = 0;
    int uphill_cnt = 0;
    int reject_cnt = 0;
    int cnt = 20;
    int cnt_optimize = 8;
    double reject_ratio = 0.0;

    //////////////////////////////////////////////////
    //Initialize B*-tree & Pins

    input_blk >> in_string >> outline_w >> outline_h;
    input_blk >> in_string >> block_num;
    input_blk >> in_string >> termi_num;

    for( int i = 0; i < block_num; i++ )
    {
        input_blk >> in_string >> in_blk_w >> in_blk_h;
        chip.insertBlk( in_string, in_blk_w, in_blk_h );
        idx_name[i] = in_string;
    }

    chip.setTour( chip.getRoot(), true );
    chip.setCostOut( outline_w, outline_h );

    for( int i = 0; i < termi_num; i++ )
    {
        input_blk >> in_string >> in_buff >> in_termi_x >> in_termi_y;
        chip.setPins( in_string, in_termi_x, in_termi_y );
    }

    input_nets >> in_string >> nets_num;

    for( int i = 0; i < nets_num; i++ )
    {
        nets *new_nets;
        input_nets >> in_string >> nets_degree;
        new_nets = new nets( nets_degree );
        new_nets->setConnectPins( input_nets );
        chip.setNetlist( new_nets );
    }

    proc_chip.copyTree( chip );
    proc_chip.setCostOut( outline_w, outline_h );
    
    //////////////////////////////////////////////////
    //Perturb until it fits in outline by Simulated Annealing
    
    tptre = 999999.0;
    while( 1 )
    {      
        bool move_check = true;

        opt_cnt = 0;
        uphill_cnt = 0;
        reject_cnt = 0;

        while( 1 )
        {
            //Perturb
            opt = rand() % 3;
            pick_idx0 = rand() % block_num;
            pick_idx1 = rand() % block_num;
            
            while( pick_idx1 == pick_idx0 )
            {
                pick_idx1 = rand() % block_num;
            }
            
            proc_chip.perturb( opt, pick_idx0, pick_idx1, idx_name, move_check );

            if( opt == 1 && move_check == false )
                continue;

            /////////////////////////
                
            proc_chip.setTour( proc_chip.getRoot(), true );
            proc_chip.setCostOut( outline_w, outline_h );
            opt_cnt++;
            cost_diff = proc_chip.getCostOut() - chip.getCostOut();     

            if( cost_diff > 0 )
            {
                accept_check = (double)rand() / ( (double)RAND_MAX + 1.0 );
                prob = exp( (-1.0) * (double)cost_diff / tptre );

                //undo the operation
                if( accept_check > prob )
                {
                    proc_chip.copyTree( chip );
                    proc_chip.setCostOut( outline_w, outline_h );
                    reject_cnt++;
                }
                else
                    uphill_cnt++;
                    
            }
            else
            {
                chip.copyTree( proc_chip );
                chip.setCostOut( outline_w, outline_h );
                if( chip.getW() <= outline_w && chip.getH() <= outline_h )
                    break;  
            }
                
            if( uphill_cnt > cnt || opt_cnt > ( cnt * 2 ) )
                break;
        }

        if( chip.getW() <= outline_w && chip.getH() <= outline_h )
            break;

        tptre = r_factor * tptre;   

        reject_ratio = (double)reject_cnt / (double)opt_cnt;
        if( reject_ratio > 0.95 || tptre < 1.0 )
            tptre = 999.0;
    }

    chip.setCost( alpha );

    best_chip.copyTree( chip );
    best_chip.setCost( alpha );

    //////////////////////////////////////////////////
    //Optimizing by Simulated Annealing

    tptre = ( -1 ) * best_chip.getCost() / log( 0.99 );
    while( tptre > 1.0 )
    {      
        bool move_check = true;
        
        opt_cnt = 0;
        uphill_cnt = 0;
        reject_cnt = 0;

        while( ( (double)clock() / CLOCKS_PER_SEC ) <= 285.0 )
        {
            //Perturb
            opt = rand() % 3;
            pick_idx0 = rand() % block_num;
            pick_idx1 = rand() % block_num;
            
            while( pick_idx1 == pick_idx0 )
            {
                pick_idx1 = rand() % block_num;
            }
            
            proc_chip.perturb( opt, pick_idx0, pick_idx1, idx_name, move_check );

            if( opt == 1 && move_check == false )
                continue;

            /////////////////////////
                
            proc_chip.setTour( proc_chip.getRoot(), true );
            proc_chip.setCost( alpha );
            opt_cnt++;
            cost_diff = proc_chip.getCost() - chip.getCost(); 

            if( cost_diff > 0 )
            {
                accept_check = (double)rand() / ( (double)RAND_MAX + 1.0 );
                prob = exp( (-1.0) * (double)cost_diff / tptre );

                //undo the operation
                if( accept_check > prob )
                {
                    proc_chip.copyTree( chip );
                    proc_chip.setCost( alpha );
                    reject_cnt++;
                }
                else
                    uphill_cnt++;
            }
            else
            {
                chip.copyTree( proc_chip );
                chip.setCost( alpha );

                if( chip.getCost() < best_chip.getCost() && chip.getW() <= outline_w && chip.getH() <= outline_h )
                {
                    best_chip.copyTree( chip );
                    best_chip.setCost( alpha );
                }
            }

            if( uphill_cnt > cnt_optimize || opt_cnt > ( cnt_optimize * 2 ) )
            {
                proc_chip.copyTree( best_chip );
                proc_chip.setCost( alpha );
                break;
            }  
        }
        
        tptre = r_factor_optimize * tptre;
    }

    //////////////////////////////////////////////////
    //Output

    output << best_chip.getCost() << endl;
    output << best_chip.getTotHPWL() << endl;
    output << best_chip.getArea() << endl;
    output << best_chip.getW() << " " << best_chip.getH() << endl;
    output << ( (double)clock() / CLOCKS_PER_SEC ) << endl;
    chip.print_tree( best_chip.getRoot(), output );   

    //////////////////////////////////////////////////
    
    input_blk.close();
    input_nets.close();
    output.close();    

    return 0;
}
