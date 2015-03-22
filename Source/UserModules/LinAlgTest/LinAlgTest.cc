//
//	LinAlgTest.cc		This file is a part of the IKAROS project
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

#include "LinAlgTest.h"
#include <Accelerate/Accelerate.h>
#include <vector>
#include <random>
// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;

//  Arguments:                                                                //
//     double *A    Pointer to the first element of the matrix A[n][n].       //
//     int    ncols The number of columns of the matrix A.                    //
//     double *S    Destination address of the submatrix.                     //
//     int    mrows The number of rows matrix S.                              //
//     int    mcols The number of columns of the matrix S.                    //
//     int    row   The row of A corresponding to the first row of S.         //
//     int    col   The column of A corresponding to the first column of S.   //
//                                                                            //
/*
void set_submatrix(float *A, int ncols, float *S, int mrows, int mcols,
                   int row, int col)
{
    int i,j;
    
    for ( A += row * ncols + col, i = 0; i < mrows; A += ncols, i++)
        for (j = 0; j < mcols; j++) *(A + j) = *S++;
}
*/
void test_function(int collector_size, int data_x, int data_y, bool by_col)
{
    float **data = create_matrix(data_x, data_y);
    random(data, 0.f, 1.f, data_x, data_y);
    print_matrix("data", data, data_x, data_y);
    
    float **collector;
    if(by_col){
        collector = create_matrix(collector_size*data_x, data_y);

        // test filling up collector
        for (int i=0; i<collector_size*data_x; i+=data_x) {
            set_submatrix(collector[0], collector_size*data_x, data[0], data_y, data_x, 0, i);
        }
        print_matrix("collector", collector, collector_size * data_x, data_y);
    }else {
        collector = create_matrix(data_x, collector_size*data_y);
        for (int i=0; i<collector_size*data_y; i+=data_y) {
            set_submatrix(collector[0], data_x, data[0], data_y, data_x, i, 0);
        }
        print_matrix("collector", collector, data_x, collector_size * data_y);
    }
    destroy_matrix(data);
    destroy_matrix(collector);
}

void
LinAlgTest::Init()
{
    Bind(parameter, "parameter1");
	Bind(debugmode, "debug");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");


    internal_array = create_array(10);

    // test sparse matrix gen
    //int dim = parameter;
    int size = 3;
    int rows = 2;
    int cols = 2;
    // fill with random values
    /*
    float **collector = create_matrix(size*cols, rows);
    float **data = create_matrix(cols, rows);
    random(data, 0.f, 1.f, cols, rows);
    print_matrix("data", data, cols, rows);
    // test filling up collector
    for (int i=0; i<size; i+=cols) {
        set_submatrix(collector[0], size*cols, data[0], rows, cols, 0, i);
    }
    */
    // TODO
    // write testfunction(int collector_size, int data_x, int data_y, char direction)
    // for each: x dir, y dir:
    // test scalars
    test_function(size, 1, 1, true); // by col
    test_function(size, 1, 1, false); // by row
    // test arrays
    test_function(size, 2, 1, true);
    test_function(size, 2, 1, false);
    // test matrices
    test_function(size, 2, 2, true);
    test_function(size, 2, 2, false);
    
    
}



LinAlgTest::~LinAlgTest()
{
    // Destroy data structures that you allocated in Init.
    destroy_array(internal_array);
}



void
LinAlgTest::Tick()
{
	if(debugmode)
	{
		// print out debug info
	}
}

int
LinAlgTest::eigs(float **result, float **matrix, int sizex, int sizey)
{
    char jobvl = 'N';
    char jobvr = 'V';
    int n = sizex;  //lapack wants num of cols

    // sgeev call seems to change original matrix, so make copy
    float **a = create_matrix(sizex, sizey);
    copy_matrix(a, matrix, sizex, sizey); 
    int lda = max(sizex, sizey); // leading dimension of array
    
    float *wr = result[0];      //real part output of eigenvalues - want this
    float *wi = new float[n];   //img part output
    
    float *vl = new float[n];   // left eigenvectors - not used
    int ldvl = n;               // not used
    float *vr = new float[n];   // right eigenvectors - not used
    int ldvr = n;
    
    float work[3*n];
    int lwork = 3*n + 32;
    int info;
    
    // call to lapack sgeev = single-general matrix-eigen vector
    sgeev_(&jobvl, &jobvr, &n,
           a[0], &lda, wr, wi,
           vl, &ldvl, vr, &ldvr,
           & (*work), & lwork, &info);
    
    delete[] wi;
    delete[] vl;
    delete[] vr;
    return info;
}

void
LinAlgTest::sprand(float *array, int size, float sparsefactor)
{
    std::default_random_engine generator;
    // normal distro with mean = 0.5 and
    std::normal_distribution<float> distribution(0.5,0.5);

    int numfilledelements = (int)size*sparsefactor;
    std::vector<int> indeces;
    indeces.reserve(numfilledelements);
    
    // generate a list of random ints for indeces
    // and add a number from normal distribution
    for(int i=0; i<numfilledelements; i++){
        std::vector<int>::iterator it;
        int index = rand() % size;
        it = std::find(indeces.begin(), indeces.end(), index);
        if (it == indeces.end()){
            indeces.push_back(index);
            float value=-1;
            do {
                value = distribution(generator);
            } while ((value<0.0)||(value>1.0));
            array[index] = value;
        }
    }
}

void 
LinAlgTest::gen_weight_matrix(float **returnmat, int dim, float fillfactor)
{
    float **eigenvals = create_matrix(dim,1);
    float spectralradius=0;
    int maxiter=10;
    do{
        LinAlgTest::sprand(returnmat[0], dim*dim, fillfactor);
        //print_matrix("LinAlgTest::sprand", returnmat, dim, dim);
        int info = LinAlgTest::eigs(eigenvals, returnmat, dim, dim);
        // print_matrix("LinAlgTest::eigs", eigenvals, dim, 1);
        spectralradius = max(abs(eigenvals, dim, 1), dim, 1);  
        //printf("LinAlgTest::spectralrad=%f\n", spectralradius);  
    }while(spectralradius==0 && (maxiter-- > 0));
    
    //print_matrix("LinAlgTest::before", returnmat, dim, dim);
    returnmat = multiply(returnmat, 1.f/spectralradius, dim, dim);
}




// Install the module. This code is executed during start-up.

static InitClass init("LinAlgTest", &LinAlgTest::Create, "Source/UserModules/LinAlgTest/");


