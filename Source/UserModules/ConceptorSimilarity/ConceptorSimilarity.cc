//
//	ConceptorSimilarity.cc		This file is a part of the IKAROS project
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

#include "ConceptorSimilarity.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;

void
ConceptorSimilarity::SetSizes()
{
    int sx_1 = GetInputSizeX("INPUT1");
    int sy_1 = GetInputSizeY("INPUT1");

    int sx_2 = GetInputSizeX("INPUT2");
    int sy_2 = GetInputSizeY("INPUT2");
    if (sx_1 != sx_2 || sx_1 != sy_1 || sx_2!=sy_2)
        Notify(msg_fatal_error,
               "ConceptorSimilarity: input dimensions do not match.\
               Check that they are square and both of same dimension.");
    SetOutputSize("OUTPUT", 1);
    int tst = GetOutputSize("OUTPUT");
    printf("%i", tst);
}

void
ConceptorSimilarity::Init()
{
    Bind(debugmode, "debug");

    input_matrix_1 = GetInputMatrix("INPUT1");
    input_matrix_2 = GetInputMatrix("INPUT2");
    input_matrix_size_x = GetInputSizeX("INPUT1");
    input_matrix_size_y = GetInputSizeY("INPUT1");
    
    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");

    int inp_size = input_matrix_size_x;
    internal_matrix_a = create_matrix(inp_size, inp_size);
    internal_matrix_b = create_matrix(inp_size, inp_size);
    internal_matrix_c = create_matrix(inp_size, inp_size);
    internal_matrix_d = create_matrix(inp_size, inp_size);
    u_a = create_matrix(inp_size, inp_size);
    s_a = create_array(inp_size);
    v_a = create_matrix(inp_size, inp_size);
    u_b = create_matrix(inp_size, inp_size);
    s_b = create_array(inp_size);
    v_b = create_matrix(inp_size, inp_size);
}



ConceptorSimilarity::~ConceptorSimilarity()
{
    // Destroy data structures that you allocated in Init.
    destroy_matrix(internal_matrix_a);
    destroy_matrix(internal_matrix_b);
    destroy_matrix(internal_matrix_c);
    destroy_matrix(internal_matrix_d);
    destroy_matrix(u_a);
    destroy_array(s_a);
    destroy_matrix(v_a);
    destroy_matrix(u_b);
    destroy_array(s_b);
    destroy_matrix(v_b);
}



void
ConceptorSimilarity::Tick()
{
    int size = input_matrix_size_x;
    reset_matrix(internal_matrix_a, size, size);
    reset_matrix(internal_matrix_b, size, size);
    reset_matrix(internal_matrix_c, size, size);
    reset_matrix(internal_matrix_d, size, size);
    // do singular value decomposition of inputs
    svd(u_a, s_a, v_a, input_matrix_1, size, size);
    svd(u_b, s_b, v_b, input_matrix_2, size, size);
    float norm_a = norm(input_matrix_1, size, size);
    float norm_b = norm(input_matrix_2, size, size);
    
    diag(internal_matrix_a, sqrt(s_a, size), size);
    //print_matrix("internal_matrix_a", internal_matrix_a, size, size);
    diag(internal_matrix_b, sqrt(s_b, size), size);
    //print_matrix("internal_matrix_b", internal_matrix_b, size, size);
    
    multiply(internal_matrix_c, internal_matrix_a, transpose(u_a, size), size, size, size);
    //print_matrix("internal_matrix_c", internal_matrix_c, size, size);
    multiply(internal_matrix_d, u_b, internal_matrix_b, size, size, size);
    //print_matrix("internal_matrix_d", internal_matrix_d, size, size);
    multiply(internal_matrix_a, internal_matrix_c, internal_matrix_d, size, size, size);
    //print_matrix("internal_matrix_a", internal_matrix_a, size, size);
    float norm_ab = norm(internal_matrix_a, size, size);
    
    output_array[0] = pow(norm_ab,2.0)/(norm_a*norm_b);
    
    
	if(debugmode)
	{
		// print out debug info
        printf("ConceptorSimilarity: output = %f", output_array[0]);
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("ConceptorSimilarity",
                      &ConceptorSimilarity::Create,
                      "Source/UserModules/ConceptorSimilarity/");


