#include"DEBUG.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include"struct.h"
#include"Router.h"
using namespace std;

void Router::parseGMP(ifstream& gmp_file)
{
    string line;
    int chip_idx = 0;
    while(gmp_file >> line)
    {
        // routing area
        if(line == ".ra")
        {
            gmp_file >> GMP_LLX >> GMP_LLY >> GMP_WIDTH >> GMP_HEIGHT;

            if(DEBUG_PARSING)
            {
                cout << "GMP_LLX: " << GMP_LLX << " GMP_LLY: " << GMP_LLY << " GMP_WIDTH: " << GMP_WIDTH << " GMP_HEIGHT: " << GMP_HEIGHT << endl;
            }
        }
        // grid info
        else if(line==".g")
        {
            gmp_file >> GRID_H >> GRID_W;

            if(DEBUG_PARSING)
            {
                cout << "GRID_H: " << GRID_H << " GRID_W: " << GRID_W << endl;
            }
        }
        // chip info
        // new chip
        else if(line==".c")
        {
            int chip_x, chip_y, chip_w, chip_h;
            gmp_file >> chip_x >> chip_y >> chip_w >> chip_h;
            if(chip_idx==0)
            {
                // calculate the absolute position of the chip
                chip1.llx = chip_x + GMP_LLX;
                chip1.lly = chip_y + GMP_LLY;

                chip1.width = chip_w;
                chip1.height = chip_h;
            }
            else if(chip_idx==1)
            {
                // calculate the absolute position of the chip
                chip2.llx = chip_x + GMP_LLX;
                chip2.lly = chip_y + GMP_LLY;
                chip2.width = chip_w;
                chip2.height = chip_h;
            }
        }
        // bump info
        else if(line==".b")
        {
            int bump_idx, bump_x, bump_y;
            gmp_file >> bump_idx >> bump_x >> bump_y;

            // calculate the absolute position of the bump
            Chip* chip_ptr = (chip_idx==0) ? &chip1 : &chip2;
            bump_x += chip_ptr->llx;
            bump_y += chip_ptr->lly;

            if(chip_idx==0)
            {
                chip1.bumps.push_back(Bump(bump_idx, bump_x, bump_y));
            }
            else if(chip_idx==1)
            {
                chip2.bumps.push_back(Bump(bump_idx, bump_x, bump_y));
            }
        }
        else
        {
            cerr << "Error: unknown token in GMP file" << endl;
            exit(1);
        }
    }

    // calculate the number of grids in the horizontal and vertical directions
    GRID_DIM_HOR = GMP_WIDTH / GRID_W;
    GRID_DIM_VER = GMP_HEIGHT / GRID_H;
}

void Router::parseGCL(ifstream& gcl_file)
{
    string line;
    // filter out .ec
    getline(gcl_file, line);
    int num_cells_processed=0;
    while(gcl_file >> line)
    {
        num_cells_processed++;
        // (UNDONE) check if the number of cells processed <= number of cells in the data structure

        int leftEdgeCap, rightEdgeCap;
        // find the first index of the first space
        stringstream ss(line);
        ss >> leftEdgeCap >> rightEdgeCap;
        
        // raster scan order

        if(DEBUG_PARSING)
        {
            cout << "leftEdgeCap: " << leftEdgeCap << " rightEdgeCap: " << rightEdgeCap << endl;
        }
    }
}

void Router::parseCST(ifstream& cst_file)
{
    string line;
    int layer_idx=0;
    while(cst_file >> line)
    {
        if(line==".alpha")
        {
            cst_file >> Alpha;
        }
        else if(line==".beta")
        {
            cst_file >> Beta;
        }
        else if(line==".gamma")
        {
            cst_file >> Gamma;
        }
        else if(line==".delta")
        {
            cst_file >> Delta;
        }
        else if(line==".v")
        {
            cst_file >> ViaCost;
        }
        // layer info of layer[layer_idx]
        else if(line==".l")
        {
            int grid_idx=0;
            Chip* chip_ptr = (layer_idx==0) ? &chip1 : &chip2;

            // raster scan order
            

            layer_idx++;
        }
    }
}