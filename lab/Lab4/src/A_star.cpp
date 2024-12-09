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
    pair<int, int> cur_idx = tar_idx;
    Direction cur_dir = VERTICAL;
    

    while(cur_idx != src_idx && parent[cur_idx.first][cur_idx.second] != make_pair(-1,-1))
    {
        pair<int, int> parent_idx = parent[cur_idx.first][cur_idx.second];
        Direction parent_dir = (parent_idx !=make_pair(-1,-1) && (parent_idx.first == cur_idx.first)) ? HORIZONTAL : VERTICAL;


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

    // overflow cost
    if(src2tar_dir == HORIZONTAL)
    {
        if(tar_idx.first == src_idx.first-1)
        {
            // check the capacity of tar grid
            bool ov = grid[tar_idx.first][tar_idx.second].leftHorCap <= grid[tar_idx.first][tar_idx.second].horNetCount;
            val += ov * Beta;
        }
        else if(tar_idx.first == src_idx.first+1)
        {
            bool ov = grid[src_idx.first][src_idx.second].leftHorCap <= grid[src_idx.first][src_idx.second].horNetCount;
            val += ov * Beta;
        }
        else
        {
            cerr << "!!: exact_cost called on non-adjacent nodes!" << endl;
            cerr <<"src_idx: (" << src_idx.first << ", " << src_idx.second << ") tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
            cerr << "src position: (" << Idx2LLcoor(src_idx.first, src_idx.second).first << ", " << Idx2LLcoor(src_idx.first, src_idx.second).second << ") tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
            cerr << "tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
            cerr << "tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
            exit(1);
        }
    }
    else
    {
        if(tar_idx.second == src_idx.second+1)
        {
            bool ov = grid[tar_idx.first][tar_idx.second].bottomVerCap <= grid[tar_idx.first][tar_idx.second].verNetCount;
            val += ov * Beta;
        }
        else if(tar_idx.second == src_idx.second-1)
        {
            bool ov = grid[src_idx.first][src_idx.second].bottomVerCap <= grid[src_idx.first][src_idx.second].verNetCount;
            val += ov * Beta;
        }
        else
        {
            cerr << "!!: exact_cost called on non-adjacent nodes!" << endl;
            cerr <<"src_idx: (" << src_idx.first << ", " << src_idx.second << ") tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
            cerr << "src position: (" << Idx2LLcoor(src_idx.first, src_idx.second).first << ", " << Idx2LLcoor(src_idx.first, src_idx.second).second << ") tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
            cerr << "tar_idx: (" << tar_idx.first << ", " << tar_idx.second << ")" << endl;
            cerr << "tar position: (" << Idx2LLcoor(tar_idx.first, tar_idx.second).first << ", " << Idx2LLcoor(tar_idx.first, tar_idx.second).second << ")" << endl;
            exit(1);
        }
    }
}

void Router::A_star(int net_id)
{
    vector<vector<pair<int,int>>> parent(GRID_DIM_VER, vector<pair<int,int>>(GRID_DIM_HOR, {-1,-1}));
    vector<vector<double>> g_cost(GRID_DIM_VER, vector<double>(GRID_DIM_HOR, numeric_limits<double>::max()));
    vector<vector<double>> f_cost(GRID_DIM_VER, vector<double>(GRID_DIM_HOR, numeric_limits<double>::max()));
    /*priority_queue*/set<ASTarNode> pq;
    unordered_map<int, set<ASTarNode>::iterator> pq_map;

    // starting from chip1, end at chip2
    pair<int, int> src_idx = LLcoor2Idx(chip1.getBump(net_id).x, chip1.getBump(net_id).y);
    pair<int, int> tar_idx = LLcoor2Idx(chip2.getBump(net_id).x, chip2.getBump(net_id).y);

    // set the initial value of g score and f score for the starting node
    g_cost[src_idx.first][src_idx.second] = 0;
    f_cost[src_idx.first][src_idx.second] = h_est(src_idx, tar_idx);

    // push the starting node to the priority queue
    pq.insert({src_idx, f_cost[src_idx.first][src_idx.second]});
    int src_key = src_idx.first * GRID_DIM_HOR + src_idx.second;
    pq_map[src_key] = pq.begin();

    while(!pq.empty())
    {
        // get the node with lowest f cost in the pq
        pair<int, int> curNodeIdx = pq.begin()->idx;
        // delete the node from the pq
        pq.erase(pq.begin());
        
        if(curNodeIdx == tar_idx)
        {
            // backtrack the path
            backTrack(parent, src_idx, tar_idx, nets[net_id].wire_segs);
            // calculate the cost of the net
            nets[net_id].cost = g_cost[tar_idx.first][tar_idx.second];
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
            double new_g_cost = g_cost[curNodeIdx.first][curNodeIdx.second] + exact_cost(curNodeIdx, neighbor, parent);
            if(new_g_cost < g_cost[neighbor.first][neighbor.second])
            {
                parent[neighbor.first][neighbor.second] = curNodeIdx;
                g_cost[neighbor.first][neighbor.second] = new_g_cost;
                f_cost[neighbor.first][neighbor.second] = g_cost[neighbor.first][neighbor.second] + h_est(neighbor, tar_idx);
                
                // find the neighbor in the pq
                int neighbor_key = neighbor.first * GRID_DIM_HOR + neighbor.second;
                auto it = pq_map.find(neighbor_key);
                if(it!= pq_map.end())
                {
                    pq.erase(it->second);
                }
                
                pq.insert({neighbor, f_cost[neighbor.first][neighbor.second]});
                pq_map[neighbor_key] = pq.begin();
            }
        }
    }

    cerr << "!!: A star search for net "<< net_id <<"failed!"<<endl;
    exit(1);
    
}