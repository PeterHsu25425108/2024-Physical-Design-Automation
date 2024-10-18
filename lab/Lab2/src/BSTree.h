#ifndef BSTREE_H
#define BSTREE_H
#include "DEBUG.h"
#include <unordered_map>
#include <iostream>
#include "struct.h"
#include <list>
#include <vector>
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
};

class BSTree
{
private:
    unordered_map<string, Block *> BlockName2Ptr;
    unordered_map<string, Point> TermName2Loc;
    Block *root;
    vector<Net> netlist;
    list<Contour> contour_list;

    // For ierating the block and term
    // stay constant after initialization
    vector<string> block_names;
    vector<string> term_names;

    int numBlocks;
    int numNets;
    int numTerms;
    int BoundaryWidth, BoundaryHeight;

public:
    BSTree();
    ~BSTree();
    BSTree(const BSTree &copied_tree);
    friend ostream &operator<<(ostream &os, const BSTree &tree);

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
    void insertBlock(string name, int width, int height);
    void updateContour();

    // Picking a random block from the tree
    Block *pickRandBlock() const;
    // Picking a random pair of blocks from the tree
    pair<Block *, Block *> pickRandPair() const;

    // 3 operations for SA, the randomness is in SA
    void SwapBlock(string block1, string block2);
    void MoveBlock(Block *future_parent, bool left_child);
    void RotateBlock(Block *block);
};

#endif