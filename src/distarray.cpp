#include "distarray.hpp"

#include <stdexcept>

namespace distarray {

  int product(int *start, int *stop) {
    int result = 1;
    while (start != stop) {
      result *= *start;
      ++start;
    }
    return result;
  }

  Distribution::Distribution(int ndim,
                             int *grid_shape,
                             int *grid,
                             size_t item_size,
                             MPI_Comm comm) :
    _grid_shape(grid_shape, grid_shape + ndim),
    _grid(grid, grid + product(grid_shape, grid_shape + ndim)),
    _item_size(item_size),
    _comm(comm)
  {
  }

  
  Distribution::Distribution(std::vector<int> grid_shape,
                             std::vector<int> grid,
                             std::vector<size_t> global_shape,
                             std::vector<AxisDistribution_p> axis_distributions,
                             size_t item_size,
                             MPI_Comm comm) :
    _grid_shape(grid_shape),
    _grid(grid),
    _local_shape(global_shape.size()),
    _global_shape(global_shape),
    _axis_distributions(axis_distributions),
    _item_size(item_size),
    _comm(comm)
  {
    if (global_shape.size() != axis_distributions.size()) {
      throw std::logic_error("shape and axis_distributions must have same length");
    }
    _initialize();
  }

  void Distribution::add_axis(size_t global_len,
                              size_t local_stride,
                              AxisDistribution_p axis) {
    if (_axis_distributions.size() == ndim()) {
      throw std::logic_error("add_axis called too many times");
    }
    _axis_distributions.push_back(axis);
    if (_axis_distributions.size() == ndim()) {
      _initialize();
    }
  }

  void Distribution::_initialize() {
    // Figure out local shape
    size_t ndim = _global_shape.size();
    for (size_t axis = 0; axis != ndim; ++axis) {
      _local_shape[axis] = _axis_distributions[axis]->get_local_length(_global_shape[axis],
                                                                       _comm);
    }
  }
  

  Distribution::~Distribution() {

  }




  class GenericCopier : public Copier {
  public:
    GenericCopier(Distribution *target_dist, Distribution *source_dist) {
      
    }
    void copy(void *target, void *source);
  };

  void GenericCopier::copy(void *target, void *source) {
    
  }


  Copier *create_copier(Distribution *target_dist, Distribution *source_dist) {
    if (target_dist->comm() != source_dist->comm()) {
      throw std::logic_error("target and source distribution do not share MPI communicator");
    }
    if (target_dist->item_size() != source_dist->item_size()) {
      throw std::logic_error("target and source distribution do not share itemsize");
    }
    target_dist->ensure_initialized();
    source_dist->ensure_initialized();
    return new GenericCopier(target_dist, source_dist);
  }
  

}


using namespace distarray;

distarray_distribution_t *distarray_create_distribution(int ndim,
                                                        int *grid_shape,
                                                        int *grid,
                                                        size_t item_size,
                                                        MPI_Comm comm) {
  try {
    Distribution *result = new Distribution(ndim, grid_shape, grid,
                                            item_size, comm);
    return (distarray_distribution_t*)result;
  } catch (...) {
    return NULL;
  }
}

void distarray_destroy(distarray_distribution_t *dist) {
  delete (Distribution*)dist;
}

int distarray_add_indexed_axis(distarray_distribution_t *dist_,
                               size_t global_len,
                               size_t local_stride,
                               size_t n,
                               distarray_global_t *indices) {
  Distribution *dist = (Distribution*)dist_;
  try {
    dist->add_axis(global_len, local_stride,
                   AxisDistribution_p(new DistributeByIndex(n, indices)));
    return 0;
  } catch (...) {
    return -1;
  }
}

distarray_copier_t *distarray_create_copier(distarray_distribution_t *target,
                                            distarray_distribution_t *source) {
  try {
    return (distarray_copier_t*)create_copier((Distribution*)target, (Distribution*)source);
  } catch (...) {
    return NULL;
  }
}

int distarray_copy(distarray_copier_t *copier,
                   void *target_local_buf,
                   void *source_local_buf) {
  try {
    ((Copier*)copier)->copy(target_local_buf, source_local_buf);
    return 0;
  } catch (...) {
    return -1;
  }
}
