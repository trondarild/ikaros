//
//	MedianFilter.cc	This file is a part of the IKAROS project
//
//
//    Copyright (C) 2013  Christian Balkenius
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

#include "MedianFilter.h"
#include "ctype.h"

using namespace ikaros;



void
MedianFilter::SetSizes()
{
    kernel_size = GetIntValue("kernel_size");
    int sx = GetInputSizeX("INPUT");
    int sy = GetInputSizeY("INPUT");

    if (sx != unknown_size && sy != unknown_size)
        SetOutputSize("OUTPUT", sx-kernel_size+1, sy-kernel_size+1);
    
    Module::SetSizes();
}



MedianFilter::~MedianFilter()
{
    destroy_array(buffer);
}



void
MedianFilter::Init()
{
    size_x	 = GetOutputSizeX("OUTPUT");
    size_y	 = GetOutputSizeY("OUTPUT");

    input    = GetInputMatrix("INPUT");
    output   = GetOutputMatrix("OUTPUT");
    
    buffer   = create_array(kernel_size*kernel_size);
}



void
MedianFilter::Tick()
{
    int kernel_size_2 = kernel_size * kernel_size;
    int median_pos = kernel_size_2/2;
    
    for(int i=0; i<size_x; i++)
        for(int j=0; j<size_y; j++)
        {
            int p = 0;
            for (int l=0; l<kernel_size; l++)
            {
                float * src = &input[j+l][i];
                for (int k=0; k<kernel_size; k++)
                    buffer[p++] = (*src++);
            }
        
            sort(buffer, kernel_size_2);
            
            output[j][i] = buffer[median_pos];
        }
}



static InitClass init("MedianFilter", &MedianFilter::Create, "Source/Modules/VisionModules/ImageOperators/MedianFilter/");

