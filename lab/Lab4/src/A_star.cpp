#include"DEBUG.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include"struct.h"
#include"Router.h"
#include<set>
#include<unordered_map>
#include <bits/stdc++.h>
using namespace std;

// M1: vertical
// M2: horizontal
// the starting and ending wire seg must be metal 1

// For testing the function, a set is used instead of a priority queue cuz the stl pq does not support the find operation
// Need to implement a custom pq to support the find operation to speed up the search

enum Direction
{
    VERTICAL, HORIZONTAL
};

struct ASTarNode
{
    pair<int, int> idx;
    double f_cost;
    bool operator<(const ASTarNode& rhs) const
    {
        return f_cost < rhs.f_cost;
    }
    
    bool operator==(const ASTarNode& rhs) const
    {
        return idx == rhs.idx;
    }
};

void Router::backTrack(const vector<vector<pair<int,int>>>& parent, pair<int, int> src_idx, pair<int,int> tar_idx, vector<WireSeg>& wire_segs)
{
    if(DEBUG_BACKTRACK)
    {
        cout << "Backtrack from src_idx: (" << src_idx.first << ", " << src_idx.second << ") to tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
    }

    pair<int, int> cur_idx = tar_idx;
    pair<int, int> end_idx = tar_idx;
    Direction prev_dir = (parent[cur_idx.first][cur_idx.second].first == cur_idx.first) ? HORIZONTAL : VERTICAL;

    while(cur_idx != make_pair(-1,-1))
    {
        pair<int, int> parent_idx = parent[cur_idx.first][cur_idx.second];
        if(cur_idx != src_idx && parent_idx == make_pair(-1,-1))
        {
            cerr << "!!: backTrack failed!" << endl;
            cerr << "src_idx: (" << src_idx.first << ", " << src_idx.second << ") tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
            cerr << "cur_idx: (" << cur_idx.first << ", " << cur_idx.second << ")" << endl;
            exit(1);
        }

        Direction parent2cur_dir = (parent_idx !=make_pair(-1,-1) && (parent_idx.first == cur_idx.first)) ? HORIZONTAL : VERTICAL;

        // if the wire seg changes direction or the parent is the start of the wire seg
        // then add a wire seg and update the verNetCount and horNetCount of the grid on the wire seg
        // need to exclude the case when a via is added onto the src or tar grid
        if(parent2cur_dir != prev_dir || parent_idx == make_pair(-1,-1))
        {
            if(DEBUG_BACKTRACK)
            {
                cout << "find wire seg from (" << cur_idx.first << ", " << cur_idx.second << ") to (" << end_idx.first << ", " << end_idx.second << ")" << endl;
            }

            // add a wire seg
            pair<int, int> cur_coor = Idx2LLcoor(cur_idx.first, cur_idx.second);
            pair<int, int> end_coor = Idx2LLcoor(end_idx.first, end_idx.second);
            MetalLayer ml = (prev_dir == HORIZONTAL) ? M2 : M1;
            wire_segs.push_back({cur_coor.first, cur_coor.second, end_coor.first, end_coor.second, ml});

            // update the grid's verNetCount or horNetCount
            if(prev_dir == HORIZONTAL)
            {
                // update the horNetCount
                for(int i = min(cur_idx.second, end_idx.second)+1; i <= max(cur_idx.second, end_idx.second); i++)
                {
                    grid[cur_idx.first][i].horNetCount++;
                }
            }
            else
            {
                // update the verNetCount
                for(int j = min(cur_idx.first, end_idx.first)+1; j <= max(end_idx.first, cur_idx.first); j++)
                {
                    grid[j][cur_idx.second].verNetCount++;
                }
            }

            prev_dir = parent2cur_dir;
            end_idx = cur_idx;
        }

        cur_idx = parent_idx;
    }
}

double Router::h_est(pair<int, int> src_idx, pair<int, int> tar_idx)
{
    // manhattan distance
    return abs(src_idx.first - tar_idx.first) + abs(src_idx.second - tar_idx.second);
}

double Router::exact_cost(pair<int, int> src_idx, pair<int, int> tar_idx, const vector<vector<pair<int,int>>>& parent)
{
    // check if src_idx and tar_idx are adjacent
    if(abs(src_idx.first - tar_idx.first) + abs(src_idx.second - tar_idx.second) != 1)
    {
        cerr << "!!: exact_cost called on non-adjacent nodes!" << endl;
        cerr << "src_idx: (" << src_idx.first << ", " << src_idx.second << ") tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
        cerr << "src position: (" << Idx2LLcoor(src_idx.first, src_idx.second).first << ", " << Idx2LLcoor(src_idx.first, src_idx.second).second << ") tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
        cerr << "tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
        cerr << "tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
        exit(1);
    }

    // account for WL cost
    double val = Alpha;

    // via cost and cell cost
    // check if wire change direction when src -> tar
    Direction src2tar_dir = (src_idx.first == tar_idx.first) ? HORIZONTAL : VERTICAL;
    // if the src_idx is the start of a wire seg, meaning parent[src_idx] is -1
    MetalLayer layer;
    if(parent[src_idx.first][src_idx.second] == make_pair(-1,-1))
    {
        // add via cost if the src->tar direction is horizontal
        // meaning the wire seg is on M2
        if(src2tar_dir == HORIZONTAL)
        {
            val += Delta*ViaCost + Gamma*(grid[src_idx.first][src_idx.second].M2_cost + grid[src_idx.first][src_idx.second].M1_cost + grid[tar_idx.first][tar_idx.second].M2_cost);
        }
        else
        {
            val += Gamma*(grid[src_idx.first][src_idx.second].M1_cost + grid[tar_idx.first][tar_idx.second].M1_cost);
        }
    }
    else
    {
        Direction parent2src_dir = (parent[src_idx.first][src_idx.second].first == src_idx.first) ? HORIZONTAL : VERTICAL;
        
        // if the wire seg changes direction
        if(src2tar_dir != parent2src_dir)
        {
            // account for via cost and the cost for the 2 layers on the src grid
            val += Delta*ViaCost + Gamma*(grid[src_idx.first][src_idx.second].M2_cost + grid[src_idx.first][src_idx.second].M1_cost);
            // cost on the tar grid, which depends on the direction of the wire seg
            if(src2tar_dir == HORIZONTAL)
            {
                val += Gamma*grid[tar_idx.first][tar_idx.second].M2_cost;
            }
            else
            {
                val += Gamma*grid[tar_idx.first][tar_idx.second].M1_cost;
            }
        }   
        else
        {
            // no via cost
            // only account for the cost on the tar grid
            if(src2tar_dir == HORIZONTAL)
            {
                val += Gamma*(grid[tar_idx.first][tar_idx.second].M2_cost + grid[src_idx.first][src_idx.second].M2_cost);
            }
            else
            {
                val += Gamma*(grid[tar_idx.first][tar_idx.second].M1_cost + grid[src_idx.first][src_idx.second].M1_cost);
            }
        }
    }

    bool ov;
    // overflow cost
    if(src2tar_dir == HORIZONTAL)
    {
        // left neighbor ov
        if(tar_idx.second == src_idx.second-1)
        {
            // check the capacity of tar grid
            ov = grid[src_idx.first][src_idx.second].leftHorCap <= grid[src_idx.first][src_idx.second].horNetCount;
            //val += ov * Beta;
        }
        // right neighbor ov
        else if(tar_idx.second == src_idx.second+1)
        {
            ov = grid[tar_idx.first][tar_idx.second].leftHorCap <= grid[tar_idx.first][tar_idx.second].horNetCount;
            //val += ov * Beta;
        }
        else
        {
            cerr << "!!: exact_cost called on non-adjacent nodes(HORIZONTAL)!" << endl;
            cerr <<"src_idx: (" << src_idx.first << ", " << src_idx.second << ") tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
            cerr << "src position: (" << Idx2LLcoor(src_idx.first, src_idx.second).first << ", " << Idx2LLcoor(src_idx.first, src_idx.second).second << ") tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
            cerr << "tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
            cerr << "tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
            exit(1);
        }

        val += ov * Beta * 0.5 * MetalCost_max;
    }
    else
    {
        // up neighbor ov
        if(tar_idx.first == src_idx.first+1)
        {
            ov = grid[tar_idx.first][tar_idx.second].bottomVerCap <= grid[tar_idx.first][tar_idx.second].verNetCount;
            //val += ov * Beta;
        }
        // down neighbor ov
        else if(tar_idx.first == src_idx.first-1)
        {
            ov = grid[src_idx.first][src_idx.second].bottomVerCap <= grid[src_idx.first][src_idx.second].verNetCount;
            //val += ov * Beta;
        }
        else
        {
            cerr << "!!: exact_cost called on non-adjacent nodes(VERTICAL)!" << endl;
            cerr <<"src_idx: (" << src_idx.first << ", " << src_idx.second << ") tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
            cerr << "src position: (" << Idx2LLcoor(src_idx.first, src_idx.second).first << ", " << Idx2LLcoor(src_idx.first, src_idx.second).second << ") tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
            cerr << "tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
            cerr << "tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
            exit(1);
        }

        val += ov * Beta * 0.5 * M1Cost_max;
    }

    return val;
}

void Router::A_star(int net_id)
{
    vector<vector<pair<int,int>>> parent(GRID_DIM_VER, vector<pair<int,int>>(GRID_DIM_HOR, {-1,-1}));
    vector<vector<double>> g_cost(GRID_DIM_VER, vector<double>(GRID_DIM_HOR, numeric_limits<double>::max()));
    vector<vector<double>> f_cost(GRID_DIM_VER, vector<double>(GRID_DIM_HOR, numeric_limits<double>::max()));
    /*priority_queue*/set<ASTarNode> pq;
    //unordered_map<int, set<ASTarNode>::iterator> pq_map;

    // starting from chip1, end at chip2
    pair<int, int> src_idx = LLcoor2Idx(chip1.getBump(net_id).x, chip1.getBump(net_id).y);
    pair<int, int> tar_idx = LLcoor2Idx(chip2.getBump(net_id).x, chip2.getBump(net_id).y);

    ofstream prop_file;
    if(PROPAGATE_PLOT)
    {
        prop_file.open("../draw/text/propagate_" + to_string(net_id) + ".txt");
        prop_file << "GMP_LLX " << GMP_LLX << " GMP_LLY " << GMP_LLY << " GRID_W " << GRID_W << " GRID_H " << GRID_H << " GMP_WIDTH " << GMP_WIDTH << " GMP_HEIGHT " << GMP_HEIGHT << endl;
        prop_file << "src idx " << src_idx.first << " " << src_idx.second << endl;
        prop_file << "tar idx " << tar_idx.first << " " << tar_idx.second << endl;
    }

    if(DEBUG_ASTAR)
    {
        cout << "src position: (" << chip1.getBump(net_id).x << ", " << chip1.getBump(net_id).y << ") tar position: (" << chip2.getBump(net_id).x << ", " << chip2.getBump(net_id).y << ")" << endl;
        cout << "tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
    }

    // check if the src and tar position are out of the routing grid
    if(chip1.getBump(net_id).x < GMP_LLX || chip1.getBump(net_id).x >= GMP_LLX + GMP_WIDTH || chip1.getBump(net_id).y < GMP_LLY || chip1.getBump(net_id).y >= GMP_LLY + GMP_HEIGHT)
    {
        cerr << "!!: A star search for net "<< net_id <<"failed!"<<endl;
        cerr << "src position: (" << chip1.getBump(net_id).x << ", " << chip1.getBump(net_id).y << ")" << endl;
        cerr << "src_idx: (" << src_idx.first << ", " << src_idx.second << ")" << endl;
        exit(1);
    }

    if(chip2.getBump(net_id).x < GMP_LLX || chip2.getBump(net_id).x >= GMP_LLX + GMP_WIDTH || chip2.getBump(net_id).y < GMP_LLY || chip2.getBump(net_id).y >= GMP_LLY + GMP_HEIGHT)
    {
        cerr << "!!: A star search for net "<< net_id <<"failed!"<<endl;
        cerr << "tar position: (" << chip2.getBump(net_id).x << ", " << chip2.getBump(net_id).y << ")" << endl;
        cerr << "tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
        exit(1);
    }
    

    if(DEBUG_ASTAR)
    {
        cout <<endl << "A star search for net "<< net_id << endl;
        cout << "src_idx: (" << src_idx.first << ", " << src_idx.second << ") tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
    }

    // set the initial value of g score and f score for the starting node
    g_cost[src_idx.first][src_idx.second] = 0;
    f_cost[src_idx.first][src_idx.second] = h_est(src_idx, tar_idx);

    // push the starting node to the priority queue
    pq.insert({src_idx, f_cost[src_idx.first][src_idx.second]});
    int src_key = src_idx.first * GRID_DIM_VER + src_idx.second;
    //pq_map[src_key] = pq.begin();

    while(!pq.empty())
    {
        // get the node with lowest f cost in the pq
        pair<int, int> curNodeIdx = pq.begin()->idx;
        // delete the node from the pq
        pq.erase(pq.begin());

        if(PROPAGATE_PLOT)
        {
            prop_file << curNodeIdx.first << " " << curNodeIdx.second << endl;
        }

        if(DEBUG_ASTAR)
        {
            cout << "curNodeIdx: (" << curNodeIdx.first << ", " << curNodeIdx.second << ") f_cost: " << f_cost[curNodeIdx.first][curNodeIdx.second] << endl;
        }
        
        if(curNodeIdx == tar_idx)
        {
            if(DEBUG_ASTAR)
            {
                cout << "A star search for net "<< net_id <<" succeed!"<<endl;
            }

            // backtrack the path
            backTrack(parent, src_idx, tar_idx, nets[net_id].wire_segs);
            // calculate the cost of the net
            nets[net_id].cost = g_cost[tar_idx.first][tar_idx.second];

            if(PROPAGATE_PLOT)
            {
                prop_file.close();
            }

            return;
        }

        // search through each neighbors of the curNodeIdx
        // update their g score if theirs are higher than g_cost[curNodeIdx] + exact_cost(curNodeIdx, neighbor)
        vector<pair<int, int>> neighbors;

        // down
        if(curNodeIdx.first > 0)
        {
            neighbors.push_back({curNodeIdx.first-1, curNodeIdx.second});
        }
        // up 
        if(curNodeIdx.first < GRID_DIM_VER-1)
        {
            neighbors.push_back({curNodeIdx.first+1, curNodeIdx.second});
        }
        // left 
        if(curNodeIdx.second > 0)
        {
            neighbors.push_back({curNodeIdx.first, curNodeIdx.second-1});
        }
        // right
        if(curNodeIdx.second < GRID_DIM_HOR-1)
        {
            neighbors.push_back({curNodeIdx.first, curNodeIdx.second+1});
        }

        for(auto& neighbor : neighbors)
        {
            if(DEBUG_ASTAR)
            {
                cout <<"search neighbor: (" << neighbor.first << ", " << neighbor.second << ")"<<endl;
            }
                
            double new_g_cost = g_cost[curNodeIdx.first][curNodeIdx.second] + exact_cost(curNodeIdx, neighbor, parent);

            if(DEBUG_ASTAR)
            {
                cout << "----------------- " << endl;
                cout << "   old g_cost: " << g_cost[neighbor.first][neighbor.second] << endl;
                cout << "   exact_cost: " << exact_cost(curNodeIdx, neighbor, parent) << endl;
                cout << "   g_cost[curNodeIdx]: " << g_cost[curNodeIdx.first][curNodeIdx.second] << endl;
                cout << "   new_g_cost: " << new_g_cost << endl;
                cout << "-----------------" << endl;
            }

            if(new_g_cost < g_cost[neighbor.first][neighbor.second])
            {
                if(DEBUG_ASTAR)
                {
                    cout << "INSERT neighbor: (" << neighbor.first << ", " << neighbor.second << ")" << endl;
                    
                }

                auto it_set = pq.find({neighbor, f_cost[neighbor.first][neighbor.second]});

                parent[neighbor.first][neighbor.second] = curNodeIdx;
                g_cost[neighbor.first][neighbor.second] = new_g_cost;
                f_cost[neighbor.first][neighbor.second] = g_cost[neighbor.first][neighbor.second] + h_est(neighbor, tar_idx);
                
                // find the neighbor in the pq
                if(it_set != pq.end())
                {
                    pq.erase(it_set);
                }

                //int neighbor_key = neighbor.first * GRID_DIM_VER + neighbor.second;
                /*auto it = pq_map.find(neighbor_key);
                if(it!= pq_map.end())
                {
                    pq.erase(it->second);
                    pq_map.erase(it);
                }*/
                
                pq.insert({neighbor, f_cost[neighbor.first][neighbor.second]});
                //pq_map[neighbor_key] = pq.begin();
            }

            if(DEBUG_ASTAR)
            {
                cout << endl;
            }
        }
    }

    prop_file.close();

    cerr << "!!: A star search for net "<< net_id <<"failed!"<<endl;
    exit(1);
    
}