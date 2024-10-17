//===========================================================================
//
// NYCU MSEDA Lab       
//
// File Name   : bsTree.cpp
// Function    : B*-Tree Class Function Def. ( 2022 Fall PDA Lab02 )
// Author      : Tracelementx ( Wei-Liang Chen )
// Version     : v2.0
//
//===========================================================================


#include "bsTree.h"

bsTree::bsTree()
    : root_ptr( 0 ), width( 0 ), height( 0 ), area( 0 ), tot_hpwl( 0 ), cost( 0 )
{

}

//////////////////////////////////////////////////

void bsTree::insertBlk( const string &name, const int &w, const int &h )
{
    insertBlkH( &root_ptr, &root_ptr, name, w, h );
}

void bsTree::insertBlkH( block **parent, block **curr, const string &name, const int &w, const int &h )
{
    if( *curr == 0 )
    {
        block *temp = *parent;
        *curr = new block( name, w, h );
        blk[name] = *curr;
        
        if( temp == 0 )
            ( *curr )->parent_ptr = 0;
        else
            ( *curr )->parent_ptr = *parent;
    }
    else
        insertBlkH( curr, &( ( *curr )->left_ptr ), name, w, h );
}

//////////////////////////////////////////////////

void bsTree::copyTree( const bsTree &best )
{
    root_ptr = copyTreeNode( best.root_ptr, 0 );
    setTour( root_ptr, true );
    pins = best.pins;
    netlist = best.netlist;
}

block * bsTree::copyTreeNode( block *curr_ptr, block *p_ptr )
{
    block *new_ptr;

    if( curr_ptr != 0 )
    {
        new_ptr = new block( curr_ptr->name, curr_ptr->width, curr_ptr->height );
        blk[curr_ptr->name] = new_ptr;
        new_ptr->parent_ptr = p_ptr;
        new_ptr->left_ptr = copyTreeNode( curr_ptr->left_ptr, new_ptr );
        new_ptr->right_ptr = copyTreeNode( curr_ptr->right_ptr, new_ptr );
        return new_ptr;
    }
    else
        return 0;
}

//////////////////////////////////////////////////

void bsTree::perturb( const int &opt, const int &pick_idx0, const int &pick_idx1, map< int, string > idx_name, bool &move_check )
{
    block *target0;
    block *target1;

    switch( opt )
    {
        //rotate
        case 0:
        {   
            target0 = blk[idx_name[pick_idx0]];
            rotaBlk( target0 );
            break;
        }
        //move
        case 1:
        { 
            target0 = blk[idx_name[pick_idx0]];
            target1 = blk[idx_name[pick_idx1]];

            if ( ( target0->left_ptr != 0 && target0->right_ptr != 0 ) || 
                    ( target1->left_ptr != 0 && target1->right_ptr != 0 ) )
            {
                move_check = false;
                break;
            }
                
            moveBlk( target0, target1 );
            break;
        }
        //swap
        case 2:
        {
            target0 = blk[idx_name[pick_idx0]];
            target1 = blk[idx_name[pick_idx1]];
            swapBlk( target0, target1 );
            break;
        }
        default: break;
    }
}

block * bsTree::find( const string &name )
{
    return blk[name];
}

void bsTree::rotaBlk( block *target0 )
{
    target0->setSize( target0->height, target0->width );
}

void bsTree::moveBlk( block *target0, block *target1 )
{
    if( target0 != root_ptr )
    {
        if( target0->left_ptr != 0 )
        {
            target0->left_ptr->parent_ptr = target0->parent_ptr;

            if( target0 == target0->parent_ptr->left_ptr ) 
                target0->parent_ptr->left_ptr = target0->left_ptr;
            else if( target0 == target0->parent_ptr->right_ptr )
                target0->parent_ptr->right_ptr = target0->left_ptr;
        }
        else if( target0->right_ptr != 0 )
        {
            target0->right_ptr->parent_ptr = target0->parent_ptr;

            if( target0 == target0->parent_ptr->left_ptr ) 
                target0->parent_ptr->left_ptr = target0->right_ptr;
            else if( target0 == target0->parent_ptr->right_ptr )
                target0->parent_ptr->right_ptr = target0->right_ptr;
        }
        else if( target0->left_ptr == 0 && target0->right_ptr == 0 )
        {
            if( target0 == target0->parent_ptr->left_ptr )
                    target0->parent_ptr->left_ptr =  0;
            else
                target0->parent_ptr->right_ptr =  0;
        }   
    }
    else if( target0 == root_ptr )
    {
        if( target0->left_ptr != 0 )
        {
            root_ptr = target0->left_ptr;
            target0->left_ptr->parent_ptr = 0;
        }
        else if( target0->right_ptr != 0 )
        {
            root_ptr = target0->right_ptr;
            target0->right_ptr->parent_ptr = 0;
        }
    }
    
    target0->left_ptr = 0;
    target0->right_ptr = 0;
    target0->parent_ptr = target1;

    if( target1->left_ptr == 0 )
        target1->left_ptr = target0;
    else if( target1->right_ptr == 0 )
        target1->right_ptr = target0;    
}

void bsTree::swapBlk( block *target0, block *target1 )
{
    block swap_temp( "swap_temp", 0, 0 );

    if( target0->parent_ptr == target1 )
    {
        block *temp;
        temp = target0;
        target0 = target1;
        target1 = temp;
    }
    if( target1->parent_ptr == target0 )
    {
        bool is_left = false;
        if( target1 == target0->left_ptr )
            is_left = true;

        if( is_left == false )
            swap_temp.left_ptr = target0->left_ptr;
        else if( is_left == true )
            swap_temp.right_ptr = target0->right_ptr;

        swap_temp.parent_ptr = target0->parent_ptr;

        if( target0->parent_ptr != 0 )
        {
            if( target0 == target0->parent_ptr->left_ptr )
                target0->parent_ptr->left_ptr = &swap_temp;
            else if( target0 == target0->parent_ptr->right_ptr )
                target0->parent_ptr->right_ptr = &swap_temp;
        }
    
        if( is_left == false && swap_temp.left_ptr != 0 )
            ( swap_temp.left_ptr )->parent_ptr = &swap_temp;
        else if( is_left == true && swap_temp.right_ptr != 0 )
            ( swap_temp.right_ptr )->parent_ptr = &swap_temp;

        /////////////////////////

        target0->left_ptr = target1->left_ptr;
        target0->right_ptr = target1->right_ptr;
        target0->parent_ptr = target1;

        if( target0->left_ptr != 0 )
            target0->left_ptr->parent_ptr = target0;
        
        if( target0->right_ptr != 0 )
            target0->right_ptr->parent_ptr = target0;

        /////////////////////////

        if( is_left == false )
        {
            target1->left_ptr = swap_temp.left_ptr;
            target1->right_ptr = target0;
        }
        else if( is_left == true )
        {
            target1->left_ptr = target0;
            target1->right_ptr = swap_temp.right_ptr;
        }
        target1->parent_ptr = swap_temp.parent_ptr;

        if( swap_temp.parent_ptr != 0 )
        {
            if( &swap_temp == swap_temp.parent_ptr->left_ptr )
                swap_temp.parent_ptr->left_ptr = target1;
            else if( &swap_temp == swap_temp.parent_ptr->right_ptr )
                swap_temp.parent_ptr->right_ptr = target1;
        }
        else
            root_ptr = target1;
    
        if( is_left == false && target1->left_ptr != 0 )
            target1->left_ptr->parent_ptr = target1;
        
        if( is_left == true &&  target1->right_ptr != 0 )
            target1->right_ptr->parent_ptr = target1;
    }   
    else
    {
        swap_temp.setBlk( target0 );
        target0->setBlk( target1 );
        target1->setBlk( &swap_temp );
        
        if( target0 == root_ptr )
            root_ptr = target1;
        else if( target1 == root_ptr )
            root_ptr = target0;
    }
}

//////////////////////////////////////////////////

void bsTree::setTour( block *curr_ptr, bool is_left )
{
    coordinate temp_coo;

    if( curr_ptr == root_ptr ) 
    {
        contour.clear();
        width = 0;
        height = 0;
        curr_ptr->coo_xlb = 0;
        curr_ptr->coo_ylb = 0;
        while( contour.size() < curr_ptr->width )
        {
            contour.push_back( 0 );
        }
    } 
    else 
    {
        if( is_left ) 
            curr_ptr->coo_xlb = curr_ptr->parent_ptr->coo_xrt;
        else if( !is_left )
            curr_ptr->coo_xlb = curr_ptr->parent_ptr->coo_xlb;

        curr_ptr->coo_ylb = 0;

        for( int i = curr_ptr->coo_xlb; i < ( curr_ptr->coo_xlb + curr_ptr->width ); i++ ) 
        {
            while( contour.size() < ( i + 1 ) )
            {
                contour.push_back( 0 );
            }
            if( curr_ptr->coo_ylb < contour[i] )
                curr_ptr->coo_ylb = contour[i];
        }
    }

    curr_ptr->coo_xrt = curr_ptr->coo_xlb + curr_ptr->width;
    curr_ptr->coo_yrt = curr_ptr->coo_ylb + curr_ptr->height;
    temp_coo.x = ( curr_ptr->coo_xlb + curr_ptr->coo_xrt ) / 2.0;
    temp_coo.y = ( curr_ptr->coo_ylb + curr_ptr->coo_yrt ) / 2.0;
    pins[curr_ptr->name] = temp_coo;

    //update bounding box
    if( curr_ptr->coo_xrt > width )
        width = curr_ptr->coo_xrt;

    if( curr_ptr->coo_yrt > height )
        height = curr_ptr->coo_yrt;

    //update contour
    for( int i = curr_ptr->coo_xlb; i < curr_ptr->coo_xrt; i++ )
    {
        if( curr_ptr->coo_yrt > contour[i] ) 
            contour[i] = curr_ptr->coo_yrt;
    }
        
    //go left_child
    if( curr_ptr->left_ptr != 0 )
        setTour( curr_ptr->left_ptr, true );

    //go right_child
    if( curr_ptr->right_ptr != 0 )
        setTour( curr_ptr->right_ptr, false );
}

void bsTree::setCost( double alpha )
{
    area = width * height;
    tot_hpwl = 0;
    
    vector< nets * >::iterator vec_it;
    for( vec_it = netlist.begin(); vec_it != netlist.end(); vec_it++ )
    {
        ( *vec_it )->setHPWL( this );
        tot_hpwl = tot_hpwl + ( *vec_it )->getHPWL();
    }

    cost = int( alpha * area + ( 1.0 - alpha ) * tot_hpwl );
}

void bsTree::setCostOut( int out_w, int out_h )
{
    cost_out = abs( width - out_w ) + abs( height - out_h );
}

void bsTree::setPins( string name, double x, double y )
{
    coordinate temp_coo;
    temp_coo.x = x;
    temp_coo.y = y;
    pins[name] = temp_coo;
}

void bsTree::setNetlist( nets *nt )
{
    netlist.push_back( nt );
}

//////////////////////////////////////////////////

void bsTree::print_tree( block *curr_ptr, ofstream &output )
{
    if( curr_ptr != 0 )
    {
        output << left << setw(8) << curr_ptr->name 
            << left << setw(8) << curr_ptr->coo_xlb
            << left << setw(8) << curr_ptr->coo_ylb
            << left << setw(8) << curr_ptr->coo_xrt
            << left << setw(8) << curr_ptr->coo_yrt << endl;

        print_tree( curr_ptr->left_ptr, output );
        print_tree( curr_ptr->right_ptr, output );
    }
}

