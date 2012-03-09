#ifndef _DISTARRAY_H_
#define _DISTARRAY_HPP_

#include <stdint.h>
#include <vector>
#include <tr1/memory>
#include <stdexcept>

#include "distarray.h"

namespace distarray {



  class AxisDistribution;
  typedef ::std::tr1::shared_ptr<AxisDistribution> AxisDistribution_p;

  class Distribution {
  public:
    Distribution(int ndim,
                 int *grid_shape,
                 int *grid,
                 size_t item_size,
                 MPI_Comm comm);

    Distribution(std::vector<int> grid_shape,
                 std::vector<int> grid,
                 std::vector<size_t> global_shape,
                 std::vector<AxisDistribution_p> axis_distributions,
                 size_t item_size,
                 MPI_Comm comm);

    virtual ~Distribution();

    void add_axis(size_t global_len,
                  size_t local_stride,
                  AxisDistribution_p axis);

    std::vector<size_t>& local_shape();
    std::vector<size_t>& global_shape();
    const MPI_Comm &comm() {
      return _comm;
    }
    size_t ndim() {
      return _grid_shape.size();
    }
    size_t item_size() {
      return _item_size;
    }
    void ensure_initialized() {
      if (_axis_distributions.size() != ndim()) {
        throw std::logic_error("add_axis called too few times");
      }
    }
  private:
    void _initialize();

    std::vector<int> _grid_shape, _grid;
    std::vector<size_t> _local_shape, _global_shape, _local_strides;
    std::vector<AxisDistribution_p> _axis_distributions;
    size_t _item_size;
    MPI_Comm _comm;
  };
  

  class AxisDistribution {
  public:
    virtual ~AxisDistribution() {}
    virtual size_t get_local_length(size_t global_length, MPI_Comm comm) = 0;
  };


  template <typename T>
  class DistArray {
  public:
    DistArray(Distribution *distribution, T *data) :
      _distribution(distribution), _data(data) {}
  private:
    Distribution *_distribution;
    T *_data;
  };


  class Copier {
  public:
    virtual ~Copier() {};
    virtual void copy(void *target, void *source) = 0;
  };



  Copier *create_copier(Distribution *target_dist, Distribution *source_dist);


  /* Particular AxisDistributions */
  class DistributeByIndex : public AxisDistribution {
  public:
    DistributeByIndex(size_t n, distarray_global_t *indices) :
      _n(n), _indices(indices) {}

    virtual size_t get_local_length(size_t global_length, MPI_Comm comm) {
      return _n;
    }
  private:
    size_t _n;
    distarray_global_t *_indices;
  };

}

#endif /* _DISTARRAY_HPP_ */
