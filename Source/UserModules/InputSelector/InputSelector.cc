//
//	InputSelector.cc		This file is a part of the IKAROS project
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

#include "InputSelector.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;

void 
InputSelector::SetSizes()
{
    int sx = GetInputSizeX("INPUT");
    SetOutputSize("OUTPUT", sx);
}

void
InputSelector::Init()
{
	Bind(debugmode, "debug");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    select = GetInputArray("SELECT");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");
}



InputSelector::~InputSelector()
{
    // Destroy data structures that you allocated in Init.
    
}



void
InputSelector::Tick()
{
    output_array[0] = input_array[(int)select[0]];
	if(debugmode)
	{
		// print out debug info
        print_array(
            "InputSelector::input", 
            input_array, 
            input_array_size);
	}
}



// Install the module. This code is executed during start-up.

static InitClass init("InputSelector", &InputSelector::Create, "Source/UserModules/InputSelector/");


