from mpi4py cimport MPI
import numpy as np
cimport numpy as cnp
cnp.import_array()

cdef extern from "distarray.h":
    ctypedef size_t distarray_global_t


    ctypedef struct distarray_distribution_t:
        pass

    ctypedef struct distarray_copier_t:
        pass
    
    distarray_distribution_t *distarray_create_distribution(int ndim,
                                                            int *grid,
                                                            size_t item_size,
                                                            MPI.MPI_Comm comm)
    
    void distarray_destroy(distarray_distribution_t *dist)

    int distarray_add_indexed_axis(distarray_distribution_t *dist,
                                   size_t global_len,
                                   size_t local_stride,
                                   size_t *n,
                                   distarray_global_t *indices)

    distarray_copier_t *distarray_create_copier(
        distarray_distribution_t *target,
        distarray_distribution_t *source)

    int distarray_copy(distarray_copier_t *copier,
                       void *target_local_buf,
                       void *source_local_buf)

if sizeof(distarray_global_t) == 4:
    global_dtype = np.int32
elif sizeof(distarray_global_t) == 8:
    global_dtype = np.int64
else:
    assert False

if sizeof(size_t) == 4:
    size_t_dtype = np.int32
elif sizeof(size_t) == 8:
    size_t_dtype = np.int64
else:
    assert False

cdef class Distribution:
    cdef distarray_distribution_t *wrapped

    def __init__(self, MPI.Comm comm, grid, dtype, axes):
        if isinstance(grid, tuple):
            grid = np.arange(np.prod(grid)).reshape(grid)
        grid = np.ascontiguousarray(grid, dtype=np.intc)
        ndim = grid.ndim
        itemsize = dtype(0).itemsize

        shape = [ax.global_length for ax in axes]
        shape = np.ascontiguousarray(shape, dtype=size_t_dtype)

        strides = [ax.local_stride for ax in axes]
        
        #if strides is None:
        #    # default to C/row-major ordering
        #    strides = (itemsize,)
        #    for d in shape[1:]:
        #        strides = (d * strides[0],) + strides
        #elif len(strides) != ndim:
        #    raise ValueError("len(strides) != ndim")
        strides = np.ascontiguousarray(strides, dtype=size_t_dtype)


        self.wrapped = distarray_create_distribution(
            ndim,
            <int*>cnp.PyArray_DATA(grid),
            itemsize,
            comm.ob_mpi)
            


class IndexedAxis:
    def __init__(self, global_length, global_indices, local_stride):
        self.global_indices = global_indices
        self.global_length = global_length
        self.local_stride = local_stride

    def apply(self, Distribution dist):
        pass
