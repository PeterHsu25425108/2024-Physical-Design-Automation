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
    int LL_rowIdx = Inst_placeRowIdx(ff);
    // the number of rows the ff occupies
    int occRowCount = ceil(ff->getHeight() / placeRows[0].getSiteHeight());
    // the row index of the highest row the ff occupies
    int top_rowIdx = LL_rowIdx + occRowCount - 1;

    if (DEBUG_BRUTEFINDINSERT)
    {
        cout << "row trait of ff " << ff->getName() << endl;
        cout << "ff height: " << ff->getHeight() << " site height: " << placeRows[0].getSiteHeight() << endl;
        cout << "LL_rowIdx: " << LL_rowIdx << " top_rowIdx: " << top_rowIdx << " occRowCount: " << occRowCount << endl;
    }

    bool found_space = false;
    int numRowVisited = 0;
    int up_rowIdx = LL_rowIdx;
    int down_rowIdx = LL_rowIdx;
    bool go_down;

    if (LL_rowIdx == 0)
    {
        go_down = true;
    }
    else if (LL_rowIdx == placeRows.size() - 1)
    {
        go_down = false;
    }
    else
    {
        go_down = rand() % 2;
    }

    unordered_map<int, int> visited_rows;

    pair<double, double> Void = make_pair(-1, -1);
    while (numRowVisited < placeRows.size())
    {
        int curr_rowIdx = go_down ? down_rowIdx : up_rowIdx;

        if (DEBUG_SEARCH)
        {
            cout << endl
                 << "------------ Solver::BruteFindInsertion: searching row " << curr_rowIdx << " --------------" << endl;
        }

        if (DEBUG_BRUTEFINDINSERT)
        {
            cout << "----------- searching insertion point for " << ff->getName() << " on row " << curr_rowIdx << " ---------" << endl;
        }

        // search the site on the current row
        PlaceRow &curr_row = placeRows[curr_rowIdx];
        pair<double, double> LL = curr_row.searchFFLL(ff, placeRows);

        if (DEBUG_SEARCH)
        {
            cout << "--------------------------------" << endl;
        }

        if (LL != Void)
        {

            if (DEBUG_BRUTEFINDINSERT)
            {
                // print out the LL and the width of the ff
                cout << "Solver::BruteFindInsertion: " << ff->getName() << " on row " << curr_rowIdx << endl;
                cout << "LL: " << LL.first << " " << LL.second << " width: " << ff->getWidth() << endl;
                cout << "Consecutive sites: " << endl;

                for (int i = LL_rowIdx; i <= min(top_rowIdx, (int)placeRows.size()); i++)
                {
                    // find the row that the ff is placed
                    PlaceRow &row = placeRows[i];
                    cout << "Row " << i << ": ";
                    bool found_site = false;
                    for (auto &site : row.getFreeSites())
                    {
                        if (site.first <= LL.first && (site.first + site.second >= LL.first + ff->getWidth()))
                        {
                            cout << "x: " << site.first << " width: " << site.second << " ";
                            found_site = true;
                        }
                    }

                    if (!found_site)
                    {
                        cout << endl
                             << " === the whole row === " << endl;
                        for (auto &site : row.getFreeSites())
                        {
                            cout << "x: " << site.first << " width: " << site.second << endl;

                            if (site.first <= LL.first)
                            {
                                cout << "fit" << endl;
                                cout << "site right x " << site.first + site.second << " ff right x " << LL.first + ff->getWidth() << endl;
                                if (site.first + site.second >= LL.first + ff->getWidth())
                                {
                                    cout << "width fit" << endl;
                                }
                            }
                        }
                        cout << " ================= " << endl;
                    }
                    cout << endl;
                }
            }

            found_space = true;
            ff->setX(LL.first);
            ff->setY(LL.second);

            if (ff->getName() == "FF_2_3")
            {
                cout << "FF_2_3: " << ff->getX() << " " << ff->getY() << endl;
            }

            addInst_PlaceRows(ff);
            // moved_ff.push_back(ff->getName());

            break;
        }

        if (found_space)
        {
            break;
        }

        // row updated when we can't find any free space that acomadate ff on current row
        if (go_down)
        {
            go_down = (down_rowIdx == 0) ? false : true;
            down_rowIdx = (down_rowIdx == 0) ? 0 : down_rowIdx - 1;
        }
        else
        {
            go_down = (up_rowIdx == placeRows.size() - 1) ? true : false;
            up_rowIdx = (up_rowIdx == placeRows.size() - 1) ? placeRows.size() - 1 : up_rowIdx + 1;
        }

        if (DEBUG_BRUTEFINDINSERT)
        {
            cout << "--------------------------------" << endl;
        }

        numRowVisited++;
        if (DEBUG_NOTFOUND)
        {
            visited_rows[curr_rowIdx] = numRowVisited;
        }

        if (DEBUG_SEARCH)
        {
            cout << "-------------------------------------------" << endl;
        }

        if (numRowVisited == placeRows.size())
        {
            break;
        }
    }

    // not found a space to place the ff
    if (!found_space)
    {

        if (visited_rows.size() < placeRows.size())
        {
            cout << "Solver::BruteFindInsertion: Terminated before all rows are visited" << endl;
            cout << "Solver::BruteFindInsertion: Visited rows: " << endl;
            cout << "total num of Placerows: " << placeRows.size() << endl;

            // sort the visited rows by the second value
            vector<pair<int, int>> visited_rows_vec(visited_rows.begin(), visited_rows.end());
            sort(visited_rows_vec.begin(), visited_rows_vec.end(), [](const pair<int, int> &a, const pair<int, int> &b)
                 { return a.second < b.second; });

            for (auto &row : visited_rows_vec)
            {
                cout << row.first << endl;
            }
        }
        else
        {
            cout << "Solver::BruteFindInsertion: Can't find a space to place " << ff->getName() << " in any row" << endl;
        }

        cerr << "Solver::BruteFindInsertion: Can't find a space to place " << ff->getName() << endl;

        if (PLOT_FINAL || PLOT_STEP)
        {
            string filename = "../draw/text/" + ff->getName() + "_remove.txt";
            ofstream plot_file(filename);
            writePlotFile(plot_file, {});
            plot_file.close();
        }

        exit(1);
    }

    return moved_ff;
}

void Solver::solve(ifstream &opt_file, ofstream &output_file)
{
    if (G_DEBUG)
    {
        cout << "Solver::solve: start solving..." << endl;
    }

    // plot the initial layout
    if (PLOT_STEP)
    {

        string filename = "../draw/text/before_solve.txt";
        ofstream plot_file(filename);
        writePlotFile(plot_file, {});
        plot_file.close();
    }

    if (G_DEBUG)
    {
        cout << "done" << endl;
    }

    string temp;
    while (getline(opt_file, temp))
    {
        stringstream ss(temp);
        // remove the "Banking_Cell: " part
        ss >> temp;

        if (DEBUG_OPTINPUT)
        {
            cout << "Parsing: " << temp << endl;
        }

        bool isBanked = true;
        vector<string> removed_ffs;

        // get the first banked ff name
        ss >> temp;
        while (temp != "-->")
        {
            if (DEBUG_OPTINPUT)
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

        if (DEBUG_OPTINPUT)
        {
            cout << "Parsing: " << new_ff_name << " " << LL_x << " " << LL_y << " " << width << " " << height << endl;
        }

        // insert the new ff into the layout, but it is not legalized yet
        Inst *new_ff = new Inst(new_ff_name, LL_x, LL_y, width, height, false);
        ff_dict[new_ff_name] = new_ff;
        // ff_onDie.push_back(new_ff_name);

        if (DEBUG_SOLVE)
        {
            cout << "Solver::solve: " << new_ff_name << " inserted into the layout." << endl;
        }

        // remove the old ffs from the layout
        for (auto &ff_name : removed_ffs)
        {
            // remove the ff from the placeRow
            removeFF_PlaceRows(ff_dict[ff_name]);

            // remove the ff from the ff_dict
            // ff_onDie.remove(ff_name);
            delete ff_dict[ff_name];
            ff_dict.erase(ff_name);

            if (DEBUG_SOLVE)
            {
                cout << "Solver::solve: " << ff_name << " removed from the layout." << endl;
            }
        }

        if (G_DEBUG)
        {
            cout << "legalizing " << new_ff_name << " ...";
        }

        if (PLOT_REMOVE)
        {
            string filename = "../draw/text/" + new_ff_name + "_remove.txt";
            ofstream plot_file(filename);
            writePlotFile(plot_file, {});
            plot_file.close();
        }

        // legalizing the new ff, this is where the legalization method is called
        // the position of the new ff or any other cells displaced and the placerows will be updated
        vector<string> moved_ff = BruteFindInsertion(new_ff);

        if (G_DEBUG)
        {
            cout << " done." << endl;
        }

        if (DEBUG_SOLVE)
        {
            cout << "Solver::solve: " << new_ff_name << " legalized." << endl;
        }

        // write the result to the output file
        writeOutput(output_file, new_ff, moved_ff);

        if (PLOT_STEP)
        {
            // plot the layout after each step
            string filename = "../draw/text/" + new_ff_name + ".txt";
            ofstream plot_file(filename);
            writePlotFile(plot_file, moved_ff);
            plot_file.close();
        }

        if (DEBUG_SOLVE)
        {
            cout << "Solver::solve: " << new_ff_name << " result written to the output file." << endl;
        }
    }
}