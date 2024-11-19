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
    bool first_placeRow = true;
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
            fix = (temp == "FIX") ? true : false;

            if (DEBUG_PARSE)
            {
                cout << name << " " << x << " " << y << " " << width << " " << height << " " << (fix ? "FIX" : "NOTFIX") << endl;
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

            if (first_placeRow)
            {
                first_placeRow = false;
                PlaceRow::startX = startX;
                PlaceRow::siteWidth = siteWidth;
                PlaceRow::siteHeight = siteHeight;
                PlaceRow::numSites = totNumSites;
            }
            PlaceRow placeRow(startY);
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

    if (DEBUG_PARSE)
    {
        // print the free sites of the first placerow
        cout << "Parsing: Printing the free sites of the first placerow." << endl;
        for (auto &site : placeRows[0].getFreeSites())
        {
            cout << "x: " << site.first << " width: " << site.second << endl;
        }
        cout << "placeRows[0].getStartX(): " << placeRows[0].getStartX() << endl;
        cout << "placeRows[0].getStartY(): " << placeRows[0].getStartY() << endl;

        // print the free sites whose one and only one site is the entire row
        for (auto &placeRow : placeRows)
        {
            if (placeRow.getFreeSites().size() != 1 && !(placeRow.getFreeSites().begin()->first == placeRow.getStartX() && placeRow.getFreeSites().begin()->second == placeRow.getNumSites() * placeRow.getSiteWidth()))
            {
                cout << "!! PlaceRow " << placeRow.getStartY() << " has abnormal free sites." << endl;
                cout << "rowidx: " << placeRow.getRowIdx() << endl;
                for (auto &site : placeRow.getFreeSites())
                {
                    cout << "x: " << site.first << " width: " << site.second << endl;
                }
            }
        }
    }

    // set the row idx
    /*for (int i = 0; i < placeRows.size(); i++)
    {
        placeRows[i].setRowIdx(i);
    }*/

    // set the lowest placerow y
    // placeRows[0].setLowestPlaceRowY(placeRows[0].getStartY());
    PlaceRow::lowest_placerowY = placeRows[0].getStartY();

    // Put the ff into the placeRows
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

            cout << endl;
            cout << "Free sites before inserting " << ff->getName() << endl;
            cout << "ff x: " << x << " FF y: " << y << endl;
            cout << "ff width " << ff->getWidth() << " ff height " << ff->getHeight() << endl;
            for (auto &site : placeRows[rowIdx].getFreeSites())
            {
                cout << "x: " << site.first << " width: " << site.second << endl;
            }
        }

        addInst_PlaceRows(ff);
        // ff_posOnPLaceRow[ff->getName()] = make_pair(rowIdx, siteIdx);

        if (DEBUG_PARSE)
        {
            cout << endl
                 << "Free sites after inserting " << ff->getName() << endl;
            for (auto &site : placeRows[rowIdx].getFreeSites())
            {
                cout << "x: " << site.first << " width: " << site.second << endl;
            }
            cout << endl;
        }
    }

    // put the gate into the placeRows
    for (auto &it : gate_dict)
    {
        Inst *gate = it.second;
        double x = gate->getX();
        double y = gate->getY();

        // calculate the row index and site index with the x and y coordinate
        int rowIdx = (y - placeRows[0].getStartY()) / placeRows[0].getSiteHeight();
        int siteIdx = (x - placeRows[0].getStartX()) / placeRows[0].getSiteWidth();

        // insert the gate into the placeRows
        if (DEBUG_PARSE)
        {
            cout << "Parsing: Adding " << gate->getName() << " to placerows" << endl;

            cout << endl;
            cout << "Free sites before inserting " << gate->getName() << endl;
            cout << "gate x: " << x << " gate y: " << y << endl;
            cout << "gate width " << gate->getWidth() << " gate height " << gate->getHeight() << endl;
            for (auto &site : placeRows[rowIdx].getFreeSites())
            {
                cout << "x: " << site.first << " width: " << site.second << endl;
            }
        }

        addInst_PlaceRows(gate);
        // gate_posOnPLaceRow[gate->getName()] = make_pair(rowIdx, siteIdx);

        if (DEBUG_PARSE)
        {
            cout << endl
                 << "Free sites after inserting " << gate->getName() << endl;
            for (auto &site : placeRows[rowIdx].getFreeSites())
            {
                cout << "x: " << site.first << " width: " << site.second << endl;
            }
            cout << endl;
        }
    }

    if (DEBUG_PARSE)
    {
        cout << " ****** PlaceRows ******* " << endl;
        for (auto &placeRow : placeRows)
        {
            cout << "row Idx: " << placeRow.getRowIdx() << endl;
            cout << "y: " << placeRow.getStartY() << endl;
            for (auto &site : placeRow.getFreeSites())
            {
                cout << "x: " << site.first << " width: " << site.second << endl;
            }
        }
        cout << " ************************ " << endl;
    }

    // if (PLOT_STEP)
    // {
    //     ofstream plot_file("../draw/text/parse.txt");
    //     writePlotFile(plot_file, {});
    //     plot_file.close();
    // }

    if (G_DEBUG)
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

void Solver::writePlotFile(ofstream &plot_file, vector<string> moved_ff)
{
    plot_file << "DieSize " << DieLLX << " " << DieLLY << " " << DieURX << " " << DieURY << endl;
    for (auto &it : ff_dict)
    {
        Inst *ff = it.second;
        plot_file << *ff << endl;
    }
    for (auto &it : gate_dict)
    {
        Inst *gate = it.second;
        plot_file << *gate << endl;
    }

    // // write the placement rows
    // for (auto &placeRow : placeRows)
    // {
    //     plot_file << placeRow << endl;
    // }

    // plot_file << "moved ffs ";
    // // write the moved ffs
    // for (auto &ff_name : moved_ff)
    // {
    //     plot_file << ff_name << " ";
    // }
}

void Solver::writePlaceRow(ofstream &plot_file)
{
    plot_file << "DieSize " << DieLLX << " " << DieLLY << " " << DieURX << " " << DieURY << endl;
    for (auto &placeRow : placeRows)
    {
        plot_file << placeRow << endl;
    }
}