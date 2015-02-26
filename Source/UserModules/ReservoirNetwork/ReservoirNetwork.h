//
//	ReservoirNetwork.h		This file is a part of the IKAROS project
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

#ifndef ReservoirNetwork_
#define ReservoirNetwork_

#include "IKAROS.h"

class ReservoirNetwork: public Module
{
public:
    static Module * Create(Parameter * p) { return new ReservoirNetwork(p); }

    ReservoirNetwork(Parameter * p) : Module(p) {}
    virtual ~ReservoirNetwork();
    void SetSizes();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array;
    int         input_array_size;
    float **    weight_matrix;
    int         weight_matrix_size_x;
    int         weight_matrix_size_y;
    float **    weight_in_matrix;
    int         weight_in_matrix_size;
    float **    bias_matrix;
    int         bias_matrix_size;

    float **    output_matrix;
    int         output_matrix_size_x;
    int         output_matrix_size_y;

    // internal data storage
    float **   network_state;  // the network state vector
    // float *    bias; // previous state
    float **   matrix_temp_a;
    float **   matrix_temp_b;
    // parameter values

    int         dimension;
    float       bias_scaling;
	bool       	debugmode;
};

#endif
