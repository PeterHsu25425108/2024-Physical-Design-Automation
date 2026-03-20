#include <iostream>
#include <fstream>
#include "DEBUG.h"
#include "struct.h"
#include "BSTree.h"
#include "SA.h"
#include <vector>
#include <cmath>
#include <chrono>
#include <ctime>
#include <cstdlib>
using namespace std;
using namespace std::chrono;

void SA::solve()
{
    // start = high_resolution_clock::now();

    ofstream SA_cost_file("SA_cost.txt");

    bs_tree.prepareForCost();
    // double cost = finalCost(bs_tree);
    double cost = SA_cost(bs_tree);

    if (SA_COST_OUT)
    {
        SA_cost_file << "Initial cost: " << cost << endl;
    }

    double best_cost = cost;
    BSTree best_tree(bs_tree);
    BSTree new_tree(bs_tree);

    if (PLOT_FINAL)
    {
        ofstream init_plot_file("../draw/text/init_plot.txt");
        new_tree.writePlotFile(init_plot_file, outlineWidth, outlineHeight);
        init_plot_file.close();
        // print the initial bounding area and HPWL
        cout << "Calculating initial HPWL and bounding area..." << endl;
        // new_tree.calcTotHPWL();
        // new_tree.calcBoundArea();
        cout << "Initial bounding area: " <<  bs_tree.getBoundingArea() << endl;
        cout << "Initial HPWL: " <<  bs_tree.getTotHPWL() << endl;
    }

    int iter = 0;
    int best_iter = 0;

    int changeTempIter = 1000;
    int reject = 0;

    while (/*T > T_min*/ true)
    {
        if (DEBUG_SA)
        {
            cout << endl
                 << "Iteration: " << iter + 1 << endl;
        }

        if (SA_COST_OUT)
            SA_cost_file << "Iteration: " << iter + 1 << endl;

        // randomly pick a move from the 3 moves
        // SwapBlock, RotateBlock, MoveBlock

        int move = rand() % 3;
        switch (move)
        {
        case 0:
        {

            if (DEBUG_SA)
            {
                cout << "Swapping blocks" << endl;
            }

            pair<Block *, Block *> blocks = new_tree.pickRandPair();

            if (DEBUG_SA)
            {
                cout << "Swapping " << blocks.first->getName() << " and " << blocks.second->getName() << endl;
            }

            if (SA_COST_OUT)
                SA_cost_file << "Swapping " << blocks.first->getName() << " and " << blocks.second->getName() << endl;

            new_tree.SwapBlock(blocks.first->getName(), blocks.second->getName());
            break;
        }
        case 1:
        {

            if (DEBUG_SA)
            {
                cout << "Rotating blocks" << endl;
            }

            Block *block = new_tree.pickRandBlock();
            if (SA_COST_OUT)
                SA_cost_file << "Rotating " << block->getName() << endl;

            if (DEBUG_SA)
            {
                cout << "Rotating " << block->getName() << endl;
            }
            new_tree.RotateBlock(block);
            break;
        }
        case 2:
        {

            if (DEBUG_SA)
            {
                cout << "Moving blocks" << endl;
            }
            pair<Block *, Block *> blocks = new_tree.pickRandPair();
            if (SA_COST_OUT)
                SA_cost_file << "Moving " << blocks.first->getName() << " and " << blocks.second->getName() << endl;

            if (DEBUG_SA)
            {
                cout << "Moving " << blocks.first->getName() << " and " << blocks.second->getName() << endl;
            }
            bool success = new_tree.MoveBlock(blocks.first, blocks.second);
            if (!success)
            {
                // swap the blocks if the move is invalid
                if (DEBUG_SA)
                {
                    cout << "Invalid move, swap the blocks" << endl;
                }
                new_tree.SwapBlock(blocks.first->getName(), blocks.second->getName());
            }
            break;
        }
        default:
            cerr << "Invalid move type: " << move << endl;
            exit(1);
            break;
        }

        // write the new tree to a plot file
        // the file name is "iter"+iter+"_plot.txt"
        if (PLOT_ITER)
        {
            ofstream plot_file("../draw/text/iter" + to_string(iter + 1) + "_plot.txt");
            new_tree.writePlotFile(plot_file, outlineWidth, outlineHeight);
            plot_file.close();
        }

        new_tree.prepareForCost();
        if (DEBUG_SA)
        {
            cout << "new tree after the operation: " << endl;
            cout << new_tree << endl;
        }
        // double new_cost = finalCost(new_tree);
        double new_cost = SA_cost(new_tree);
        double delta = new_cost - cost;

        /*if (delta < 0)
        {
            cout << "SA cost decreased by: " << -delta << endl;
        }*/

        // new_tree.prepareForCost();
        if (SA_COST_OUT)
        {
            SA_cost_file << " SA_cost: " << new_cost << endl;
            SA_cost_file << "final cost: " << finalCost(new_tree) << endl;
            SA_cost_file << "HPWL: " << new_tree.getTotHPWL() << endl;
            SA_cost_file << "Bound area: " << new_tree.getBoundingArea() << endl;
            SA_cost_file << "acceptance prob: " << exp(-delta / T) << endl;
        }

        if (DEBUG_SA)
        {
            cout << "Bound area: " << new_tree.getBoundingArea() << endl;
            cout << "HPWL: " << new_tree.getTotHPWL() << endl;

            cout << "new cost: " << new_cost << endl;
            cout << "current cost: " << cost << endl;
            cout << "best cost: " << best_cost << endl;
        }

        if (delta < 0 || exp(-delta / T) > (double)rand() / RAND_MAX)
        {
            if (SA_COST_OUT)
                SA_cost_file << "Accept the new tree" << endl;
            if (DEBUG_SA)
            {
                cout << "Accept the new tree" << endl;
                cout << "new tree structure: " << endl;
                cout << new_tree << endl;
            }

            bs_tree = new_tree;
            cost = new_cost;
            if (cost < best_cost /* && fitInOutline(new_tree)*/)
            {
                /*if (DEBUG_SA)
                {*/
                if (SA_COST_OUT)
                    SA_cost_file << "Updating best tree cuz it's better" << endl;
                //}

                best_cost = cost;
                best_tree = bs_tree;
                best_iter = iter;
            }
        }
        else if (!fitInOutline(best_tree) && fitInOutline(new_tree))
        {
            /*if (DEBUG_SA)
            {*/
            if (SA_COST_OUT)
                SA_cost_file << "Updating best tree cuz new_tree is the 1st feasible sol" << endl;
            //}
            best_tree = new_tree;
            best_cost = new_cost;
            best_iter = iter;
        }
        else
        {
            if (SA_COST_OUT)
                SA_cost_file << "Reject the new tree" << endl;
            new_tree = bs_tree;
            reject++;
        }

        if ((iter + 1) % changeTempIter == 0)
        {
            T *= t_decay;

            double reject_rate = (double)reject / changeTempIter;
            /*if (reject_rate > 0.95)
            {
                if (SA_COST_OUT)
                {
                    SA_cost_file << "iteration: " << iter + 1 << " reject rate: " << reject_rate << endl;
                    SA_cost_file << "Reject rate is too high, stop the iteration" << endl;
                }
                break;
            }*/
        }

        // if (DEBUG_SA)
        //{

        //}

        if (DEBUG_BREAK && iter >= DEBUG_MAXITER)
        {
            if (SA_COST_OUT)
            {
                SA_cost_file << "iteration: " << iter + 1 << " cost: " << cost << endl;
                SA_cost_file << "Debug break, stop the iteration" << endl;
            }
            break;
        }

        if (((double)clock() / CLOCKS_PER_SEC) > 300 * 0.95)
        {
            if (SA_COST_OUT)
            {
                SA_cost_file << "iteration: " << iter + 1 << " cost: " << cost << endl;
                SA_cost_file << "Time limit reached, stop the iteration" << endl;
            }
            break;
        }

        iter++;
        /*if (PLOT_FINAL)
        {
            ofstream final_plot_file("../draw/text/final_plot.txt");
            best_tree.writePlotFile(final_plot_file, outlineWidth, outlineHeight);
            final_plot_file.close();
        }*/
    }
    bs_tree = best_tree;

    if (SA_COST_OUT)
    {
        SA_cost_file << "Best SA_cost: " << best_cost << endl;
        SA_cost_file << "Best final cost: " << finalCost(best_tree) << endl;
        SA_cost_file << "Best HPWL: " << best_tree.getTotHPWL() << endl;
        SA_cost_file << "Best bound area: " << best_tree.getBoundingArea() << endl;
    }

    if (PLOT_FINAL)
    {
        ofstream final_plot_file("../draw/text/final_plot.txt");
        best_tree.writePlotFile(final_plot_file, outlineWidth, outlineHeight);
        final_plot_file.close();
        // print the final bounding area and HPWL
        cout << "Calculating final HPWL and bounding area..." << endl;
        best_tree.calcTotHPWL();
        best_tree.calcBoundArea();
        cout << "Final bounding area: " << best_tree.getBoundingArea() << endl;
        cout << "Final HPWL: " << best_tree.getTotHPWL() << endl;
    }
    if (DEBUG_SA)
    {
        cout << "best tree selected at iteration: " << best_iter << endl;
    }

    if (fitInOutline(best_tree))
    {
        cout << "Best tree fits in the outline" << endl;
    }
    else
    {
        cout << "Best tree does not fit in the outline" << endl;
    }

    // end = high_resolution_clock::now();

    SA_cost_file.close();
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

    if (DEBUG_PARSE)
    {
        cout << " == Parsing block file == " << endl;
        cout << "Outline width: " << outlineWidth << " Outline height: " << outlineHeight << endl;
        cout << "Number of blocks: " << numBlocks << endl;
        cout << "Number of terms: " << numTerms << endl;
    }

    for (int i = 0; i < numBlocks; i++)
    {
        string name;
        int width, height;
        block_file >> name >> width >> height;
        if (DEBUG_PARSE)
        {

            cout << "block name: " << name << " width: " << width << " height: " << height << endl;
        }
        bs_tree.insertBlock(name, width, height);

        if (DEBUG_CONTOUR)
        {
            cout << "Contour list after insert block: " << endl;
            for (auto &contour : bs_tree.getContourList())
            {
                cout << "x_left: " << contour.x_left << " x_right: " << contour.x_right << " y_top: " << contour.y_top << endl;
            }
        }

        if (DEBUG_INSERT || DEBUG_CONTOUR)
        {
            cout << "bs tree after insert block: " << endl;
            cout << bs_tree << endl;
        }
    }

    for (int i = 0; i < numTerms; i++)
    {
        string name;
        int x, y;
        block_file >> name >> temp >> x >> y;
        bs_tree.addTerminal(name, x, y);

        if (DEBUG_PARSE)
        {
            cout << "Inserting terminal: " << name << " x: " << x << " y: " << y << endl;
        }
    }

    if (DEBUG_PARSE)
    {
        cout << " == Parsing block file done == " << endl
             << endl;
    }
}

void SA::parseNet(ifstream &net_file)
{

    if (DEBUG_INSERT)
    {
        cout << "bs tree before parse net: " << endl;
        cout << bs_tree << endl;
    }

    string temp;
    net_file >> temp;
    net_file >> numNets;
    for (int i = 0; i < numNets; i++)
    {
        Net net(&bs_tree);
        int netDegree;
        net_file >> temp >> netDegree;

        if (DEBUG_PARSE)
        {
            cout << "Inserting net: " << endl
                 << " degree: " << netDegree << endl;
        }

        for (int j = 0; j < netDegree; j++)
        {
            string pin_name;
            net_file >> pin_name;
            if (DEBUG_PARSE)
            {
                cout << "pin name: " << pin_name << endl;
            }
            net.addPin(pin_name);
            if (DEBUG_PARSE)
            {
                cout << "addPin done" << endl;
            }
        }
        if (DEBUG_PARSE)
        {
            cout << "addNet" << endl;
        }
        bs_tree.addNet(net);
        if (DEBUG_PARSE)
        {
            cout << "addNet done" << endl;
        }
    }

    if (DEBUG_PARSE)
    {
        cout << " == Parsing net file done == " << endl
             << endl;
    }

    if (DEBUG_PARSE || DEBUG_NET)
    {
        cout << " tree structure after parsing block and net file: " << endl;
        cout << bs_tree << endl;
    }
}

void SA::writeOutput(ostream &out)
{
    bs_tree.prepareForCost();

    double final_cost = finalCost(this->bs_tree);
    out << (long)final_cost << endl;

    // total HPWL
    out << bs_tree.getTotHPWL() << endl;

    // print bounding area
    out << bs_tree.getBoundaryWidth() * bs_tree.getBoundaryHeight() << endl;

    // print bounding width and height
    out << bs_tree.getBoundaryWidth() << " " << bs_tree.getBoundaryHeight() << endl;

    // double time_taken = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    double time_taken = (double)clock() / CLOCKS_PER_SEC;
    // print program running time
    out << time_taken << endl;

    // print block names and their locations
    bs_tree.printBlockInfo(out);
}