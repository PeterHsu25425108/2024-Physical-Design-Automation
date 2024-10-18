#include <iostream>
#include <vector>
#include "DEBUG.h"
#include <unordered_map>
#include "struct.h"
#include "BSTree.h"
#include <limits>
using namespace std;

BSTree::BSTree()
{
    root = nullptr;
    numBlocks = 0;
    numNets = 0;
    BoundaryWidth = 0;
    BoundaryHeight = 0;
}

BSTree::~BSTree()
{
    for (auto &pair : BlockName2Ptr)
    {
        delete pair.second;
    }
}

Block *BSTree::findBlock(string BlockName) const
{
    if (BlockName2Ptr.find(BlockName) == BlockName2Ptr.end())
    {
        cerr << "ERROR: BSTree::findBlock(" << BlockName << ") failed because " << BlockName << " does not exist in BlockName2Ptr" << endl;
        exit(1);
    }
    return BlockName2Ptr.at(BlockName);
}

Point BSTree::getTermLoc(string TermName) const
{
    if (TermName2Loc.find(TermName) == TermName2Loc.end())
    {
        cerr << "ERROR: BSTree::getTermLoc(" << TermName << ") failed because " << TermName << " does not exist in TermName2Loc" << endl;
        exit(1);
    }
    return TermName2Loc.at(TermName);
}

void BSTree::addTerminal(string name, int x, int y)
{
    TermName2Loc[name] = {x, y};
    term_names.push_back(name);
    numTerms++;
}

void BSTree::addNet(Net net)
{
    netlist.push_back(net);
    numNets++;
}

void BSTree::insertBlock(string name, int width, int height)
{
    Block *new_block = new Block(name, width, height);
    BlockName2Ptr[name] = new_block;
    block_names.push_back(name);
    numBlocks++;
    if (root == nullptr)
    {
        root = new_block;
        root->setBL({0, 0});
        root->setWidth(width);
        root->setHeight(height);
    }
    else
    {
        // Randonly pick a Block to insert
        int rand_name_idx = rand() % numBlocks;
        Block *curr = BlockName2Ptr[block_names[rand_name_idx]];

        int iter = 0;
        while (curr->getLeft() != nullptr && curr->getRight() != nullptr)
        {
            bool go_left = rand() % 2;

            if (go_left)
            {
                curr = curr->getLeft();
            }
            else
            {
                curr = curr->getRight();
            }

            if (iter > numBlocks)
            {
                cerr << "ERROR: BSTree::insertBlock(" << name << ", " << width << ", " << height << ") stuck in inf loop" << endl;
                exit(1);
            }
        }

        if (curr->getLeft() == nullptr && curr->getRight() == nullptr)
        {
            bool go_left = rand() % 2;
            if (go_left)
            {
                curr->setLeft(new_block);
            }
            else
            {
                curr->setRight(new_block);
            }
        }
        else if (curr->getLeft() == nullptr)
        {
            curr->setLeft(new_block);
        }
        else
        {
            curr->setRight(new_block);
        }
    }
    updateContour();
    // prepare for cost evaluation in SA
    prepareForCost();
}

void BSTree::updateContour()
{
    // clear the contour list
    contour_list.clear();
}

Block *BSTree::pickRandBlock() const
{
    int rand_name_idx = rand() % numBlocks;
    return BlockName2Ptr.at(block_names[rand_name_idx]);
}

pair<Block *, Block *> BSTree::pickRandPair() const
{
    int rand_name_idx0 = rand() % numBlocks;
    int rand_name_idx1 = rand() % numBlocks;
    while (rand_name_idx1 == rand_name_idx0)
    {
        rand_name_idx1 = rand() % numBlocks;
    }
    return {BlockName2Ptr.at(block_names[rand_name_idx0]), BlockName2Ptr.at(block_names[rand_name_idx1])};
}

void BSTree::RotateBlock(Block *block)
{
    int temp_width = block->getWidth();
    int temp_height = block->getHeight();

    block->setWidth(temp_height);
    block->setHeight(temp_width);

    // Update the contour
    updateContour();
    // prepare for cost evaluation in SA
    prepareForCost();
}

// Swap the position of two blocks by swapping their parent, left, right
// and :
// 1. If the block is the root, then set the root to the other block
void BSTree::SwapBlock(string block1, string block2)
{
    Block *block1_ptr = findBlock(block1);
    Block *block2_ptr = findBlock(block2);

    Block *block1_parent = block1_ptr->getParent();
    Block *block2_parent = block2_ptr->getParent();
    Block *block1_left = block1_ptr->getLeft();
    Block *block2_left = block2_ptr->getLeft();
    Block *block1_right = block1_ptr->getRight();
    Block *block2_right = block2_ptr->getRight();

    // If the block is the root, then set the root to the other block
    if (block1_parent == nullptr)
    {
        root = block2_ptr;
    }
    else if (block2_parent == nullptr)
    {
        root = block1_ptr;
    }

    // Update the left/right of the parent
    if (block1_ptr->getChildType() == LEFT_CHILD)
    {
        block1_parent->setLeft(block2_ptr);
    }
    else
    {
        block1_parent->setRight(block2_ptr);
    }

    if (block2_ptr->getChildType() == LEFT_CHILD)
    {
        block2_parent->setLeft(block1_ptr);
    }
    else
    {
        block2_parent->setRight(block1_ptr);
    }

    // Update the parent of the left child and right child
    if (block1_left != nullptr)
    {
        block1_left->setParent(block2_ptr);
    }
    if (block1_right != nullptr)
    {
        block1_right->setParent(block2_ptr);
    }
    if (block2_left != nullptr)
    {
        block2_left->setParent(block1_ptr);
    }
    if (block2_right != nullptr)
    {
        block2_right->setParent(block1_ptr);
    }

    // Swapt the parent
    block1_ptr->setParent(block2_parent);
    block2_ptr->setParent(block1_parent);

    // Swap the left
    block1_ptr->setLeft(block2_left);
    block2_ptr->setLeft(block1_left);

    // Swap the right
    block1_ptr->setRight(block2_right);
    block2_ptr->setRight(block1_right);

    // Update the contour
    updateContour();
    // prepare for cost evaluation in SA
    prepareForCost();
}

void BSTree::calcBoundArea()
{
    bounding_area = BoundaryWidth * BoundaryHeight;
}

void BSTree::calcAspectRatio()
{
    aspect_ratio = (double)BoundaryWidth / (double)BoundaryHeight;
}

void BSTree::calcTotHPWL()
{
    tot_HPWL = 0;
    for (Net &net : netlist)
    {
        net.calcHPWL();
        tot_HPWL += net.getHPWL();
    }
}