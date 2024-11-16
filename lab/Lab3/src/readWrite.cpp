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
                // ff_onDie.push_back(name);
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

    // set the row idx
    /*for (int i = 0; i < placeRows.size(); i++)
    {
        placeRows[i].setRowIdx(i);
    }*/

    // set the lowest placerow y
    placeRows[0].setLowestPlaceRowY(placeRows[0].getStartY());

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
        if (DEBUG_PARSE)
        {
            cout << "Parsing: Adding " << ff->getName() << " to placerows" << endl;
        }

        addFF_PlaceRows(ff);
        // ff_posOnPLaceRow[ff->getName()] = make_pair(rowIdx, siteIdx);
    }

    if (DEBUG_PARSE)
    {
        cout << "Parsing done." << endl;
    }
}

void Solver::writeOutput(ofstream &output_file, Inst *new_ff, vector<string> moved_ff)
{
    // write the result of the legalizing operation of new_ff to the output file

    // write the new ff x, y
    output_file << new_ff->getX() << " " << new_ff->getY() << endl;
    output_file << moved_ff.size() << endl;
    // output name, x, y of the moved ffs
    for (auto &ff_name : moved_ff)
    {
        Inst *ff = ff_dict[ff_name];
        output_file << ff->getName() << " " << ff->getX() << " " << ff->getY() << endl;
    }
}