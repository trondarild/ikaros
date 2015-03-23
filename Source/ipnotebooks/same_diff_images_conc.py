# -*- coding: utf-8 -*-
"""
Created on Mon Mar 23 14:29:58 2015

@author: trondarildtjostheim
"""

from os import listdir
from os.path import isfile, join
from random import shuffle
from numpy import matrix
from numpy.random import randn
# from numpy.linalg import inv
from conceptor import generate_internal_weights, conceptor_similarity,\
    evolve_conceptor
from scipy import misc


  

def conceptor_compare(inp1, inp2, noisefactor):
    # set up weights and aperture
    input_rows, cols = inp1.shape
    netsize = 100
    W = 1.5*generate_internal_weights(netsize, 0.1)
    W_in = matrix(1.5*randn(netsize, input_rows*input_rows))
    bias = 0.2*randn(netsize, 1)
    learnlength = 1000
    washout = 500
    aperture = 10
    inp_mat1 = (matrix(inp1.flatten(0))).transpose()
    inp_mat2 = (matrix(inp2.flatten(0))).transpose()
    
    
    conc1 = evolve_conceptor(W,
                        W_in,
                        bias,
                        netsize,
                        learnlength,
                        washout,
                        aperture,
                        inp_mat1,
                        noisefactor)
    conc2 = evolve_conceptor(W,
                        W_in,
                        bias,
                        netsize,
                        learnlength,
                        washout,
                        aperture,
                        inp_mat2,
                        noisefactor)
    return conceptor_similarity(conc1, conc2)



mypath = '/Users/trondarildtjstheim/Desktop/images for same different/gray20x20'
lol = lambda lst, sz: [lst[i:i+sz] for i in range(0, len(lst), sz)]
onlyfiles = [ f for f in listdir(mypath) if isfile(join(mypath,f)) ]
numfiles = len(onlyfiles)
# split files            
chunks = lol(onlyfiles, numfiles/3)
same = [(x, x) for x in chunks[0]]#chunks[0]
diff = [(chunks[1][x], chunks[2][x]) for x in range(len(chunks[1]))]
all_img = same
all_img.extend(diff)
img_indeces = [i for i in range(len(all_img))]
shuffle(img_indeces)
max_gray_val = 255.0
noisefactor = 0.0

for ix in img_indeces:
    # read the images
    pair = all_img[ix]
    im1 = misc.imread(join(mypath, pair[0]))
    im2 = im1
    correct = 'same'
    if not (pair[0] == pair[1]):
        im2 = misc.imread(join(mypath, pair[1]))
        correct = 'different'
    im1 = im1/max_gray_val
    im2 = im2/max_gray_val
    
    simil_val = conceptor_compare(im1, im2, noisefactor)
    print pair[0], pair[1], simil_val, correct
    

