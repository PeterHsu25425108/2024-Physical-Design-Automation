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

    if (DEBUG_INSERT)
    {
        cout << "BSTree constructor" << endl;
    }

    root = nullptr;
    numBlocks = 0;
    numNets = 0;
    BoundaryWidth = 0;
    BoundaryHeight = 0;
    numTerms = 0;
}

BSTree::~BSTree()
{
    for (auto &pair : BlockName2Ptr)
    {
        delete pair.second;
    }
}

BSTree &BSTree::operator=(const BSTree &copied_tree)
{

    if (DEBUG_SA)
    {
        cout << "BSTree assignment operator" << endl;
        cout << "BSTree before assignment" << endl;
        cout << *this << endl;
        cout << "copied_tree" << endl;
        cout << copied_tree << endl;
    }

    if (this == &copied_tree)
    {
        if (DEBUG_SA)
        {
            cout << "BSTree assignment operator self assignment" << endl;
        }
        return *this;
    }

    // delete the old tree
    for (auto &pair : BlockName2Ptr)
    {
        delete pair.second;
    }
    BlockName2Ptr.clear();
    root = nullptr;
    numBlocks = copied_tree.numBlocks;
    numNets = copied_tree.numNets;
    BoundaryWidth = copied_tree.BoundaryWidth;
    BoundaryHeight = copied_tree.BoundaryHeight;
    TermName2Loc = copied_tree.TermName2Loc;
    netlist = copied_tree.netlist;
    block_names = copied_tree.block_names;
    term_names = copied_tree.term_names;
    contour_list = copied_tree.contour_list;

    // BlockName2Ptr and root(and the entire tree) should be deep copied
    // first deep copy the BlockName2Ptr
    for (auto &pair : copied_tree.BlockName2Ptr)
    {
        Block *new_block = new Block(*pair.second);
        BlockName2Ptr[pair.first] = new_block;
    }
    // use the BlockName2Ptr to deep copy the tree structure,
    // namely assign the parent, left, right of each block in *this to the corresponding block in BlockName2Ptr
    copyTreeNode(copied_tree.root);

    if (DEBUG_SA)
    {
        cout << "BSTree after assignment" << endl;
        cout << *this << endl;
    }

    return *this;
}

BSTree::BSTree(const BSTree &copied_tree)
{

    if (DEBUG_SA)
    {
        cout << "BSTree copy constructor" << endl;
    }

    numBlocks = copied_tree.numBlocks;
    numNets = copied_tree.numNets;
    BoundaryWidth = copied_tree.BoundaryWidth;
    BoundaryHeight = copied_tree.BoundaryHeight;
    TermName2Loc = copied_tree.TermName2Loc;
    netlist = copied_tree.netlist;
    block_names = copied_tree.block_names;
    term_names = copied_tree.term_names;
    contour_list = copied_tree.contour_list;

    // BlockName2Ptr and root(and the entire tree) should be deep copied
    // first deep copy the BlockName2Ptr
    for (auto &pair : copied_tree.BlockName2Ptr)
    {
        Block *new_block = new Block(*pair.second);
        BlockName2Ptr[pair.first] = new_block;
    }
    // use the BlockName2Ptr to deep copy the tree structure,
    // namely assign the parent, left, right of each block in *this to the corresponding block in BlockName2Ptr
    copyTreeNode(copied_tree.root);
}

void BSTree::copyTreeNode(Block *copied_block)
{
    if (copied_block == nullptr)
    {
        return;
    }

    string name = copied_block->getName();
    Block *copied_parent = copied_block->getParent();
    if (copied_parent != nullptr)
    {
        string parent_name = copied_parent->getName();
        Block *parent = BlockName2Ptr[parent_name];
        BlockName2Ptr[name]->setParent(parent);
    }
    else
    {
        root = BlockName2Ptr[name];
    }

    Block *copied_left = copied_block->getLeft();
    if (copied_left != nullptr)
    {
        string left_name = copied_left->getName();
        Block *left = BlockName2Ptr[left_name];
        BlockName2Ptr[name]->setLeft(left);
    }

    Block *copied_right = copied_block->getRight();
    if (copied_right != nullptr)
    {
        string right_name = copied_right->getName();
        Block *right = BlockName2Ptr[right_name];
        BlockName2Ptr[name]->setRight(right);
    }

    copyTreeNode(copied_block->getLeft());
    copyTreeNode(copied_block->getRight());
}

Block *BSTree::findBlock(string BlockName) const
{

    if (BlockName2Ptr.empty() || BlockName2Ptr.find(BlockName) == BlockName2Ptr.end())
        return nullptr;
    else
        return BlockName2Ptr.at(BlockName);
}

Point BSTree::getTermLoc(string TermName) const
{
    if (TermName2Loc.find(TermName) == TermName2Loc.end())
        return {-1, -1};
    else
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

Block::Block(const Block &block)
{
    name = block.name;
    width = block.width;
    height = block.height;
    BL = block.BL;

    // leave the parent, left, right to be nullptr
    parent = nullptr;
    left = nullptr;
    right = nullptr;
}

void BSTree::insertBlock(string name, int width, int height)
{

    if (DEBUG_INSERT)
    {
        cout << "Inserting block: " << name << " width: " << width << " height: " << height << endl;
    }

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
        while (block_names[rand_name_idx] == name)
        {
            rand_name_idx = rand() % numBlocks;
        }
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
        new_block->setParent(curr);
    }

    // update the contour list and the boundary width and height
    // the new_block should be a leaf node

    // check if the new_block is a leaf node
    if (new_block->getLeft() != nullptr || new_block->getRight() != nullptr)
    {
        cerr << "ERROR: BSTree::insertBlock(" << name << ", " << width << ", " << height << ") failed because the new_block is not a leaf node" << endl;
        exit(1);
    }
    // this should take constant time since the new_block is a leaf node
    updateContour(new_block);
}

// update the contour list and the boundary width and height
void BSTree::updateContour()
{
    // clear the contour list
    contour_list.clear();
    // update the contour list and the boundary width and height
    updateContour(root);
}

// the recursive helper function for updateContour
// the boundary width and height,contour_list, the LB of each block will be updated
void BSTree::updateContour(Block *curr)
{

    // stop condition
    if (curr == nullptr)
    {
        return;
    }

    if (DEBUG_CONTOUR)
    {
        cout << "***** updateContour: " << curr->getName() << " width: " << curr->getWidth() << ", height: " << curr->getHeight() << " *****" << endl;
    }

    if (curr->getChildType() == ROOT)
    {
        curr->setBL({0, 0});
        contour_list.push_back(Contour(0, curr->getWidth(), curr->getHeight()));
        BoundaryWidth = curr->getWidth();
        BoundaryHeight = curr->getHeight();
        // contour_xleft.insert(SetElement(&contour_list.back()));
    }
    else
    {
        int x = (curr->getChildType() == RIGHT_CHILD) ? curr->getParent()->getBL().x : curr->getParent()->getBL().x + curr->getParent()->getWidth();
        curr->setBLX(x);
        curr->setBLY(0);
        BoundaryWidth = max(BoundaryWidth, x + curr->getWidth());

        auto list_it = contour_list.begin();
        while (list_it->x_right <= x && list_it != prev(contour_list.end()))
        {
            if (DEBUG_CONTOUR)
            {
                cout << "list_it: " << list_it->x_left << " " << list_it->x_right << " " << list_it->y_top << endl;
            }
            list_it = next(list_it);
        }

        if (DEBUG_CONTOUR)
        {
            cout << "first contour elemet: " << list_it->x_left << " " << list_it->x_right << " " << list_it->y_top << endl;
            cout << "curr block: " << curr->getName() << " x: " << x << " y: " << curr->getBL().y << endl;
        }

        // curr->setBLY(list_it->y_top);
        //  the iterator where the new contour should be inserted
        //  the new contour will be inserted before the insert_pos
        list<Contour>::iterator insert_pos = list_it;

        int blockXleft = x;
        int blockXright = x + curr->getWidth();

        // update contour_list and the boundary width and height of the LB of the block
        while (list_it != contour_list.end() && list_it->x_left <= blockXright)
        {

            if (DEBUG_CONTOUR)
            {
                cout << "list_it: " << "x_left: " << list_it->x_left << " x_right: " << list_it->x_right << " y_top: " << list_it->y_top << endl;
                cout << "whole contour list: " << endl;
                for (auto &contour : contour_list)
                {
                    cout << "(x_left: " << contour.x_left << " x_right: " << contour.x_right << " y_top: " << contour.y_top << ") ";
                }
                cout << endl;
            }

            // handle the contour list

            if (DEBUG_CONTOUR)
            {
                cout << "blockXleft: " << blockXleft << " blockXright: " << blockXright << endl;
            }

            // case 1: the contour is completely covered by the block
            // remove the contour
            if (list_it->x_left >= blockXleft && list_it->x_right <= blockXright)
            {
                if (DEBUG_CONTOUR)
                {
                    cout << "case 1" << endl;
                }
                curr->setBLY(max(curr->getBL().y, list_it->y_top));
                BoundaryHeight = max(BoundaryHeight, curr->getTR().y);
                if (DEBUG_CONTOUR)
                {
                    cout << "remove contour: " << list_it->x_left << " " << list_it->x_right << " " << list_it->y_top << endl;
                }

                // cerr << "size before erase: " << contour_list.size() << endl;
                // cerr << "erase" << endl;
                list_it = contour_list.erase(list_it);
                if (DEBUG_CONTOUR)
                {

                    if (list_it == contour_list.end())
                    {
                        cout << "list_it is at the end" << endl;
                    }
                    else
                    {
                        cout << "return val of erase: " << list_it->x_left << " " << list_it->x_right << " " << list_it->y_top << endl;
                        if (list_it == contour_list.begin())
                            cout << "list_it is at the begin" << endl;
                    }
                }
                if (list_it != contour_list.end() && list_it != contour_list.begin())
                    list_it = prev(list_it);
                // cerr << "size after erase: " << contour_list.size() << endl;
                if (DEBUG_CONTOUR)
                {
                    cout << "list_it after erase: " << list_it->x_left << " " << list_it->x_right << " " << list_it->y_top << endl;
                }
                if (list_it == contour_list.end())
                {
                    insert_pos = contour_list.end();
                    if (DEBUG_CONTOUR)
                    {
                        cout << "insert_pos: end" << endl;
                    }
                }

                if (list_it == contour_list.begin())
                {
                    if (DEBUG_CONTOUR)
                    {
                        cout << "list_it is at the begin, so continue" << endl;
                    }
                    continue;
                }
            }
            // case 2: the right side of the contour is covered by the block, but the left side is not
            else if (list_it->x_left < blockXleft && list_it->x_right > blockXleft &&
                     list_it->x_right <= blockXright)
            {
                if (DEBUG_CONTOUR)
                {
                    cout << "case 2" << endl;
                }
                curr->setBLY(max(curr->getBL().y, list_it->y_top));
                BoundaryHeight = max(BoundaryHeight, curr->getTR().y);

                if (DEBUG_CONTOUR)
                {
                    cout << "shrink the x_right of contour: " << list_it->x_left << " " << list_it->x_right << " " << list_it->y_top << endl;
                    cout << "to " << blockXleft << endl;
                }

                // shrink the x_right of list_it
                list_it->x_right = blockXleft;
                // the new contour will be inserted at the back of list_it
                // insert_pos = next(list_it);
                if (next(list_it) == contour_list.end())
                {
                    insert_pos = contour_list.end();
                }
            }
            // case 3: the left side of the contour is covered by the block, but the right side is not
            else if (list_it->x_left < blockXright && list_it->x_right > blockXright &&
                     list_it->x_left >= blockXleft)
            {
                if (DEBUG_CONTOUR)
                {
                    cout << "case 3" << endl;
                    cout << "curr->getBL().y: " << curr->getBL().y << " list_it->y_top: " << list_it->y_top << endl;
                }
                curr->setBLY(max(curr->getBL().y, list_it->y_top));
                BoundaryHeight = max(BoundaryHeight, curr->getTR().y);
                if (DEBUG_CONTOUR)
                {
                    cout << "shrink the x_left of contour: " << list_it->x_left << " " << list_it->x_right << " " << list_it->y_top << endl;
                    cout << "to " << blockXright << endl;
                }

                // shrink the x_left of list_it
                list_it->x_left = blockXright;
                // the new contour will be inserted at the front of list_it
                insert_pos = list_it;
            }
            // case 4: both sides stick out
            else if (list_it->x_left < blockXleft && list_it->x_right > blockXright)
            {
                if (DEBUG_CONTOUR)
                {
                    cout << "case 4" << endl;
                }
                curr->setBLY(max(curr->getBL().y, list_it->y_top));
                BoundaryHeight = max(BoundaryHeight, curr->getTR().y);
                if (DEBUG_CONTOUR)
                {
                    cout << "split the contour: " << list_it->x_left << " " << list_it->x_right << " " << list_it->y_top << endl;
                    cout << "to be: " << list_it->x_left << " " << blockXleft << " " << list_it->y_top << endl;
                    cout << "and: " << blockXright << " " << list_it->x_right << " " << list_it->y_top << endl;
                }

                // split the contour into two, while the new contour will be inserted between the two
                Contour new_contour_right(blockXright, list_it->x_right, list_it->y_top);
                list_it->x_right = blockXleft;
                // insert the new contour
                contour_list.insert(next(list_it), new_contour_right);
                // the new contour will be inserted at the back of list_it
                insert_pos = next(list_it);
            }
            // case 5: the contour is right on the right side of the block
            else if (blockXleft == list_it->x_right)
            {
                if (DEBUG_CONTOUR)
                {
                    cout << "case 5" << endl;
                }
                /*curr->setBLY(max(curr->getBL().y, list_it->y_top));
                BoundaryHeight = max(BoundaryHeight, curr->getTR().y);*/

                if (next(list_it) == contour_list.end())
                {
                    if (DEBUG_CONTOUR)
                    {
                        cout << "next(list_it) == contour_list.end()" << endl;
                        cout << "set block " << curr->getName() << " BLY to 0" << endl;
                    }
                    insert_pos = contour_list.end();
                    curr->setBLY(0);
                    if (DEBUG_CONTOUR)
                    {
                        cout << "block " << curr->getName() << " BL: " << curr->getBL().x << " " << curr->getBL().y << endl;
                    }
                }
            }
            // case 6: the contour is right on the left side of the block
            else if (blockXright == list_it->x_left)
            {
                if (DEBUG_CONTOUR)
                {
                    cout << "case 6" << endl;
                }
                /*curr->setBLY(max(curr->getBL().y, list_it->y_top));
                BoundaryHeight = max(BoundaryHeight, curr->getTR().y);*/
                insert_pos = list_it;
                if (list_it == contour_list.begin())
                {
                    if (DEBUG_CONTOUR)
                    {
                        cout << "set block " << curr->getName() << " BLY to 0" << endl;
                    }
                    curr->setBLY(0);
                    if (DEBUG_CONTOUR)
                    {
                        cout << "block " << curr->getName() << " BL: " << curr->getBL().x << " " << curr->getBL().y << endl;
                    }
                }
            }
            else
            {
                cerr << "ERROR: BSTree::updateContour() failed because the contour x doesn't overlap with the block" << endl;
                cerr << "block: " << curr->getName() << "blockXleft: " << blockXleft << " blockXright: " << blockXright << endl;
                cerr << "contour: x_left: " << list_it->x_left << " x_right: " << list_it->x_right
                     << "y_top: " << list_it->y_top << endl;
                exit(1);
            }

            // insert the new contour
            /*Contour new_contour(blockXleft, blockXright, curr->getTR().y);
            contour_list.insert(insert_pos, new_contour);

            if (DEBUG_CONTOUR)
            {
                cout << "insert contour: " << new_contour.x_left << " " << new_contour.x_right << " " << new_contour.y_top << endl;
            }*/

            if (list_it == contour_list.end())
            {
                if (DEBUG_CONTOUR)
                {
                    cout << "break" << endl;
                }
                break;
            }

            list_it = next(list_it);

            if (DEBUG_CONTOUR)
            {
                if (list_it == contour_list.end())
                {
                    cout << "last contour" << endl;
                }
                else
                {
                    cout << "next contour: " << list_it->x_left << " " << list_it->x_right << " " << list_it->y_top << endl;
                }
            }
        }
        if (DEBUG_CONTOUR)
        {
            cout << "insert contour: " << blockXleft << " " << blockXright << " " << curr->getTR().y << endl;
            cout << "the insert pos: " << insert_pos->x_left << " " << insert_pos->x_right << " " << insert_pos->y_top << endl;
        }

        // insert the new contour
        Contour new_contour(blockXleft, blockXright, curr->getTR().y);
        contour_list.insert(insert_pos, new_contour);

        if (DEBUG_CONTOUR)
        {
            cout << "insert contour: " << new_contour.x_left << " " << new_contour.x_right << " " << new_contour.y_top << endl;
        }

        // Find the first contour that has x_left >= x
        /*auto it = contour_xleft.lower_bound(SetElement(nullptr));
        Contour *firstToUpdate = it->contour_ptr;
        while (firstToUpdate->x_left < x && it != contour_xleft.begin())
        {
            it--;
            firstToUpdate = it->contour_ptr;
        }*/
    }

    if (DEBUG_CONTOUR)
    {
        // print the cooridnates of the block
        cout << "Block: " << curr->getName() << " x: " << curr->getBL().x << " y: " << curr->getBL().y << endl;
    }

    // update the contour list in preorder
    // check if the left is the same as curr node
    if (curr->getLeft() == curr)
    {
        cerr << "current node: " << curr->getName() << endl
             << " curr->getLeft(): " << curr->getLeft()->getName() << endl;
        cerr << "ERROR: BSTree::updateContour() failed because the left child is the same as the current node" << endl;
        exit(1);
    }
    updateContour(curr->getLeft());
    if (curr->getRight() == curr)
    {
        cerr << "ERROR: BSTree::updateContour() failed because the right child is the same as the current node" << endl;
        exit(1);
    }
    updateContour(curr->getRight());

    if (DEBUG_CONTOUR)
    {
        cout << "***** updateContour return *****" << endl
             << endl;
    }
}

Block *BSTree::pickRandBlock() const
{

    if (numBlocks == 0)
    {
        cerr << "ERROR: BSTree::pickRandBlock() failed because the number of blocks is 0" << endl;
        cerr << "tree structure: " << endl;
        cerr << *this << endl;
        exit(1);
    }

    int rand_name_idx = rand() % numBlocks;
    return BlockName2Ptr.at(block_names[rand_name_idx]);
}

pair<Block *, Block *> BSTree::pickRandPair() const
{

    if (numBlocks < 2)
    {
        cerr << "ERROR: BSTree::pickRandPair() failed because the number of blocks is less than 2" << endl;
        cerr << "numBlocks: " << numBlocks << endl;
        cerr << "tree structure: " << endl;
        cerr << *this << endl;
        exit(1);
    }

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

    if (DEBUG_SWAP)
    {
        cout << "block1: " << block1 << " block2: " << block2 << endl;
    }

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

    if (block1_parent == block2_ptr || block2_parent == block1_ptr)
    {
        if (DEBUG_SWAP)
        {
            cout << "swap adjacent blocks" << endl;
        }

        Block *parent_block = block1_parent == block2_ptr ? block1_parent : block2_parent;
        Block *child_block = block1_parent == block2_ptr ? block1_ptr : block2_ptr;

        Block *grand_parent = parent_block->getParent();
        Block *sibling = child_block->getChildType() == LEFT_CHILD ? parent_block->getRight() : parent_block->getLeft();
        Block *left_grand_child = child_block->getLeft();
        Block *right_grand_child = child_block->getRight();
        CHILD_TYPE child_type = child_block->getChildType();
        CHILD_TYPE parent_type = parent_block->getChildType();

        // Update the left/right of the grand parent
        if (parent_type == LEFT_CHILD)
        {
            grand_parent->setLeft(child_block);
        }
        else if (parent_type == RIGHT_CHILD)
        {
            grand_parent->setRight(child_block);
        }

        // update the parent of the cousin
        if (sibling != nullptr)
        {
            sibling->setParent(child_block);
        }

        // Update the parent of the left and right grand child
        if (left_grand_child != nullptr)
        {
            left_grand_child->setParent(parent_block);
        }
        if (right_grand_child != nullptr)
        {
            right_grand_child->setParent(parent_block);
        }

        // Swap the parent
        child_block->setParent(grand_parent);
        parent_block->setParent(child_block);

        // update the left and right child of the parent
        parent_block->setLeft(left_grand_child);
        parent_block->setRight(right_grand_child);

        // update the left and right child of the child
        if (child_type == LEFT_CHILD)
        {
            child_block->setLeft(parent_block);
            child_block->setRight(sibling);
        }
        else if (child_type == RIGHT_CHILD)
        {
            child_block->setLeft(sibling);
            child_block->setRight(parent_block);
        }
    }
    else
    {
        if (DEBUG_SWAP)
        {
            cout << "swap non-adjacent blocks" << endl;
        }

        CHILD_TYPE block1_child_type = block1_ptr->getChildType();
        CHILD_TYPE block2_child_type = block2_ptr->getChildType();

        // Update the left/right of the parent
        if (block1_child_type == LEFT_CHILD)
        {
            block1_parent->setLeft(block2_ptr);
        }
        else if (block1_child_type == RIGHT_CHILD)
        {
            block1_parent->setRight(block2_ptr);
        }

        if (DEBUG_SWAP)
        {
            cout << "update the left/right of the parent of " << block1_ptr->getName() << endl;
        }

        if (block2_child_type == LEFT_CHILD)
        {
            block2_parent->setLeft(block1_ptr);
        }
        else if (block2_child_type == RIGHT_CHILD)
        {
            block2_parent->setRight(block1_ptr);
        }

        if (DEBUG_SWAP)
        {
            cout << "update the left/right of the parent of " << block2_ptr->getName() << endl;
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

        // Swap the parent
        block1_ptr->setParent(block2_parent);
        block2_ptr->setParent(block1_parent);

        // Swap the left
        block1_ptr->setLeft(block2_left);
        block2_ptr->setLeft(block1_left);

        // Swap the right
        block1_ptr->setRight(block2_right);
        block2_ptr->setRight(block1_right);
    }

    // Update the contour
    updateContour();
    // prepare for cost evaluation in SA
    prepareForCost();
}

bool BSTree::MoveBlock(Block *block1, Block *block2)
{
    if (block1 == nullptr)
    {
        cerr << "ERROR: BSTree::MoveBlock(future_parent, left_child) failed because block1 is nullptr" << endl;
        exit(1);
    }
    if (block2 == nullptr)
    {
        cerr << "ERROR: BSTree::MoveBlock(future_parent, left_child) failed because block2 is nullptr" << endl;
        exit(1);
    }

    Block *future_parent, *future_child;
    bool become_left_child;

    if (!WhoIsParent(block1, block2, future_parent, future_child, become_left_child))
    {
        return false;
    }

    if (DEBUG_MOVE)
    {
        cout << "future_parent: ";
        if (future_parent != nullptr)
        {
            cout << future_parent->getName() << endl;
        }
        else
        {
            cout << "nullptr" << endl;
        }

        cout << "future_child: ";
        if (future_child != nullptr)
        {
            cout << future_child->getName() << endl;
        }
        else
        {
            cout << "nullptr" << endl;
        }
    }

    // future_child should not be root
    CHILD_TYPE child_type = future_child->getChildType();
    if (child_type == ROOT)
    {
        cerr << "ERROR: BSTree::MoveBlock(future_parent, left_child) failed because future_child is root" << endl;
        // print the parent, left, right of the block
        cerr << "future_child.parent: ";
        if (future_child->getParent() != nullptr)
        {
            cerr << future_child->getParent()->getName() << endl;
        }
        else
        {
            cerr << "nullptr" << endl;
        }
        cerr << "future_child.left: ";
        if (future_child->getLeft() != nullptr)
        {
            cerr << future_child->getLeft()->getName() << endl;
        }
        else
        {
            cerr << "nullptr" << endl;
        }
        cerr << "future_child.right: ";
        if (future_child->getRight() != nullptr)
        {
            cerr << future_child->getRight()->getName() << endl;
        }
        else
        {
            cerr << "nullptr" << endl;
        }

        // print the parent, left, right of the future_child.parent if it is not nullptr
        if (future_child->getParent() != nullptr)
        {
            cerr << "future_child.parent.left: ";
            if (future_child->getParent()->getLeft() != nullptr)
            {
                cerr << future_child->getParent()->getLeft()->getName() << endl;
            }
            else
            {
                cerr << "nullptr" << endl;
            }
            cerr << "future_child.parent.right: ";
            if (future_child->getParent()->getRight() != nullptr)
            {
                cerr << future_child->getParent()->getRight()->getName() << endl;
            }
            else
            {
                cerr << "nullptr" << endl;
            }
        }

        cerr << "tree structure: " << endl;
        cerr << *this << endl;

        exit(1);
    }

    //  check if the future_parent has left child
    if (become_left_child && future_parent->getLeft() != nullptr)
    {
        cerr << "ERROR: BSTree::MoveBlock(future_parent, left_child) failed because future_parent already has left child" << endl;
        cerr << "future_parent: " << future_parent->getName() << " future_child: " << future_child->getName() << endl;
        cerr << "block1: " << block1->getName() << " block2: " << block2->getName() << endl;
        exit(1);
    }

    // check if the future_parent has right child
    if (!become_left_child && future_parent->getRight() != nullptr)
    {
        cerr << "ERROR: BSTree::MoveBlock(future_parent, left_child) failed because future_parent already has right child" << endl;
        cerr << "future_parent: " << future_parent->getName() << " future_child: " << future_child->getName() << endl;
        cerr << "block1: " << block1->getName() << " block2: " << block2->getName() << endl;
        exit(1);
    }

    if (become_left_child)
    {
        future_parent->setLeft(future_child);
    }
    else
    {
        future_parent->setRight(future_child);
    }

    // update the future_child's parent and handle the future_child's children
    // case1: future_child has no child
    if (future_child->getLeft() == nullptr && future_child->getRight() == nullptr)
    {
        // disconnect future_child from its parent
        Block *original_parent = future_child->getParent();
        if (child_type == LEFT_CHILD)
        {
            original_parent->setLeft(nullptr);
        }
        else if (child_type == RIGHT_CHILD)
        {
            original_parent->setRight(nullptr);
        }
        else
        {
            cerr << "ERROR: BSTree::MoveBlock(future_parent, left_child) failed because future_child is root" << endl;
            cerr << "case: become_left_child, future_child has no child" << endl;
            exit(1);
        }
        future_child->setParent(future_parent);
    }
    // case2: future_child has left child
    else if (future_child->getLeft() != nullptr)
    {
        // replace future_child with its left child
        Block *original_child_left = future_child->getLeft();
        Block *original_parent = future_child->getParent();

        original_child_left->setParent(original_parent);
        if (child_type == LEFT_CHILD)
        {
            original_parent->setLeft(original_child_left);
        }
        else if (child_type == RIGHT_CHILD)
        {
            original_parent->setRight(original_child_left);
        }
        else
        {
            cerr << "ERROR: BSTree::MoveBlock(future_parent, left_child) failed because future_child is root" << endl;
            cerr << "case: become_left_child, future_child has left child" << endl;
            exit(1);
        }

        future_child->setParent(future_parent);
    }
    // case3: future_child has right child
    else if (future_child->getRight() != nullptr)
    {
        Block *original_child_right = future_child->getRight();
        Block *original_parent = future_child->getParent();

        original_child_right->setParent(original_parent);
        if (child_type == LEFT_CHILD)
        {
            original_parent->setLeft(original_child_right);
        }
        else if (child_type == RIGHT_CHILD)
        {
            original_parent->setRight(original_child_right);
        }
        else
        {
            cerr << "ERROR: BSTree::MoveBlock(future_parent, left_child) failed because future_child is root" << endl;
            cerr << "case: become_left_child, future_child has right child" << endl;
            exit(1);
        }

        future_child->setParent(future_parent);
    }

    // update the contour
    updateContour();

    // Move success
    return true;
}

bool BSTree::WhoIsParent(const Block *block1, const Block *block2, Block *&parent, Block *&child, bool &become_left_child)
{
    if (block1 == nullptr || block2 == nullptr)
    {
        if (block1 == nullptr)
        {
            cerr << "ERROR: BSTree::WhoIsParent(block1, block2) failed because block1 is nullptr" << endl;
        }
        if (block2 == nullptr)
        {
            cerr << "ERROR: BSTree::WhoIsParent(block1, block2) failed because block2 is nullptr" << endl;
        }
        exit(1);
    }

    // check if the block1 and block2 are adjacent
    if (block1->getLeft() == block2 || block1->getRight() == block2)
    {
        return false;
    }
    if (block2->getLeft() == block1 || block2->getRight() == block1)
    {
        return false;
    }

    // special case when root is one of the block
    // Note: root cannot be the child, if a root is passed in, it will be the parent if it has < 2 children
    if (root == block1)
    {
        if (root->getLeft() != nullptr && root->getRight() != nullptr)
        {
            return false;
        }

        parent = const_cast<Block *>(block1);
        child = const_cast<Block *>(block2);

        // decide whether the child is the left or right child
        if (parent->getRight() != nullptr)
        {
            // parent has only left child left unassigned
            become_left_child = true;
        }
        // parent has only right child
        else if (parent->getLeft() != nullptr)
        {
            become_left_child = false;
        }
        else
        {
            // parent has both left and right child
            become_left_child = rand() % 2;
        }
        return true;
    }
    else if (root == block2)
    {
        if (root->getLeft() != nullptr && root->getRight() != nullptr)
        {
            return false;
        }

        parent = const_cast<Block *>(block2);
        child = const_cast<Block *>(block1);

        // decide whether the child is the left or right child
        if (parent->getRight() != nullptr)
        {
            // parent has only left child left unassigned
            become_left_child = true;
        }
        // parent has only right child
        else if (parent->getLeft() != nullptr)
        {
            become_left_child = false;
        }
        else
        {
            // parent has both left and right child
            become_left_child = rand() % 2;
        }
        return true;
    }

    bool block1_can_be_parent = (block1->getLeft() != block2 && block1->getRight() != block2);
    bool block2_can_be_parent = (block2->getLeft() != block1 && block2->getRight() != block1);
    if (block1_can_be_parent && block2_can_be_parent)
    {
        // decide who will be the parent
        bool block1_is_parent = rand() % 2;
        parent = const_cast<Block *>(block1_is_parent ? block1 : block2);
        child = const_cast<Block *>(block1_is_parent ? block2 : block1);
    }
    else if (block1_can_be_parent)
    {
        parent = const_cast<Block *>(block1);
        child = const_cast<Block *>(block2);
    }
    else if (block2_can_be_parent)
    {
        parent = const_cast<Block *>(block2);
        child = const_cast<Block *>(block1);
    }
    else
    {
        return false;
    }

    // We have decided the parent and child, and the parent should have at least one child
    // decide whether the child is the left or right child
    if (parent->getRight() != nullptr)
    {
        // parent has only left child left unassigned
        become_left_child = true;
    }
    // parent has only right child
    else if (parent->getLeft() != nullptr)
    {
        become_left_child = false;
    }
    else
    {
        // parent has both left and right child
        become_left_child = rand() % 2;
    }

    return true;
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

ostream &operator<<(ostream &os, const BSTree &tree)
{
    os << " ------- BSTree ------- " << endl;
    os << "root: ";
    if (tree.root != nullptr)
    {
        os << tree.root->getName() << endl;
    }
    else
    {
        os << "nullptr" << endl;
    }
    os << "BSTree: " << endl;
    os << "numBlocks: " << tree.numBlocks << endl;
    os << "numNets: " << tree.numNets << endl;
    os << "numTerms: " << tree.numTerms << endl;
    os << "BoundaryWidth: " << tree.BoundaryWidth << endl;
    os << "BoundaryHeight: " << tree.BoundaryHeight << endl;
    os << "BlockName2Ptr: " << endl;

    for (auto &pair : tree.BlockName2Ptr)
    {
        cout << "BlockName: " << pair.first << " BlockPtr->name: " << pair.second->getName() << endl;
    }
    os << "TermName2Loc: " << endl;
    for (auto &pair : tree.TermName2Loc)
    {
        os << "TermName: " << pair.first << " Loc: " << pair.second << endl;
    }
    os << "netlist: " << endl;
    for (const Net &net : tree.netlist)
    {
        os << "pins: " << endl;
        for (const Pin &pin : net.getPins())
        {
            os << "pin_type: " << (pin.pin_type == BLOCKPIN ? "block," : "terminal,") << " name: " << pin.name << endl;
        }
        os << "HPWL: " << net.getHPWL() << endl;
    }
    os << "block_names: " << endl;
    for (const string &name : tree.block_names)
    {
        os << name << " ";
    }
    os << endl;
    os << "term_names: " << endl;
    for (const string &name : tree.term_names)
    {
        os << name << " ";
    }
    os << endl;
    os << "contour_list: " << endl;
    for (const Contour &contour : tree.contour_list)
    {
        os << "x_left: " << contour.x_left << " x_right: " << contour.x_right << " y_top: " << contour.y_top << endl;
    }

    // print the tree structure
    // print the name of each block and the name of its parent, left, right, if not nullptr
    os << " Tree Structure: " << endl;
    for (auto &pair : tree.BlockName2Ptr)
    {
        if (pair.second == nullptr)
        {
            os << "Key: " << pair.first << " Value: nullptr ";

            os << endl;
            continue;
        }
        else
        {
            os << "Key: " << pair.first << ", ";
        }

        Block *curr = pair.second;
        os << "Block: " << curr->getName() << " Parent: ";
        if (curr->getParent() != nullptr)
        {
            os << curr->getParent()->getName() << ", ";
        }
        else
        {
            os << "nullptr ";
        }
        os << "Left: ";
        if (curr->getLeft() != nullptr)
        {
            os << curr->getLeft()->getName() << ", ";
        }
        else
        {
            os << "nullptr ";
        }
        os << "Right: ";
        if (curr->getRight() != nullptr)
        {
            os << curr->getRight()->getName() << " ";
        }
        else
        {
            os << "nullptr ";
        }

        if (pair.second->getParent() == nullptr)
        {
            os << "<-- root";
        }
        os << endl;
    }
    os << " ---------------------- " << endl;

    return os;
}

void BSTree::writePlotFile(ostream &out, int OutlineWidth, int OutlineHeight) const
{
    // Print the BL and width, height of each block
    out << "NumBlocks: " << numBlocks << endl;
    for (auto &pair : BlockName2Ptr)
    {
        Block *curr = pair.second;
        // order: name, x, y, width, height, parent, left, right
        out << curr->getName() << " ";
        out << curr->getBL().x << " " << curr->getBL().y << " " << curr->getWidth() << " " << curr->getHeight();
        out << " " << (curr->getParent() == nullptr ? "nullptr" : curr->getParent()->getName());
        out << " " << (curr->getLeft() == nullptr ? "nullptr" : curr->getLeft()->getName());
        out << " " << (curr->getRight() == nullptr ? "nullptr" : curr->getRight()->getName());
        out << endl;
    }

    // Print the contour list
    out << "NumContours: " << contour_list.size() << endl;
    for (list<Contour>::const_iterator it = contour_list.begin(); it != contour_list.end(); it++)
    {
        out << it->x_left << " " << it->x_right << " " << it->y_top << endl;
    }

    // Print the outline of the bounding box
    out << "Outline: " << OutlineWidth << " " << OutlineHeight << " " << BoundaryWidth << " " << BoundaryHeight << endl;
}