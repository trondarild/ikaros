#
# functions for conceptor and reservoir processing
#
"""Random sparse matrices"""

import numpy as np
from scipy.sparse import csr_matrix
from scipy import rand, randn

def _rand_sparse(m, n, density, format='csr'):
    """Helper function for sprand, sprandn"""

    nnz = max( min( int(m*n*density), m*n), 0)

    row  = np.random.random_integers(low=0, high=m-1, size=nnz)
    col  = np.random.random_integers(low=0, high=n-1, size=nnz)
    data = np.ones(nnz, dtype=float)

    # duplicate (i,j) entries will be summed together
    return csr_matrix((data,(row,col)), shape=(m,n))

def sprand(m, n, density, format='csr'):
    """Returns a random sparse matrix.

    Parameters
    ----------
    m, n : int
        shape of the result
    density : float
        target a matrix with nnz(A) = m*n*density, 0<=density<=1
    format : string
        sparse matrix format to return, e.g. 'csr', 'coo', etc.

    Returns
    -------
    A : sparse matrix
        m x n sparse matrix

    Examples
    --------
    >>> import numpy
    >>> A = sprand(5,5,3/5.0)

    """
    m,n = int(m),int(n)
    # get sparsity pattern
    A = _rand_sparse(m, n, density, format='csr')
    # replace data with random values - need 
    A.data = rand(A.nnz)
    return A.asformat(format)

def sprandn(m, n, density, format='csr'):
    """Returns a random sparse matrix.

    Parameters
    ----------
    m, n : int
        shape of the result
    density : float
        target a matrix with nnz(A) = m*n*density, 0<=density<=1
    format : string
        sparse matrix format to return, e.g. 'csr', 'coo', etc.

    Returns
    -------
    A : sparse matrix
        m x n sparse matrix

    Examples
    --------
    >>> import numpy
    >>> A = sprand(5,5,3/5.0)

    """
    m,n = int(m),int(n)
    # get sparsity pattern
    A = _rand_sparse(m, n, density, format='csr')
    # replace data with random values - need 
    A.data = randn(A.nnz)
    return A.asformat(format)

# from http://pydoc.net/Python/oceans/0.2.1/oceans.ff_tools.teaching/
def rmsd(x, y, normalize=False):
    """Compute root mean square difference (or distance).
    The normalized root-mean-square deviation or error (NRMSD or NRMSE) is the
    RMSD divided by the range of observed values.  The value is often expressed
    as a percentage, where lower values indicate less residual variance.

    Note: does not handle sparse matrices!
    """
    x, y = map(np.asanyarray, (x, y))
    rmsd = np.sqrt(np.sum(np.power((x - y), 2)) / x.size)
    if normalize:
        rmsd = rmsd / x.ptp()
    return rmsd

from numpy import matrix, identity
from numpy.linalg import inv
def ridgeregression(a, b, alpha):
    rows, cols = a.shape
    I = matrix(identity(rows))
    retval = (inv(a*a.transpose() +\
                    alpha*I) * a *\
                    b.transpose()).transpose()
    return retval

from numpy.linalg import svd, norm
from numpy import sqrt
def conceptor_similarity (a, b): 
      U_a, S_a, V_a = svd(a);
      U_b, S_b, V_b = svd(b);
      S_a = diag(S_a)
      S_b = diag(S_b)
      # similarity with previous conceptor
      return pow(norm((sqrt(S_a) * U_a.transpose() * \
              U_b * sqrt(S_b))),2) / \
              (norm(a) * norm(b));


def NOT(R):
	# NOT defined by I - R

	dim, col = R.shape

	return  identity(dim) - R
	# U, S, V = svd(notR)
	# 
	# nout = max(nargout,1)-1
	# varargout = []
	# for k in range(nout):
	#     if k == 1:
	#         varargout[k] = U
	#     elif k == 2:
	#         varargout[k] = S

from numpy import diag
from numpy.linalg import pinv
def AND(C, B):
	
	dim, col = C.shape
	tolerance = 1e-14

	UC, SC, UtC = svd(C)
	UB, SB, UtB = svd(B)

	diag_SC = diag(SC)
	diag_SB = diag(SB)

	# sum up how many elements on diagonal 
	# are bigger than tolerance
	numRankC =  (1.0 * (diag_SC > tolerance)).sum()
	numRankB =  (1.0 * (diag_SB > tolerance)).sum()

	UC0 = matrix(UC[:, numRankC:])
	UB0 = matrix(UB[:, numRankB:])
	W, Sigma, Wt = svd(UC0 * UC0.transpose() + UB0 * UB0.transpose())
	numRankSigma =  (1.0 * (diag(Sigma) > tolerance)).sum()
	Wgk = matrix(W[:, numRankSigma:])
	I = matrix(identity(dim))
	CandB = \
	  Wgk * inv(Wgk.transpose() *  \
	  ( pinv(C, tolerance) + pinv(B, tolerance) - \
	    I) * Wgk) *Wgk.transpose()
	return CandB

def OR(R,Q):
	RorQ = NOT(AND(NOT(R), NOT(Q)))
	return RorQ

from numpy import Inf
def PHI(C, gamma):
	dim, cols = C.shape
	# Cnew = matrix()
	if gamma == 0:
	    U, S, V = svd(C);
	    Sdiag = diag(S); # diagonal of S is the singular values
	    # Sdiag[Sdiag < 1] = zeros((Sdiag < 1).sum(), 1);
	    Sdiag = (Sdiag<1).choose(Sdiag, 0)
	    Cnew = U * Sdiag * U.transpose()    
	    
	elif gamma == Inf:
	    U, S, V = svd(C)
	    Sdiag = diag(S)
	    # Sdiag[Sdiag > 0] = ones((Sdiag > 0).sum(), 1)
	    Sdiag = (Sdiag>0).choose(Sdiag, 1.)
	    Cnew = U * Sdiag * U.transpose() 
	    
	else:
	    Cnew = C * inv(C + pow(gamma,-2) * (identity(dim) - C))
	return Cnew

from scipy.sparse.linalg import eigs 
def generate_internal_weights(num_units, connectivity):
    success = False    
    while not success:
        try: # eigs can generate exception
            weights = sprandn(num_units, num_units, connectivity)
            spectral_radius = abs(eigs(weights, k=1, 
                                       which = 'LM',
                                       return_eigenvectors = False))
            weights = weights/spectral_radius
            success = True
            return weights
        except:
            pass
        
from numpy import float64, zeros, tanh        
def evolve_conceptor(
        W,
        W_in,
        bias,
        netsize,
        learnlength,
        washout,
        aperture,
        inp, 
        noisefactor):
    # evolve conceptor for static input
    
    I = matrix (identity(netsize))
    x_collector = matrix(zeros((netsize, learnlength), float64))
    x = matrix(zeros((netsize, 1), float64))
    inp_dim, cols = inp.shape
    for n in range(washout + learnlength):
        noise_vec = noisefactor * rand(inp_dim,1)
        x = tanh(W*x + W_in*(inp + noise_vec) + bias) #+ noisefactor * randn()  
        
        if n > washout:
            x_collector[:, n-washout] = x
    # calc conceptor
    R = x_collector*x_collector.transpose()/learnlength
    conceptor = R * inv(R + pow(aperture, -2)*I)
    return conceptor  