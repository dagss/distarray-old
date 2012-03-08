#ifndef _DISTARRAY_H_
#define _DISTARRAY_H_

#include <stdint.h>
#include <vector>
#include <tr1/memory>

#include <mpi.h>



typedef struct {
} distarray_distribution_t;

typedef struct {
} distarray_axis_t;


distarray_distribution_t distarray_create_distribution(
    int ndim, size_t *shape, distarray_axis_t *axes);

void distarray_destroy(distarray_distribution_t *dist);

distarray_axis_t distarray_create_indexed_axis();


namespace distarray {


  typedef size_t local_t;
  typedef size_t global_t;

  class AxisDistribution;
  typedef ::std::tr1::shared_ptr<AxisDistribution> AxisDistribution_p;

  class Distribution {
  public:
    Distribution(std::vector<size_t> global_shape,
                 std::vector<AxisDistribution_p> axis_distributions,
                 MPI_Comm comm);

    virtual ~Distribution();

    std::vector<size_t>& local_shape();
    std::vector<size_t>& global_shape();
    const MPI_Comm &comm() {
      return _comm;
    }
  private:
    std::vector<size_t> _local_shape, _global_shape;
    std::vector<AxisDistribution_p> _axis_distributions;
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
    virtual void copy(double *target, double *source) = 0;
  };



  Copier *create_copier(Distribution *target_dist, Distribution *source_dist);


  /* Particular AxisDistributions */
  class DistributeByIndex : public AxisDistribution {
  public:
    DistributeByIndex(size_t n, global_t *indices) :
      _n(n), _indices(indices) {}

    virtual size_t get_local_length(size_t global_length, MPI_Comm comm) {
      return _n;
    }
  private:
    size_t _n;
    global_t *_indices;
  };

}



#endif // _DISTARRAY_H_
