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
    return 

from numpy import linalg, matrix, identity
def ridgeregression(a, b, alpha):
    rows, cols = a.shape
    I = matrix(identity(rows))
    retval = (linalg.inv(a*a.transpose() +\
                    alpha*I) * a *\
                    b.transpose()).transpose()
    return retval

from numpy.linalg import svd, norm
from numpy import sqrt
def conceptor_similarity (a, b):
       
 	U_a, S_a, V_a = svd(a);
 	U_b, S_b, V_b = svd(b);
 	# similarity with previous conceptor
 	return pow(norm((sqrt(S_a) * U_a.transpose() * \
 	         U_b * sqrt(S_b))),2) / \
 	         (norm(a) * norm(b));