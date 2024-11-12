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
                // FF instantiation(UNDONE)
            }
            else
            {
                // Cell instantiation(UNDONE)
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

            // PlacementRows instantiation(UNDONE)
        }
        else
        {
            cerr << "Solver::readlg: Unknown keyword: " << head << endl;
            exit(1);
        }
    }

    if (DEBUG_PARSE)
    {
        cout << "Parsing done." << endl;
    }
}

void Solver::writeOutput(ofstream &output_file)
{
    // write output(UNDONE)
}