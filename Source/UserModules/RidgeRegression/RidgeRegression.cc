//
//	RidgeRegression.cc		This file is a part of the IKAROS project
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

#include "RidgeRegression.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;

void
RidgeRegression::SetSizes()
{
    // get sizes of inputs
    //int in_x = GetInputSizeX("INPUT");
    int in_y = GetInputSizeY("INPUT");
    //int tgt_x = GetInputSizeX("TARGET");
    int tgt_y = GetInputSizeY("TARGET");

    // get output dimensions
    int out_x = GetIntValue("output_x");
    int out_y = GetIntValue("output_y");

    // check for correctness
    if(in_y % out_y != 0)
        Notify(msg_fatal_error, "Submatrix: Illegal range for input y compared to output y - input should be integer multiple of output.");

    if(tgt_y % out_y != 0)
        Notify(msg_fatal_error, "Submatrix: Illegal range for target y compared to output y - target should be integer multiple of output.");
    // y= columns in input, x = cols in target
    SetOutputSize("OUTPUT", out_x, out_y);
}

void
RidgeRegression::Init()
{
    
	Bind(debugmode, "debug");    

    input_matrix = GetInputMatrix("INPUT");
    input_matrix_size_x = GetInputSizeX("INPUT");
    input_matrix_size_y = GetInputSizeY("INPUT");
    target_matrix = GetInputMatrix("TARGET");
    target_matrix_size_x = GetInputSizeX("TARGET");
    target_matrix_size_y = GetInputSizeY("TARGET");
    alpha = GetInputArray("ALPHA");

    output_matrix = GetOutputMatrix("OUTPUT");
    output_matrix_size_x = GetOutputSizeX("OUTPUT");
    output_matrix_size_y = GetOutputSizeY("OUTPUT");


    internal_matrix_a = create_matrix(
        input_matrix_size_y,
        input_matrix_size_y);
    internal_matrix_b = create_matrix(
        input_matrix_size_y,
        input_matrix_size_y);
    input_transpose = create_matrix(
        input_matrix_size_y,
        input_matrix_size_x);
    identity = create_matrix(
        input_matrix_size_y,
        input_matrix_size_y);
    pre_output_transposed = create_matrix(
        output_matrix_size_y,
        output_matrix_size_x);
    target_transpose = create_matrix(
        target_matrix_size_y,
        target_matrix_size_x);
    eye(identity, input_matrix_size_y);

}



RidgeRegression::~RidgeRegression()
{
    // Destroy data structures that you allocated in Init.
    destroy_matrix(internal_matrix_a);
    destroy_matrix(internal_matrix_b);
    destroy_matrix(input_transpose);
    destroy_matrix(pre_output_transposed);
    destroy_matrix(target_transpose);
    destroy_matrix(identity);
}



void
RidgeRegression::Tick()
{
   // output the input matrices so can compare with matlab
   //print_matrix("input_matrix", input_matrix, input_matrix_size_x, input_matrix_size_y, 4 | MATLAB);
   //print_matrix("target_matrix", target_matrix, target_matrix_size_x, target_matrix_size_y, 4 | MATLAB);
    // do ridge regression procedure
    /*

    (inv(
        input * input' +
        alpha * eye(input_size_y)
        )
    * input * target')'

    */
   transpose(
        input_transpose,
        input_matrix,
        input_matrix_size_y,
        input_matrix_size_x);
//    printf("RidgeRegression::input transpose ok\n");
//    print_matrix(
//                "RidgeRegression::input_transpose",
//                input_transpose,
//                input_matrix_size_y,
//                input_matrix_size_x);
    multiply(
                internal_matrix_a,
                input_matrix,
                input_transpose,
                input_matrix_size_y,
                input_matrix_size_y,
                input_matrix_size_x);
//    printf("RidgeRegression::correlation multiplication ok\n");
//    print_matrix(
//                "RidgeRegression::internal_matrix_a",
//                internal_matrix_a,
//                input_matrix_size_y,
//                input_matrix_size_y);
    multiply(
                internal_matrix_b,
                identity,
                alpha[0],
                input_matrix_size_y,
                input_matrix_size_y);
//    printf("RidgeRegression::alpha multiplication ok\n");
//    print_matrix(
//                "RidgeRegression::internal_matrix_b",
//                internal_matrix_b,
//                input_matrix_size_y,
//                input_matrix_size_y, 4);

   add(
        internal_matrix_a, 
        internal_matrix_a, 
        internal_matrix_b, 
        input_matrix_size_y,
        input_matrix_size_y);
//    printf("RidgeRegression::addition ok\n");
//    print_matrix(
//                "RidgeRegression::internal_matrix_a",
//                internal_matrix_a,
//                input_matrix_size_y,
//                input_matrix_size_y);

    bool okk = inv(
        internal_matrix_b, 
        internal_matrix_a,
        input_matrix_size_y);
//    printf("RidgeRegression::inversion ok: %i\n", (int)okk);
//    print_matrix(
//                "RidgeRegression::internal_matrix_b",
//                internal_matrix_b,
//                input_matrix_size_y,
//                input_matrix_size_y);
    
    transpose(
        target_transpose,
        target_matrix,
        target_matrix_size_y,
        target_matrix_size_x);
    multiply(
        pre_output_transposed,
        input_matrix,
        target_transpose,
        output_matrix_size_y,
        output_matrix_size_x,
        input_matrix_size_x);
//    printf("RidgeRegression::multiplication target ok\n");
//    print_matrix(
//                "RidgeRegression::output_matrix",
//                pre_output_transposed,
//                output_matrix_size_y,
//                output_matrix_size_x);

    multiply(
        pre_output_transposed,
        internal_matrix_b,
        pre_output_transposed,
        output_matrix_size_y,
        output_matrix_size_x,
        input_matrix_size_y);
//    printf("RidgeRegression::multiplication output ok\n");
    transpose(
             output_matrix,
             pre_output_transposed,
             output_matrix_size_x,
             output_matrix_size_y);

//    print_matrix(
//                "RidgeRegression::output_matrix",
//                output_matrix,
//                output_matrix_size_x,
//                output_matrix_size_y, 4);

    
	if(debugmode)
	{
		// print out debug info
        print_matrix(
            "RidgeRegression::output",
            output_matrix,
            output_matrix_size_x,
            output_matrix_size_y, 5);
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("RidgeRegression", &RidgeRegression::Create, "Source/UserModules/RidgeRegression/");


