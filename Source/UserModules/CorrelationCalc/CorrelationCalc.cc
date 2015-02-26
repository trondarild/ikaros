//
//	CorrelationCalc.cc		This file is a part of the IKAROS project
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

#include "CorrelationCalc.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;

void
CorrelationCalc::SetSizes()
{
    int sy  = GetInputSizeY("INPUT");
    
    // result is a square matrix, with dimensions
    // equal to number of rows
    SetOutputSize("OUTPUT", sy, sy);
}

void
CorrelationCalc::Init()
{
    
	Bind(debugmode, "debug");    

    input_matrix = GetInputMatrix("INPUT");
    input_matrix_size_x = GetInputSizeX("INPUT");
    input_matrix_size_y = GetInputSizeY("INPUT");
    scalar = GetInputArray("SCALE");

    output_matrix = GetOutputMatrix("OUTPUT");
    output_matrix_size = GetOutputSizeX("OUTPUT");

    // to hold the transpose
    internal_matrix = create_matrix(input_matrix_size_y, input_matrix_size_x);
    
}



CorrelationCalc::~CorrelationCalc()
{
    // Destroy data structures that you allocated in Init.
    destroy_matrix(internal_matrix);
}



void
CorrelationCalc::Tick()
{

	transpose(
        internal_matrix, 
        input_matrix, 
        input_matrix_size_y, 
        input_matrix_size_x);
    
    multiply(
        output_matrix,
        input_matrix,
        internal_matrix,
        output_matrix_size,
        output_matrix_size,
        input_matrix_size_x);
    
    multiply(
        output_matrix,
        1.f/scalar[0],
        output_matrix_size,
        output_matrix_size);

    if(debugmode)
	{
		// print out debug info
        print_matrix(
            "CorrelationCalc::output",
            output_matrix,
            output_matrix_size,
            output_matrix_size, 4);
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("CorrelationCalc", &CorrelationCalc::Create, "Source/UserModules/CorrelationCalc/");


