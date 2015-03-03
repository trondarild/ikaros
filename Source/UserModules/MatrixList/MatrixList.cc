//
//	MatrixList.cc		This file is a part of the IKAROS project
//
//    Copyright (C) 2012 <Author Name>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    See http://www.ikaros-project.org/ for more information.
//
//  This example is intended as a starting point for writing new Ikaros modules
//  The example includes most of the calls that you may want to use in a module.
//  If you prefer to start with a clean example, use he module MinimalModule instead.
//

#include "MatrixList.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
MatrixList::Init()
{
    Bind(size, "size");
    Bind(store_on_change, "store_on_change");
    Bind(default_identity, "default_identity");
	Bind(debugmode, "debug");    

    input_matrix = GetInputMatrix("INPUT");
    input_matrix_size_x = GetInputSizeX("INPUT");
    input_matrix_size_y = GetInputSizeY("INPUT");
    if(input_matrix_size_y!=input_matrix_size_x)
        Notify(msg_warning, "MatrixList: Warning - input matrix not square.");
    select = GetInputArray("SELECT");
    output_matrix = GetOutputMatrix("OUTPUT");

    internal_matrix = create_matrix(
        input_matrix_size_x, 
        input_matrix_size_y* 
            (default_identity ? ++size :size));
    prev_input = create_matrix(
        input_matrix_size_x,
        input_matrix_size_x);
    
    max_pos = size-1;
    min_pos = 0;
    // add identity matrix at index 0 ?
    if(default_identity)
    {
        min_pos = 1;
        current_pos = min_pos;
        float **identity = create_matrix(
            input_matrix_size_x,
            input_matrix_size_x);
        eye(identity, input_matrix_size_x);
        set_submatrix(
            internal_matrix[0],
            input_matrix_size_x,
            identity[0],
            input_matrix_size_x,
            input_matrix_size_x,
            0, 0);
        destroy_matrix(identity);
    }
}



MatrixList::~MatrixList()
{
    // Destroy data structures that you allocated in Init.
    destroy_matrix(internal_matrix);
    destroy_matrix(prev_input);
}



void
MatrixList::Tick()
{
    // add new?
    if((store_on_change && !equal(input_matrix, prev_input, input_matrix_size_x, input_matrix_size_x, 0.0001))
        || !store_on_change)
    {
        // cyclically overwrite
        // TODO make this a flag
        current_pos = current_pos <= max_pos ? current_pos : min_pos; 
        set_submatrix(
            internal_matrix[0],
            input_matrix_size_x,
            input_matrix[0],
            input_matrix_size_x,
            input_matrix_size_x,
            input_matrix_size_y * current_pos,  // start row
            0);                                 // start col
        current_pos++;
    }
    // output
    // TODO debug this:
    int ix = (int)select[0];
    if(ix < size && ix > -1)
        copy_matrix(
            output_matrix,
            internal_matrix + ix*input_matrix_size_y,
            input_matrix_size_x,
            input_matrix_size_x);
    else
        Notify(msg_warning, "Warning: MatrixList::Tick: SELECT is invalid");

	if(debugmode)
	{
		// print out debug info
        print_matrix(
            "MatrixList::output",
            output_matrix,
            input_matrix_size_x,
            input_matrix_size_x);
        print_matrix(
            "MatrixList::internal_matrix",
            internal_matrix,
            input_matrix_size_x,
            input_matrix_size_y*size);
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("MatrixList", &MatrixList::Create, "Source/UserModules/MatrixList/");


