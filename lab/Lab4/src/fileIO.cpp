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
                    chip1.addBump(Bump(bump_idx, bump_x, bump_y));

                    if(DEBUG_PARSING)
                    {
                        cout << "chip1 bump: " << chip1.bumps.back() << endl;
                    }

                    // add net
                    nets.push_back(Net(bump_idx));
                }
                else if(chip_idx==1)
                {
                    chip2.addBump(Bump(bump_idx, bump_x, bump_y));

                    if(DEBUG_PARSING)
                    {
                        cout << "chip2 bump: " << chip2.bumps.back() << endl;
                    }
                }

                getline(gmp_file, temp);
            }
            chip_idx++;
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

        grid[row_idx][col_idx].leftHorCap = leftEdgeCap;
        grid[row_idx][col_idx].bottomVerCap = rightEdgeCap;

        if(DEBUG_PARSING)
        {
            cout << "grid[" << row_idx << "][" << col_idx << "].leftHorCap: " << grid[row_idx][col_idx].leftHorCap << " grid[" << row_idx << "][" << col_idx << "].bottomVerCap: " << grid[row_idx][col_idx].bottomVerCap << endl;
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
                        M1Cost_max = max(M1Cost_max, cost);

                        if(DEBUG_PARSING)
                        {
                            cout << "grid[" << row_idx << "][" << col_idx << "].M1_cost: " << grid[row_idx][col_idx].M1_cost << endl;
                        }
                    }
                    else
                    {
                        grid[row_idx][col_idx].M2_cost = cost;
                        M2Cost_max = max(M2Cost_max, cost);

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

    MetalCost_max = max(M1Cost_max, M2Cost_max);
    if(DEBUG_PARSING)
    {
        cout << "M1Cost_max: " << M1Cost_max << " M2Cost_max: " << M2Cost_max << endl;
        cout << "MetalCost_max: " << MetalCost_max << endl;
    }
}

void Router::writeOutput(ofstream& out_file, const Net& net)
{

    out_file <<"n"<< net.net_id << endl;
    MetalLayer prev_layer  = M1;
    vector<WireSeg>::const_reverse_iterator rit;
    for(rit = net.wire_segs.rbegin(); rit != net.wire_segs.rend(); ++rit)
    {
        WireSeg wire_seg = *rit;
        if(prev_layer != wire_seg.metal_layer)
        {
            out_file << "via" << endl;
            prev_layer = wire_seg.metal_layer;
        }
        
        out_file << (wire_seg.metal_layer==M1 ? "M1" : "M2") << " " << wire_seg.x1 << " " << wire_seg.y1 << " " << wire_seg.x2 << " " << wire_seg.y2 << endl;
    
        // if the last wire segment is not on M1, add a via
        if(wire_seg.metal_layer == M2 && &wire_seg == &net.wire_segs.back())
        {
            out_file << "via" << endl;
        }
    }
    out_file<<".end"<<endl;
    
}