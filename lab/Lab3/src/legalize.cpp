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
        bool isBanked = true;
        vector<string> removed_ffs;

        // get the first banked ff name
        ss >> temp;
        while (temp != "-->")
        {
            string ff_name = temp;
            removed_ffs.push_back(ff_name);
            ss >> temp;
        }

        // get the new ff name, LL_x,  LL_y, width, height
        string new_ff_name;
        double LL_x, LL_y;
        int width, height;
        ss >> new_ff_name >> LL_x >> LL_y >> width >> height;

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