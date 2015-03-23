//
//	ReservoirNetwork.cc		This file is a part of the IKAROS project
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

#include "ReservoirNetwork.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;

void
ReservoirNetwork::SetSizes()
{
    int dim = GetIntValue("dimension");
    SetOutputSize("OUTPUT", 1, dim);
}

void
ReservoirNetwork::Init()
{
    Bind(dimension, "dimension");
    Bind(bias_scaling, "bias_scaling");
	Bind(debugmode, "debug");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    weight_matrix = GetInputMatrix("W");
    weight_matrix_size_x = GetInputSizeX("W");
    weight_matrix_size_y = GetInputSizeY("W");
    weight_in_matrix = GetInputMatrix("W_IN");
    weight_in_matrix_size = GetInputSizeY("W_IN"); // should checked in SetSizes
    
    bias_matrix = GetInputMatrix("BIAS");
    bias_matrix_size = GetInputSizeY("BIAS"); // should be checked in SetSizes
    

    output_matrix = GetOutputMatrix("OUTPUT");
    output_matrix_size_y = GetOutputSizeY("OUTPUT");
    output_matrix_size_x = GetOutputSizeX("OUTPUT");

    network_state = create_matrix(output_matrix_size_x, dimension); // network is a one col matrix
    //bias = create_array(dimension);
    //random(bias, 0.f, 1.f, dimension);


    matrix_temp_a = create_matrix(output_matrix_size_x, dimension);
    matrix_temp_b = create_matrix(output_matrix_size_x, dimension);
}



ReservoirNetwork::~ReservoirNetwork()
{
    // Destroy data structures that you allocated in Init.
    destroy_matrix(network_state);
    //destroy_array(bias);
    destroy_matrix(matrix_temp_a);
    destroy_matrix(matrix_temp_b);
}



void
ReservoirNetwork::Tick()
{
    // printf("input value: %f\n", input_array[0]);
	// update the network
    // x = tanh(W*x + W_in*input + bias)
    multiply(
        matrix_temp_a,
        weight_matrix,
        network_state,
        output_matrix_size_x,
        dimension,
        dimension);
    // print_matrix("W*x", matrix_temp_a, output_matrix_size_x, dimension, 4);
    multiply(
        matrix_temp_b,
        weight_in_matrix,
        input_array[0],
        output_matrix_size_x,
        dimension);
    // print_matrix("W_in*input", matrix_temp_b, output_matrix_size_x, dimension, 4);
    add(
        matrix_temp_a,
        matrix_temp_a,
        matrix_temp_b,
        output_matrix_size_x,
        dimension);
    multiply(
        matrix_temp_b,
        bias_matrix,
        bias_scaling,
        output_matrix_size_x,
        dimension);
    // print_matrix("bias", matrix_temp_b, output_matrix_size_x, dimension, 4);
    add(
        network_state,
        matrix_temp_a,
        matrix_temp_b,
        output_matrix_size_x,
        dimension);
    tanh(
        network_state,
        output_matrix_size_x,
        dimension);
    
    copy_matrix(output_matrix, network_state, output_matrix_size_x, dimension);
    if(debugmode)
	{
		print_matrix(
            "ReservoirNetwork::network_state", 
            network_state,
            output_matrix_size_x,
            dimension, 4);
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("ReservoirNetwork", &ReservoirNetwork::Create, "Source/UserModules/ReservoirNetwork/");


