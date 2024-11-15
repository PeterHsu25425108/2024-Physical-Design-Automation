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
#include "Inst.h"
#include "PlaceRow.h"
using namespace std;

double PlaceRow::startX = 0.0;
int PlaceRow::siteWidth = 0.0;
int PlaceRow::siteHeight = 0.0;
int PlaceRow::numSites = 0;
double PlaceRow::lowest_placerowY = numeric_limits<double>::max();

// O(logn)
// assuming the ff can be placed on the row without overlapping,
// so a empty site with x<=ff->getX() and x+width>=ff->getX()+ff->getWidth() should be found
void PlaceRow::insertFF(Inst *ff, int siteIdx)
{
    // find the site that the ff will be placed
    // return the first site whose x is not less than ff->getX()
    // auto it = free_sites.lower_bound(EmptySite(ff->getX(), numeric_limits<double>::max()));
    auto it = free_sites.lower_bound(ff->getX());

    if (it == free_sites.end())
    {
        cerr << "PlaceRow::insertFF: Cannot find a site for the ff cuz return end." << endl;
        exit(1);
    }
    else if (it->first > ff->getX())
    {
        cerr << "PlaceRow::insertFF: Found site x is greater than ff->getX()." << endl;
        exit(1);
    }
    else if (it->first + it->second < ff->getX() + ff->getWidth())
    {
        cerr << "PlaceRow::insertFF: Found site's width is less than ff " << ff->getName() << "'s width." << endl;
        exit(1);
    }

    // insert the ff into the site
    // case 1: the site x is equal to ff->getX()
    if (it->first == ff->getX())
    {
        // case 1.1: the site width is equal to ff->getWidth()
        if (it->second == ff->getWidth())
        {
            free_sites.erase(it);
        }
        // case 1.2: the site width is greater than ff->getWidth()
        else
        {
            // EmptySite newSite(ff->getX() + ff->getWidth(), it->width - ff->getWidth());
            free_sites.erase(it);
            free_sites[ff->getX() + ff->getWidth()] = it->second - ff->getWidth();
        }
    }
    // case2: the site x is larger than ff->getX()
    else
    {
        // find the site that has x < ff->getX(), and ensue that the site width >= ff->getWidth()
        it = it == free_sites.begin() ? it : prev(it);
        if (it->second < ff->getWidth())
        {
            cerr << "PlaceRow::insertFF: Found site's width is less than ff " << ff->getName() << "'s width." << endl;
            exit(1);
        }

        // cut out the site before the ff(definitely exists cuz site x > ff->getX())
        it->second -= ff->getX() - it->first;
        // cut out the site after the ff if it exists(ff.getX() + ff.getWidth() < site.x + site.width)
        if (it->first + it->second > ff->getX() + ff->getWidth())
        {
            free_sites[ff->getX() + ff->getWidth()] = it->first + it->second - ff->getX() - ff->getWidth();
        }
    }
}

void PlaceRow::removeFF(const Inst *ff)
{
    // recover the site
    // insert the site and get the iterator to the inserted site
    free_sites[ff->getX()] = ff->getWidth();
    auto it_inserted = free_sites.find(ff->getX());

    // find the site that is before the inserted site
    auto it_before = it_inserted == free_sites.begin() ? it_inserted : prev(it_inserted);
    if (it_before != free_sites.begin())
    {
        // merge the site before the inserted site and the inserted site
        if (it_before->first + it_before->second == it_inserted->first)
        {
            /*EmptySite newSite(it_before->x, it_before->width + it_inserted->width);
            free_sites.erase(it_before);
            free_sites.erase(it_inserted);
            free_sites.insert(newSite);*/

            it_before->second = it_before->second + it_inserted->second;
        }
    }

    // find the site that is after the inserted site
    auto it_after = next(it_inserted);
    if (it_after != free_sites.end())
    {
        // merge the inserted site and the site after the inserted site
        if (it_inserted->first + it_inserted->second == it_after->first)
        {
            /*EmptySite newSite(it_inserted->x, it_inserted->width + it_after->width);
            free_sites.erase(it_inserted);
            free_sites.erase(it_after);
            free_sites.insert(newSite);*/
            it_inserted->second = it_inserted->second + it_after->second;
        }
    }
}