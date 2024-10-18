#ifndef BSTREE_H
#define BSTREE_H

#include "DEBUG.h"
#include <unordered_map>
#include <iostream>
#include "struct.h"
using namespace std;

class BSTree
{
private:
    unordered_map<string, Block *> BlockName2Ptr;
    unordered_map<string, Point> TermName2Loc;

public:
    Block *findBlock(string BlockName);
    Point getTermLoc(string TermName);
};

#endif