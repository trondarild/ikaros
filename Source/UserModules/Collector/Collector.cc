//
//	Collector.cc		This file is a part of the IKAROS project
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

#include "Collector.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;

const char *c_x_dir = "x";
const char *c_y_dir = "y";

void
Collector::SetSizes()
{
    int sx = GetInputSizeX("INPUT");
    int sy = GetInputSizeY("INPUT");
    int size = GetIntValue("size");
    const char * direction = GetValue("collection_direction");
    
    if(!equal_strings(direction, c_x_dir) && !equal_strings(direction, c_y_dir))
        Notify(msg_fatal_error, "Collector: Illegal value for collection_direction. Should be 'x' or 'y'.");

    int out_x, out_y;
    if(equal_strings(direction, c_x_dir))
    {
        out_x = sx*size;
        out_y = sy;
    } else
    {
        out_x = sx;
        out_y = sy*size;
    }
    SetOutputSize("OUTPUT", out_x, out_y);
    
}

void
Collector::Init()
{
    collection_direction = GetValue("collection_direction");
    Bind(size, "size");
	Bind(debugmode, "debug");    

    input_matrix = GetInputMatrix("INPUT");
    input_matrix_size_x = GetInputSizeX("INPUT");
    input_matrix_size_y = GetInputSizeY("INPUT");

    output_matrix = GetOutputMatrix("OUTPUT");
    output_matrix_size_x = GetOutputSizeX("OUTPUT");
    output_matrix_size_y = GetOutputSizeY("OUTPUT");

    internal_matrix = create_matrix(
        output_matrix_size_x,
        output_matrix_size_y);
}



Collector::~Collector()
{
    // Destroy data structures that you allocated in Init.
    destroy_matrix(internal_matrix);
}



void
Collector::Tick()
{
    // default is collect in x dir
    int internal_row = 0;
    int internal_col = current_pos;
    int increment = input_matrix_size_x;
    int limit = output_matrix_size_x;
    
    if (equal_strings(collection_direction, c_y_dir))
    {
        internal_row = current_pos;
        internal_col = 0;
        increment = input_matrix_size_y;
        limit = output_matrix_size_y;
    }
        
    set_submatrix(
        internal_matrix[0],
        output_matrix_size_x,
        input_matrix[0],
        input_matrix_size_y,
        input_matrix_size_x,
        internal_row,
        internal_col);
    current_pos += increment;
    if(current_pos >= limit)
    {
        copy_matrix(
            output_matrix,
            internal_matrix,
            output_matrix_size_x,
            output_matrix_size_y);
        current_pos = 0;
    }
        
    if(debugmode)
	{
        print_matrix(
             "Collector::internal_matrix",
             internal_matrix,
             output_matrix_size_x,
             output_matrix_size_y, 4);
		// print out debug info
        print_matrix(
            "Collector::output",
            output_matrix,
            output_matrix_size_x,
            output_matrix_size_y, 4);
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("Collector", &Collector::Create, "Source/UserModules/Collector/");


