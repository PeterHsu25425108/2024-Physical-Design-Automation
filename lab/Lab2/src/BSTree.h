#ifndef BSTREE_H
#define BSTREE_H
#include "DEBUG.h"
#include <unordered_map>
#include <iostream>
#include "struct.h"
#include <list>
#include <vector>
#include <algorithm>
#include <set>
using namespace std;

// Store a horizontal edge on the contour
// x_left must be diffrent from x_right,
// which means if no block in BSTree, contour_list will be empty
struct Contour
{
    int x_left, x_right;
    int y_top;

    Contour(int x_left, int x_right, int y_top) : x_left(x_left), x_right(x_right), y_top(y_top) {}
    ~Contour() {}
    Contour() {}
    /*bool operator<(const Contour &rhs) const
    {
        return x_left < rhs.x_left;
    }*/
};

// used for sorting the contour list
// and quickly finding the first contour that has x_left >= target_x
/*struct SetElement
{
    Contour *contour_ptr;
    SetElement(Contour *contour) : contour_ptr(contour) {}
    SetElement() { contour_ptr = nullptr; }
    ~SetElement() {}
    bool operator<(const SetElement &rhs) const
    {
        return contour_ptr->x_left < rhs.contour_ptr->x_left;
    }
};*/

class BSTree
{
private:
    /* == These data memberss stay constant after initialization == */
    unordered_map<string, Block *> BlockName2Ptr;
    unordered_map<string, Point> TermName2Loc;
    vector<Net> netlist;
    // For ierating the block and term
    vector<string> block_names;
    vector<string> term_names;
    int numBlocks;
    int numNets;
    int numTerms;

    /* == These data members will be updated == */
    list<Contour> contour_list;
    Block *root;
    int BoundaryWidth, BoundaryHeight;
    double tot_HPWL;
    double bounding_area;
    double aspect_ratio;
    double outOfBoundArea;
    /* ============================================= */

    // helper function for copy constructor
    void copyTreeNode(Block *copied_block);

    // update the contour list and the boundary width and height
    void updateContour();
    // the recursive helper function for updateContour
    // the boundary width and height,contour_list, the LB of each block will be updated
    void updateContour(Block *curr);

    // Cost function realted functions
    void calcTotHPWL();
    void calcBoundArea();
    void calcAspectRatio();
    // void calcOutOfBoundArea(int outline_width, int outline_height);

public:
    BSTree();
    ~BSTree();

    BSTree(const BSTree &copied_tree);
    // assignment operator
    BSTree &operator=(const BSTree &copied_tree);

    friend ostream &operator<<(ostream &os, const BSTree &tree);
    // ostream &printNode(ostream &os, const Block *curr);

    // basic getter and setter
    Block *getRoot() const { return root; }
    int getNumBlocks() const { return numBlocks; }
    int getNumNets() const { return numNets; }
    int getNumTerms() const { return numTerms; }
    int getBoundaryWidth() const { return BoundaryWidth; }
    int getBoundaryHeight() const { return BoundaryHeight; }
    vector<Net> getNetlist() const { return netlist; }
    list<Contour> getContourList() const { return contour_list; }

    Block *findBlock(string BlockName) const;
    Point getTermLoc(string TermName) const;

    void addNet(Net net);
    void addTerminal(string name, int x, int y);
    // Insert a block to the tree, the first block will be the root,
    void insertBlock(string name, int width, int height);

    // Picking a random block from the tree
    Block *pickRandBlock() const;
    // Picking a random pair of blocks from the tree
    pair<Block *, Block *> pickRandPair() const;

    // 3 operations for SA, the randomness is in SA
    // the only function that will change the tree structure
    void SwapBlock(string block1, string block2);
    void RotateBlock(Block *block);
    bool MoveBlock(Block *block1, Block *block2);
    // a helper function for MoveBlock that will decide who will be the parent
    // return false if the move is invalid, which means
    // 1. block1 and block2 are adjacent
    // 2. block1 and block2 both have left and right child
    bool WhoIsParent(const Block *block1, const Block *block2, Block *&parent, Block *&child, bool &become_left_child);

    // Return the val needed for cost evaluation
    double getTotHPWL() const { return tot_HPWL; }
    double getBoundingArea() const { return bounding_area; }
    double getAspectRatio() const { return aspect_ratio; }
    // double getOutOfBoundArea() const { return outOfBoundArea; }

    // prepare for cost evaluation in SA
    void prepareForCost()
    {
        calcTotHPWL();
        calcBoundArea();
        calcAspectRatio();
    }

    // print the block names and LB of each block
    void printBlockInfo(ostream &out) const
    {
        for (const string &name : block_names)
        {
            Block *block = BlockName2Ptr.at(name);
            out << name << block->getBL().x << " " << block->getBL().y << endl;
        }
    }
};

#endif