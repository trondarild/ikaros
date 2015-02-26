//
//	SparseMatrix.h		This file is a part of the IKAROS project
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

#ifndef SparseMatrix_
#define SparseMatrix_

#include "IKAROS.h"

class SparseMatrix: public Module
{
public:
    static Module * Create(Parameter * p) { return new SparseMatrix(p); }

    SparseMatrix(Parameter * p) : Module(p) {}
    virtual ~SparseMatrix();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes


    float **    output_matrix;
    int         output_matrix_size_x;
    int         output_matrix_size_y;

    // internal data storage

    // parameter values

    int         dimension;
    float       fillfactor;
	bool       	debugmode;
};

#endif
