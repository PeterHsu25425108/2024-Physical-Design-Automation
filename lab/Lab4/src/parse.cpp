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

                if(DEBUG_PARSING)
                {
                    cout <<"chip1 info: " << chip1.llx << " " << chip1.lly << " " << chip1.width << " " << chip1.height << endl;
                }
            }
            else if(chip_idx==1)
            {
                // calculate the absolute position of the chip
                chip2.llx = chip_x + GMP_LLX;
                chip2.lly = chip_y + GMP_LLY;
                chip2.width = chip_w;
                chip2.height = chip_h;

                if(DEBUG_PARSING)
                {
                    cout <<"chip2 info: " << chip2.llx << " " << chip2.lly << " " << chip2.width << " " << chip2.height << endl;
                }
            }
        }
        // bump info
        else if(line==".b")
        {
            int bump_idx, bump_x, bump_y;
            string temp;
            getline(gmp_file, temp);
            getline(gmp_file, temp);
            while(temp!="")
            {
                stringstream ss(temp);
                ss >> bump_idx >> bump_x >> bump_y;

                // calculate the absolute position of the bump
                Chip* chip_ptr = (chip_idx==0) ? &chip1 : &chip2;
                bump_x += chip_ptr->llx;
                bump_y += chip_ptr->lly;

                if(chip_idx==0)
                {
                    chip1.bumps.push_back(Bump(bump_idx, bump_x, bump_y));

                    if(DEBUG_PARSING)
                    {
                        cout << "chip1 bump: " << chip1.bumps.back() << endl;
                    }
                }
                else if(chip_idx==1)
                {
                    chip2.bumps.push_back(Bump(bump_idx, bump_x, bump_y));

                    if(DEBUG_PARSING)
                    {
                        cout << "chip2 bump: " << chip2.bumps.back() << endl;
                    }
                }

                getline(gmp_file, temp);
            }

        }
        else
        {
            cerr << "Error: unknown token "<<line<<" in GMP file" << endl;
            exit(1);
        }
    }

    // calculate the number of grids in the horizontal and vertical directions
    GRID_DIM_HOR = GMP_WIDTH / GRID_W;
    GRID_DIM_VER = GMP_HEIGHT / GRID_H;
    grid.resize(GRID_DIM_VER, vector<GCell>(GRID_DIM_HOR));

    if(DEBUG_PARSING)
    {
        cout << "GRID_DIM_HOR: " << GRID_DIM_HOR << " GRID_DIM_VER: " << GRID_DIM_VER << endl;
        cout << "grid.size(): " << grid.size() << " grid[0].size(): " << grid[0].size() << endl;
    }
}

void Router::parseGCL(ifstream& gcl_file)
{
    string line;
    // filter out .ec
    gcl_file >> line;
    int num_cells_processed=0;
    int leftEdgeCap, rightEdgeCap;

    while(gcl_file >> leftEdgeCap >> rightEdgeCap)
    {
        num_cells_processed++;
        // (UNDONE) check if the number of cells processed <= number of cells in the data structure
        if(num_cells_processed > GRID_DIM_HOR * GRID_DIM_VER)
        {
            cerr << "Error: number of cells processed exceeds the number of cells in the data structure" << endl;
            exit(1);
        }        
        
        // raster scan order
        int row_idx = (num_cells_processed-1) / GRID_DIM_HOR;
        int col_idx = (num_cells_processed-1) % GRID_DIM_HOR;

        grid[row_idx][col_idx].horCap = leftEdgeCap;
        grid[row_idx][col_idx].verCap = rightEdgeCap;

        if(DEBUG_PARSING)
        {
            cout << "grid[" << row_idx << "][" << col_idx << "].horCap: " << grid[row_idx][col_idx].horCap << " grid[" << row_idx << "][" << col_idx << "].verCap: " << grid[row_idx][col_idx].verCap << endl;
            //cout << "leftEdgeCap: " << leftEdgeCap << " rightEdgeCap: " << rightEdgeCap << endl;
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

            if(DEBUG_PARSING)
            {
                cout << "Alpha: " << Alpha << endl;
            }
        }
        else if(line==".beta")
        {
            cst_file >> Beta;

            if(DEBUG_PARSING)
            {
                cout << "Beta: " << Beta << endl;
            }
        }
        else if(line==".gamma")
        {
            cst_file >> Gamma;

            if(DEBUG_PARSING)
            {
                cout << "Gamma: " << Gamma << endl;
            }
        }
        else if(line==".delta")
        {
            cst_file >> Delta;

            if(DEBUG_PARSING)
            {
                cout << "Delta: " << Delta << endl;
            }
        }
        else if(line==".v")
        {
            cst_file >> ViaCost;

            if(DEBUG_PARSING)
            {
                cout << "ViaCost: " << ViaCost << endl;
            }
        }
        // layer info of layer[layer_idx]
        else if(line==".l")
        {
            int grid_idx=0;
            bool isM1 = (layer_idx==0) ? true : false;

            if(DEBUG_PARSING)
            {
                cout <<(isM1 ? "M1" : "M2") << " layer" << endl;
            }

            // raster scan order
            for(int row_idx=0; row_idx<GRID_DIM_VER; row_idx++)
            {
                for(int col_idx=0; col_idx<GRID_DIM_HOR; col_idx++)
                {
                    double cost;
                    cst_file >> cost;

                    if(isM1)
                    {
                        grid[row_idx][col_idx].M1_cost = cost;

                        if(DEBUG_PARSING)
                        {
                            cout << "grid[" << row_idx << "][" << col_idx << "].M1_cost: " << grid[row_idx][col_idx].M1_cost << endl;
                        }
                    }
                    else
                    {
                        grid[row_idx][col_idx].M2_cost = cost;
                        if(DEBUG_PARSING)
                        {
                            cout << "grid[" << row_idx << "][" << col_idx << "].M2_cost: " << grid[row_idx][col_idx].M2_cost << endl;
                        }
                    }
                }
            }
            layer_idx++;
        }
    }
}