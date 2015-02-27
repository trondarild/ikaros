//
//	Randomizer.cc	This file is a part of the IKAROS project
// 					Implements a a modules that outputs a random array
//
//    Copyright (C) 2003-2011 Christian Balkenius
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
//	Created: 2003-07-14
//


#include "Randomizer.h"

using namespace ikaros;

const char *c_uniform = "uniform";
const char *c_gaussian = "gaussian";
void
Randomizer::Init()
{
    Bind(maximum, "max");
    Bind(minimum, "min");
    Bind(interval, "interval");
    distribution = GetValue("distribution");
    Bind(meanval, "mean");
    Bind(variance, "variance");
    
    output		=	GetOutputArray("OUTPUT");
    outputsize	=	GetOutputSize("OUTPUT");	// Use one-dimensional internally
    done = false;
}




void
Randomizer::Tick()
{
    // do only once if interval set to -1
    if (interval == -1) {
        if(!done) {
            if(equal_strings(distribution, c_uniform))
               random(output, minimum, maximum, outputsize);
            else
                ikaros::sprand(output, outputsize, 1.f, minimum, maximum, meanval, variance);
            done = true;
        }
    } else {
        if(GetTick() % interval == 0){
            if(equal_strings(distribution, c_uniform))
                random(output, minimum, maximum, outputsize);
            else
                ikaros::sprand(output, outputsize, 1.f, minimum, maximum, meanval, variance);
        }
    }
}


static InitClass init("Randomizer", &Randomizer::Create, "Source/Modules/UtilityModules/Randomizer/");
