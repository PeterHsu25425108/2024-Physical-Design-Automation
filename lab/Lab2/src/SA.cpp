#include <iostream>
#include <fstream>
#include "DEBUG.h"
#include "struct.h"
#include "BSTree.h"
#include "SA.h"
#include <vector>
#include <cmath>
using namespace std;

void SA::solve()
{

    bs_tree.prepareForCost();
    double cost = finalCost();
    double best_cost = cost;
    BSTree best_tree(bs_tree);

    int iter = 0;
    while (T > T_min)
    {
        for (int i = 0; i < 100; i++)
        {
            BSTree new_tree(bs_tree);
            // randomly pick a move from the 3 moves
            // SwapBlock, RotateBlock, MoveBlock
            int move = rand() % 3;
            switch (move)
            {
            case 0:
            {
                pair<Block *, Block *> blocks = bs_tree.pickRandPair();
                new_tree.SwapBlock(blocks.first->getName(), blocks.second->getName());
                break;
            }
            case 1:
            {
                Block *block = bs_tree.pickRandBlock();
                new_tree.RotateBlock(block);
                break;
            }
            case 2:
            {
                pair<Block *, Block *> blocks = bs_tree.pickRandPair();
                new_tree.MoveBlock(blocks.first, blocks.second);
                break;
            }
            default:
                break;
            }

            new_tree.prepareForCost();
            double new_cost = finalCost();
            double delta = new_cost - cost;

            if (delta < 0 || exp(-delta / T) > (double)rand() / RAND_MAX)
            {
                bs_tree = new_tree;
                cost = new_cost;
                if (cost < best_cost)
                {
                    best_cost = cost;
                    best_tree = bs_tree;
                }
            }
        }
        T *= t_decay;
        iter++;
    }
    bs_tree = best_tree;
}

void SA::parseBlock(ifstream &block_file)
{
    string temp;
    block_file >> temp;
    block_file >> outlineWidth >> outlineHeight;

    block_file >> temp;
    block_file >> numBlocks;

    block_file >> temp;
    block_file >> numTerms;

    for (int i = 0; i < numBlocks; i++)
    {
        string name;
        int width, height;
        block_file >> name >> width >> height;
        bs_tree.insertBlock(name, width, height);
    }

    for (int i = 0; i < numTerms; i++)
    {
        string name;
        int x, y;
        block_file >> name >> temp >> x >> y;
        bs_tree.addTerminal(name, x, y);
    }
}

void SA::parseNet(ifstream &net_file)
{
    string temp;
    net_file >> temp;
    net_file >> numNets;
    for (int i = 0; i < numNets; i++)
    {
        Net net;
        int netDegree;
        net_file >> temp >> netDegree;
        for (int j = 0; j < netDegree; j++)
        {
            string pin_name;
            net_file >> pin_name;
            net.addPin(pin_name);
        }
        bs_tree.addNet(net);
    }
}

void SA::writeOutput(ostream &out, double time_taken)
{
    // bs_tree.prepareForCost();

    double final_cost = finalCost();
    out << final_cost << endl;

    // print bounding area
    out << bs_tree.getBoundingArea() << endl;

    // print bounding width and height
    out << bs_tree.getBoundaryWidth() << " " << bs_tree.getBoundaryHeight() << endl;

    // print program running time
    out << time_taken << endl;

    // print block names and their locations
    bs_tree.printBlockInfo(out);
}