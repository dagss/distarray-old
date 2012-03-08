#include "distarray.hpp"

#include <stdexcept>

namespace distarray {
  
  Distribution::Distribution(std::vector<size_t> global_shape,
                             std::vector<AxisDistribution_p> axis_distributions,
                             MPI_Comm comm) :
    _local_shape(global_shape.size()),
    _global_shape(global_shape),
    _axis_distributions(axis_distributions),
    _comm(comm)
  {
    if (global_shape.size() != axis_distributions.size()) {
      throw std::logic_error("shape and axis_distributions must have same length");
    }
    // Figure out local shape
    size_t ndim = global_shape.size();
    for (size_t axis = 0; axis != ndim; ++axis) {
      _local_shape[axis] = axis_distributions[axis]->get_local_length(global_shape[axis],
                                                                      comm);
    }
  }

  Distribution::~Distribution() {

  }




  class GenericCopier : public Copier {
  public:
    GenericCopier(Distribution *target_dist, Distribution *source_dist) {
      
    }
    void copy(double *target, double *source);
  };

  void GenericCopier::copy(double *target, double *source) {
    
  }


  Copier *create_copier(Distribution *target_dist, Distribution *source_dist) {
    if (target_dist->comm() != source_dist->comm()) {
      throw std::logic_error("target and source distribution do not share MPI communicator");
    }
    return new GenericCopier(target_dist, source_dist);
  }
  

}
