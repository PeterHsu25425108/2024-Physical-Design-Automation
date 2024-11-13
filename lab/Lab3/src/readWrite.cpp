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

void Solver::readlg(ifstream &lg_file)
{
    string head;
    while (lg_file >> head)
    {
        if (DEBUG_PARSE)
        {
            cout << "Parsing: " << head << endl;
        }

        if (head == "Alpha")
        {
            lg_file >> Alpha;

            if (DEBUG_PARSE)
            {
                cout << "Alpha: " << Alpha << endl;
            }
        }
        else if (head == "Beta")
        {
            lg_file >> Beta;

            if (DEBUG_PARSE)
            {
                cout << "Beta: " << Beta << endl;
            }
        }
        else if (head == "DieSize")
        {
            lg_file >> DieLLX >> DieLLY >> DieURX >> DieURY;

            if (DEBUG_PARSE)
            {
                cout << "DieSize: " << DieLLX << " " << DieLLY << " " << DieURX << " " << DieURY << endl;
            }
        }
        else if (head.substr(0, 2) == "FF" || head.substr(0, 1) == "C")
        {
            string name = head;
            bool fix;
            double x, y;
            double width, height;
            string temp;

            lg_file >> x >> y >> width >> height >> temp;
            fix = (temp == "FIXED") ? true : false;

            if (DEBUG_PARSE)
            {
                cout << name << " " << x << " " << y << " " << width << " " << height << " " << temp << endl;
            }

            if (head.substr(0, 2) == "FF")
            {
                // FF instantiation
                ff_onDie.push_back(name);
                ff_dict[name] = new Inst(name, x, y, width, height, fix);
            }
            else
            {
                // Cell instantiation
                gate_dict[name] = new Inst(name, x, y, width, height, fix);
            }
        }
        else if (head == "PlacementRows")
        {
            double startX, startY, siteWidth, siteHeight;
            int totNumSites;
            lg_file >> startX >> startY >> siteWidth >> siteHeight >> totNumSites;

            if (DEBUG_PARSE)
            {
                cout << "PlacementRows: " << startX << " " << startY << " " << siteWidth << " " << siteHeight << " " << totNumSites << endl;
            }

            // PlacementRows instantiation
            PlaceRow placeRow(startY);
            placeRow.setStartX(startX);
            placeRow.setSiteWidth(siteWidth);
            placeRow.setSiteHeight(siteHeight);
            placeRow.setNumSites(totNumSites);
            placeRows.push_back(placeRow);
        }
        else
        {
            cerr << "Solver::readlg: Unknown keyword: " << head << endl;
            exit(1);
        }
    }

    // sort placeRows by y coordinate
    sort(placeRows.begin(), placeRows.end());

    // Put the ff and cells into the placeRows
    for (auto &it : ff_dict)
    {
        Inst *ff = it.second;
        double x = ff->getX();
        double y = ff->getY();

        // calculate the row index and site index with the x and y coordinate
        int rowIdx = (y - placeRows[0].getStartY()) / placeRows[0].getSiteHeight();
        int siteIdx = (x - placeRows[0].getStartX()) / placeRows[0].getSiteWidth();

        // insert the ff into the placeRows
        placeRows[rowIdx].insertFF(ff, siteIdx);
        ff_posOnPLaceRow[ff->getName()] = make_pair(rowIdx, siteIdx);
    }

    if (DEBUG_PARSE)
    {
        cout << "Parsing done." << endl;
    }
}

void Solver::writeOutput(ofstream &output_file)
{
    // write output from result
    for (auto &it : result)
    {
        int merged_ff_x = ff_dict[it.first]->getX();
        int merged_ff_y = ff_dict[it.first]->getY();
        int num_of_moved_ff = it.second.size();

        output_file << it.first << " " << merged_ff_x << " " << merged_ff_y << endl;
        output_file << num_of_moved_ff << endl;
        for (auto &ff_name : it.second)
        {
            output_file << ff_name << " " << ff_dict[ff_name]->getX() << " " << ff_dict[ff_name]->getY() << endl;
        }
    }
}