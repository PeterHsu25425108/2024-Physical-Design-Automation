#include"DEBUG.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include"struct.h"
#include"Router.h"
#include <bits/stdc++.h>
#include<algorithm>
using namespace std;

// solve the routing problem
// each net are printed after being routed
void Router::solve(ofstream& out_file)
{
    // to determine the net ordering, calculate the area of the bounding box
    // and sort the net by it in ascending order
    
    // collect all net_id and their bounding box area
    vector<pair<int, int>> net_id_area;
    for(auto& net : nets)
    {
        int min_x = min(chip1.getBump(net.net_id).x, chip2.getBump(net.net_id).x);
        int min_y = min(chip1.getBump(net.net_id).y, chip2.getBump(net.net_id).y);
        int max_x = max(chip1.getBump(net.net_id).x, chip2.getBump(net.net_id).x);
        int max_y = max(chip1.getBump(net.net_id).y, chip2.getBump(net.net_id).y);

        int area = (max_x - min_x) * (max_y - min_y);
        net_id_area.push_back({net.net_id, area});
    }
    
    // sort the net by the area in ascending order
    sort(net_id_area.begin(), net_id_area.end(), [](const pair<int, int>& a, const pair<int, int>& b) { return a.second < b.second; });

    // for each net, route it
    for(auto& p : net_id_area)
    {
        // route the net
        A_star(p.first);

        // write the output
        writeOutput(out_file, nets[p.first]);
    }
}