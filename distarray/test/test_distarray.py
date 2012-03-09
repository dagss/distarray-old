from nose.tools import eq_, ok_, assert_raises

from mpi4py import MPI
import numpy as np

from .. import wrapper as distarray


comm = MPI.COMM_WORLD

def test_basic():
    i = np.arange(200)
    axes = [
        distarray.IndexedAxis(100, i[comm.Get_rank()::comm.Get_size()], 8)
        ]
    dist_a = distarray.Distribution(comm, (2, 1), np.float64, axes)
    
