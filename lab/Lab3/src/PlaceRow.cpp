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
#include <set>
#include <queue>
#include "Inst.h"
#include "PlaceRow.h"
using namespace std;

double PlaceRow::startX = 0.0;
int PlaceRow::siteWidth = 0.0;
int PlaceRow::siteHeight = 0.0;
int PlaceRow::numSites = 0;
double PlaceRow::lowest_placerowY = numeric_limits<double>::max();

// search for the site that can accommodate the ff
// if found, return the LL corner of the site which is on this row
// else return(-1, -1)
pair<double, double> PlaceRow::searchFFLL(Inst *ff, vector<PlaceRow> &placeRows)
{
    if (free_sites.empty())
    {
        if (DEBUG_SEARCH)
        {
            cout << "searchFFLL: No free site on row " << this->getRowIdx() << endl;
        }

        return make_pair(-1, -1);
    }

    int LL_rowIdx = (int)((this->startY - PlaceRow::lowest_placerowY) / PlaceRow::siteHeight);

    // the number of rows the ff occupies
    int occRowCount = ceil(ff->getHeight() / PlaceRow::siteHeight);
    // the row index of the highest row the ff occupies
    int top_rowIdx = LL_rowIdx + occRowCount - 1;

    if (top_rowIdx >= placeRows.size())
    {
        if (DEBUG_SEARCH)
        {
            cout << "searchFFLL: The ff " << ff->getName() << " is out of the layout on row " << this->getRowIdx() << endl;
        }

        return make_pair(-1, -1);
    }

    // starting from the site where the ff is placed
    // search for the site in the occRowCount-1 rows above the LL_rowIdx that have overlaped interval
    // with width >= ff->getWidth()
    // if the current site can't accommodate the ff, move to the next site, the order if leftward, rightward alternately,
    // until the end of both direction on the current row are reached

    // search the site on the current row
    auto it = free_sites.lower_bound(ff->getX());
    // filter out the sites that cannot accommodate the ff
    if (it == free_sites.end())
    {
        it = prev(it);
        // if(it->first + it->second < ff->getX() + ff->getWidth())
        // {
        //     return make_pair(-1, -1);
        // }
    }
    else if (it->first > ff->getX())
    {
        // if(it == free_sites.begin())
        // {
        //     return make_pair(-1, -1);
        // }

        // it = (it == free_sites.begin()) ? it : prev(it);
        if (it != free_sites.begin())
        {
            double curr_disp = abs(it->first - ff->getX());
            double prev_disp = abs((prev(it)->first + prev(it)->second) - (ff->getX() + ff->getWidth()));
            it = (curr_disp < prev_disp) ? it : prev(it);
        }

        // if(it->first + it->second < ff->getX() + ff->getWidth())
        // {
        //     return make_pair(-1, -1);
        // }
    }
    // else // it->first == ff->getX()
    // {
    //     if(it->second < ff->getWidth())
    //     {
    //         return make_pair(-1, -1);
    //     }
    // }

    if (DEBUG_BRUTEFINDINSERT)
    {
        cout << "The initial site iterator at row " << LL_rowIdx << " is: " << it->first << " " << it->second << endl;
    }

    auto it_left = it;
    auto it_right = it;
    // whether to move leftward or rightward after this iteration completes
    bool go_left = rand() % 2;

    // search the site on the rows above the current row
    // the row is iterated using it_left and it_right

    unordered_map<int, bool> visited_sites;

    while (it_left != free_sites.end() || it_right != free_sites.end())
    {
        // the x interval of the free site that is being checked
        auto it_curr = go_left ? it_left : it_right;
        visited_sites[it_curr->first] = true;

        Interval init_interval(it_curr->first, it_curr->second, LL_rowIdx);
        queue<Interval> q;
        q.push(init_interval);
        // Stores the intervals whose topIdx == top_rowIdx and
        vector<Interval> success_Intervals;

        while (!q.empty())
        {
            // whoever gets poped from quene has width >= ff->getWidth()
            Interval curr_interval = q.front();
            q.pop();

            if (DEBUG_BRUTEFINDINSERT || DEBUG_SEARCH)
            {
                cout << "curr_interval: " << "x left " << curr_interval.x_left << " width " << curr_interval.width << " x right " << curr_interval.x_left + curr_interval.width << " topIdx " << curr_interval.topIdx << endl;
            }

            // if the interval's width isn't enough to accommodate the ff, skip it
            if (curr_interval.topIdx == top_rowIdx)
            {
                if (curr_interval.width >= ff->getWidth())
                {
                    success_Intervals.push_back(curr_interval);
                    if (DEBUG_SEARCH)
                    {
                        cout << "success interval: " << "x left " << curr_interval.x_left << " width " << curr_interval.width << " x right " << curr_interval.x_left + curr_interval.width << " topIdx " << curr_interval.topIdx << endl;
                    }
                }
                continue;
            }

            // find a seq of intervals that overlap with the current interval
            double xl = curr_interval.x_left;
            double xr = curr_interval.x_left + curr_interval.width;
            PlaceRow &above_row = placeRows[curr_interval.topIdx + 1];

            // find the 1st interval that overlaps with the current interval
            auto left_bound = above_row.free_sites.lower_bound(xl);

            // left_bound->first > xl
            if (left_bound->first > xl)
            {
                // if left_bound is the first site and it doesn't overlap with the current interval
                if (left_bound == above_row.free_sites.begin() && left_bound->first >= xr)
                {
                    continue;
                }

                auto prev_left_bound = prev(left_bound);
                // if prev_left_bound overlap with the current interval
                if (prev_left_bound->first + prev_left_bound->second > xl)
                {
                    left_bound = prev_left_bound;
                }
                // if left_bound doesn't overlap with the current interval
                else if (left_bound->first > xr)
                {
                    continue;
                }
            }

            // find the last interval that overlaps with the current interval
            // right_bound->first >= xr
            auto right_bound = above_row.free_sites.lower_bound(xr);
            // if right_bound doesn't overlap with the current interval and it is the first site
            if (right_bound == above_row.free_sites.begin())
            {
                continue;
            }
            else
            {
                // right_bound->first <= xr
                right_bound = prev(right_bound);
                // if right_bound doesn't overlap with the current interval
                if (right_bound->first + right_bound->second < xl)
                {
                    continue;
                }
            }

            // find the intervals with width >= ff->getWidth() within the interval [left_bound, right_bound]
            for (auto it = left_bound; it != next(right_bound); it++)
            {
                // if the interval's width is enough to accommodate the ff
                if (it->second >= ff->getWidth())
                {
                    double x_left = max(xl, it->first);
                    double x_right = min(xr, it->first + it->second);
                    Interval new_interval(x_left, x_right - x_left, curr_interval.topIdx + 1);
                    q.push(new_interval);
                }
            }
        }

        // from all success intervals, find the one that has the smallest displacement from the ff's x
        if (!success_Intervals.empty())
        {
            double min_displacement = numeric_limits<double>::max();
            Interval best_interval;

            double final_x;
            for (auto &interval : success_Intervals)
            {
                double closest_x;
                // find the closest x to the ff's x
                if (ff->getX() < interval.x_left)
                {
                    closest_x = interval.x_left;
                }
                else if (ff->getX() + ff->getWidth() > interval.x_left + interval.width)
                {
                    closest_x = interval.x_left + interval.width - ff->getWidth();
                }
                else
                {
                    closest_x = ff->getX();
                }

                if (DEBUG_SEARCH)
                {
                    cout << "interval: x left " << interval.x_left << " width " << interval.width << " x right " << interval.x_left + interval.width << " " << interval.topIdx << " closest_x: " << closest_x << endl;
                }

                double displacement = abs(closest_x - ff->getX()) + abs(this->startY - ff->getY());
                if (displacement < min_displacement)
                {
                    min_displacement = displacement;
                    best_interval = interval;
                    final_x = closest_x;
                }
            }

            if (DEBUG_BRUTEFINDINSERT)
            {
                cout << "Best interval found: " << best_interval.x_left << " " << best_interval.width << " " << best_interval.topIdx << endl;
                cout << "Final x: " << final_x << endl;
            }

            return make_pair(final_x, this->startY);
        }

        // update the iterators
        if (go_left)
        {
            it_left = (it_left == free_sites.begin()) ? free_sites.end() : prev(it_left);
            go_left = (it_right == free_sites.end()) ? true : false;
        }
        else
        {
            it_right = next(it_right);
            go_left = (it_left == free_sites.end()) ? false : true;
        }
    }

    if (DEBUG_BRUTEFINDINSERT || DEBUG_SEARCH)
    {
        cout << "searchFFLL: Can't find a site that can accommodate the ff." << endl;
    }

    if (visited_sites.size() != this->free_sites.size())
    {
        cout << "searchFFLL: rowIdx: " << this->getRowIdx() << endl;
        cout << "searchFFLL: Terminated before all sites are visited." << endl;
        cout << "searchFFLL: Visited row number: " << visited_sites.size() << endl;
        cout << "searchFFLL: Total row number: " << this->free_sites.size() << endl;
        // for (auto &site : visited_sites)
        // {
        //     cerr << site.first << " ";
        // }
        // exit(1);
    }

    // delete the hash table
    visited_sites.clear();

    return make_pair(-1, -1);
}

// O(logn)
// assuming the ff can be placed on the row without overlapping,
// so a empty site with x<=ff->getX() and x+width>=ff->getX()+ff->getWidth() should be found
void PlaceRow::insertInst(Inst *inst, int siteIdx)
{
    // update ff_xPos2Inst
    if (inst->getFixed() == false)
    {
        ff_xPos2Inst[inst->getX()] = inst;
    }
    else
    {
        gate_xPos2Inst[inst->getX()] = inst;
    }

    // find the site that the ff will be placed
    // return the first site whose x is not less than ff->getX()
    // auto it = free_sites.lower_bound(EmptySite(ff->getX(), numeric_limits<double>::max()));
    auto it = free_sites.lower_bound(inst->getX());

    // insert the ff into the site
    // case 1: the site x is equal to ff->getX()
    if (it->first == inst->getX())
    {
        // if (DEBUG_PLACEROW)
        // {
        //     cout << "PlaceRow::insertFF: Found site x == ff->getX()." << endl;
        // }

        // case 1.1: the site width is equal to ff->getWidth()
        if (it->second == inst->getWidth())
        {
            if (DEBUG_PLACEROW)
            {
                cout << "case 1.1: the site width is equal to ff->getWidth()" << endl;
            }

            free_sites.erase(it);
        }
        // case 1.2: the site width is greater than ff->getWidth()
        else
        {
            if (DEBUG_PLACEROW)
            {
                cout << "case 1.2: the site width is greater than ff->getWidth()" << endl;
            }

            // EmptySite newSite(ff->getX() + ff->getWidth(), it->width - ff->getWidth());
            free_sites[inst->getX() + inst->getWidth()] = it->second - inst->getWidth();
            free_sites.erase(it);
        }
    }
    // case2: the site x is larger than inst->getX()
    else if (it->first > inst->getX())
    {
        // if (DEBUG_PLACEROW)
        // {
        //     cout << "PlaceRow::insertinst: case 2: Found site x > inst->getX()." << endl;
        // }

        // find the site that has x < inst->getX(), and ensue that the site width >= inst->getWidth()
        it = it == free_sites.begin() ? it : prev(it);
        if (it->second < inst->getWidth())
        {
            // Print the free sites
            cout << "Free sites:" << endl;
            for (auto &site : free_sites)
            {
                cout << "x: " << site.first << " width: " << site.second << endl;
            }
            // print out the inst coordinates
            cout << "inst x: " << inst->getX() << " inst width: " << inst->getWidth() << endl;

            cerr << "(case2) PlaceRow::insertinst: Found site's width is less than inst " << inst->getName() << "'s width." << endl;
            cerr << "ff x: " << inst->getX() << " ff width: " << inst->getWidth() << endl;
            cerr << "site x: " << it->first << " site width: " << it->second << endl;
            exit(1);
        }

        // cut out the site after the inst if it exists(inst.getX() + inst.getWidth() < site.x + site.width)
        if (it->first + it->second > inst->getX() + inst->getWidth())
        {
            free_sites[inst->getX() + inst->getWidth()] = it->first + it->second - inst->getX() - inst->getWidth();
        }

        // cut out the site before the inst(definitely exists cuz site x > inst->getX())
        it->second = inst->getX() - it->first;
    }
    // case 3: the site x is less than inst->getX()
    // in this case, all sites have x < inst->getX()
    // meaning the last site should be able to accommodate the inst
    else
    {
        if (DEBUG_PLACEROW)
        {
            cout << "PlaceRow::insertinst: case 3: Found site x < inst->getX()." << endl;
        }

        auto it_last = prev(free_sites.end());
        // the last free site can't cover the inst completely

        // if (inst->getName() == "FF_2_3")
        // {
        // cout << "insertinst: " << inst->getName() << endl;
        // cout << "inst x: " << inst->getX() << " inst width: " << inst->getWidth() << endl;
        // cout << "inst right x " << inst->getX() + inst->getWidth() << endl;
        // cout << "site x: " << it_last->first << " site width: " << it_last->second << endl;
        // cout << "site right x " << it_last->first + it_last->second << endl;
        // }

        if (it_last->first + it_last->second < inst->getX() + inst->getWidth())
        {
            cerr << "PlaceRow::insertinst: Found site can't cover inst " << inst->getName() << "." << endl;
            cerr << "site x: " << it_last->first << " site width: " << it_last->second << endl;
            cerr << "inst x " << inst->getX() << " inst width " << inst->getWidth() << endl;

            exit(1);
        }

        // cut out the site after the inst if it exists(inst.getX() + inst.getWidth() < site.x + site.width)
        if (it_last->first + it_last->second > inst->getX() + inst->getWidth())
        {
            free_sites[inst->getX() + inst->getWidth()] = it_last->first + it_last->second - inst->getX() - inst->getWidth();
        }

        // cut out the site before the inst
        it_last->second = inst->getX() - it_last->first;
    }
}

void PlaceRow::removeFF(const Inst *ff)
{

    if (DEBUG_removeFF)
    {
        // print out the free sites before removing the ff
        cout << "PlaceRow::removeFF: Before removing " << ff->getName() << endl;
        cout << "ff left x: " << ff->getX() << " ff width: " << ff->getWidth() << " ff right x: " << ff->getX() + ff->getWidth() << endl;
        for (auto &site : free_sites)
        {
            cout << "x left: " << site.first << " width: " << site.second << " x right: " << site.first + site.second << endl;
        }
        cout << endl;
    }

    // delete the ff from the ff_xPos2Inst
    ff_xPos2Inst.erase(ff->getX());

    // recover the site
    // insert the site and get the iterator to the inserted site
    free_sites[ff->getX()] = ff->getWidth();

    // find the site whose x <= ff->getX()
    auto it_inserted = free_sites.find(ff->getX());

    if (it_inserted == free_sites.end())
    {
        cerr << "PlaceRow::removeFF: Can't find the site that is inserted when removing ff " << ff->getName() << endl;
        // exit(1);
    }

    // free_sites.lower_bound(ff->getX());
    /*if (it_inserted->first > ff->getX())
    {
        // since it_inserted is the first site whose x > ff->getX(),
        // so the site before it_inserted is the site that we want to merge with the inserted site
        it_inserted = prev(it_inserted);
    }*/

    bool merge_before = false;
    bool merge_after = false;

    // find the site that is before the inserted site
    auto it_before = it_inserted == free_sites.begin() ? it_inserted : prev(it_inserted);
    if (it_before != it_inserted)
    {
        // merge the site before the inserted site and the inserted site
        if (it_before->first + it_before->second == it_inserted->first)
        {
            if (DEBUG_removeFF)
            {
                cout << "merge before" << endl;
                cout << "it_before :";
                cout << "x_left: " << it_inserted->first << "x_right: " << it_inserted->first + it_inserted->second << endl;
                cout << "it_after :";
                cout << "x_left: " << it_before->first << "x_right: " << it_before->first + it_before->second << endl;
            }

            /*EmptySite newSite(it_before->x, it_before->width + it_inserted->width);
            free_sites.erase(it_before);
            free_sites.erase(it_inserted);
            free_sites.insert(newSite);*/

            it_before->second = it_before->second + it_inserted->second;
            free_sites.erase(it_inserted);
            it_inserted = it_before;
        }
    }

    // find the site that is after the inserted site
    auto it_after = next(it_inserted);
    if (it_after != free_sites.end())
    {
        // merge the inserted site and the site after the inserted site
        if (it_inserted->first + it_inserted->second == it_after->first)
        {
            if (DEBUG_removeFF)
            {
                cout << "merge after" << endl;
                cout << "it_inserted :" << endl;
                cout << "x_left: " << it_inserted->first << "x_right: " << it_inserted->first + it_inserted->second << endl;
                cout << "it_after :";
                cout << "x_left: " << it_after->first << "x_right: " << it_after->first + it_after->second << endl;
            }

            /*EmptySite newSite(it_inserted->x, it_inserted->width + it_after->width);
            free_sites.erase(it_inserted);
            free_sites.erase(it_after);
            free_sites.insert(newSite);*/
            it_inserted->second = it_inserted->second + it_after->second;
            free_sites.erase(it_after);
        }
    }

    if (DEBUG_removeFF)
    {
        // print out the free sites after removing the ff
        cout << "PlaceRow::removeFF: After removing " << ff->getName() << endl;
        cout << "ff left x: " << ff->getX() << " ff width: " << ff->getWidth() << " ff right x: " << ff->getX() + ff->getWidth() << endl;
        for (auto &site : free_sites)
        {
            cout << "x left: " << site.first << " width: " << site.second << " x right: " << site.first + site.second << endl;
        }
        cout << endl;
    }
}

ostream &operator<<(ostream &os, const PlaceRow &placeRow)
{
    os << "PlacementRows ";
    os << placeRow.startY << " " << PlaceRow::siteHeight;
    for (auto &site : placeRow.free_sites)
    {
        os << " " << site.first << "," << site.second;
        // if (site != *placeRow.free_sites.rbegin())
        // {
        //     os << endl;
        // }
    }
    return os;
}