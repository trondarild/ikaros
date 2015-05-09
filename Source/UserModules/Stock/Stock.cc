//
//	Stock.cc		This file is a part of the IKAROS project
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

#include "Stock.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
Stock::Init()
{
    Bind(initial_value, "initial_value");
	Bind(debugmode, "debug");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    out_flow_rate = GetInputArray("OUT_FLOW_RATE");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");
    current_level = GetOutputArray("CURRENT_LEVEL");

    internal_array = create_array(input_array_size);
    set_array(internal_array, initial_value, input_array_size);
}



Stock::~Stock()
{
    // Destroy data structures that you allocated in Init.
    destroy_array(internal_array);
}



void
Stock::Tick()
{
    for (int i = 0; i < input_array_size; ++i)
    {
        // add input flow
        internal_array[i] += input_array[i];
        // subtract output flow, assume same out flow for all for now

        float tmp = internal_array[i] - out_flow_rate[0];
        if(internal_array[i] == 0)
        {
            output_array[i] = 0;
        }
        else if (tmp<0 && internal_array[i]>0)
        {
            output_array[i] = out_flow_rate[0] + tmp;
            internal_array[i] = 0;
        }
        else
        {
            output_array[i] = out_flow_rate[0];
            internal_array[i] -= out_flow_rate[0];
        }
        current_level[i] = internal_array[i];
        if(debugmode)
        {
            printf("tmp= %f\n", tmp);
            printf("outflowrate= %f\n", output_array[0]);
        }
    }
	
}



// Install the module. This code is executed during start-up.

static InitClass init("Stock", &Stock::Create, "Source/UserModules/Stock/");


