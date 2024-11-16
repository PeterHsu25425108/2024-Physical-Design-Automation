#include "DEBUG.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>
#include <cmath>
#include <iomanip>
#include <unordered_map>
#include "Inst.h"
#include "Solver.h"
using namespace std;

vector<string> Solver::BruteFindInsertion(Inst *ff)
{
    vector<string> moved_ff;

    // to place the ff into an empty hole, we need to
    // 1. find a consecutive empty space that can accommodate the ff
    // 2. find the best position to place the ff in the empty space

    // find the row that the ff is placed
    int LL_rowIdx = ff_placeRowIdx(ff);
    // the number of rows the ff occupies
    int occRowCount = ceil(ff->getHeight() / placeRows[0].getSiteHeight());
    // the row index of the highest row the ff occupies
    int top_rowIdx = LL_rowIdx + occRowCount - 1;
    
    bool found_space = false;
    int numRowVisited = 0;
    int up_rowIdx = LL_rowIdx;
    int down_rowIdx = LL_rowIdx;
    bool go_down;

    if(LL_rowIdx == 0)
    {
        go_down = true;
    }
    else if(LL_rowIdx == placeRows.size()-1)
    {
        go_down = false;
    }
    else
    {
        go_down = rand()%2;
    }

    pair<double, double> Void = make_pair(-1, -1);
    while(numRowVisited < placeRows.size())
    {
        int curr_rowIdx = go_down ? down_rowIdx : up_rowIdx;
        // search the site on the current row
        PlaceRow &curr_row = placeRows[curr_rowIdx];
        pair<double, double> LL = curr_row.searchFFLL(ff, placeRows);
        
        if(LL != Void)
        {
            found_space = true;
            ff->setX(LL.first);
            ff->setY(LL.second);
            addFF_PlaceRows(ff);
            moved_ff.push_back(ff->getName());
            break;
        }

        if(found_space)
        {
            break;
        }

        // row updated when we can't find any free space that acomadate ff on current row
        if(go_down)
        {
            go_down = (down_rowIdx == 0) ? false : true;
            down_rowIdx = (down_rowIdx == 0) ? 0 : down_rowIdx - 1;
        }
        else
        {
            go_down = (up_rowIdx == placeRows.size()-1) ? true : false;
            up_rowIdx = (up_rowIdx == placeRows.size()-1) ? placeRows.size()-1 : up_rowIdx + 1;
        }

        numRowVisited++;
        if(numRowVisited == placeRows.size())
        {
            break;
        }
    }

    return moved_ff;
}

void Solver::solve(ifstream &opt_file, ofstream &output_file)
{
    string temp;
    while (getline(opt_file, temp))
    {
        stringstream ss(temp);
        // remove the "Banking_Cell: " part
        ss >> temp;

        if(DEBUG_PARSE)
        {
            cout << "Parsing: " << temp << endl;
        }

        bool isBanked = true;
        vector<string> removed_ffs;

        // get the first banked ff name
        ss >> temp;
        while (temp != "-->")
        {
            if(DEBUG_PARSE)
            {
                cout << "Parsing: " << temp << endl;
            }

            string ff_name = temp;
            removed_ffs.push_back(ff_name);
            ss >> temp;
        }

        // get the new ff name, LL_x,  LL_y, width, height
        string new_ff_name;
        double LL_x, LL_y;
        int width, height;
        ss >> new_ff_name >> LL_x >> LL_y >> width >> height;

        if(DEBUG_PARSE)
        {
            cout << "Parsing: " << new_ff_name << " " << LL_x << " " << LL_y << " " << width << " " << height << endl;
        }

        // insert the new ff into the layout, but it is not legalized yet
        Inst *new_ff = new Inst(new_ff_name, LL_x, LL_y, width, height, false);
        ff_dict[new_ff_name] = new_ff;
        // ff_onDie.push_back(new_ff_name);

        // remove the old ffs from the layout
        for (auto &ff_name : removed_ffs)
        {
            // remove the ff from the ff_dict
            // ff_onDie.remove(ff_name);
            delete ff_dict[ff_name];
            ff_dict.erase(ff_name);

            // remove the ff from the placeRow
            removeFF_PlaceRows(ff_dict[ff_name]);
        }
        // legalizing the new ff, this is where the legalization method is called
        // the position of the new ff or any other cells displaced and the placerows will be updated
        vector<string> moved_ff = BruteFindInsertion(new_ff);

        // write the result to the output file
        writeOutput(output_file, new_ff, moved_ff);
    }
}