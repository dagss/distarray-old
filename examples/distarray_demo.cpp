#include <stdexcept>
#include <iostream>

#include <tr1/memory>

#include <distarray.hpp>

using namespace distarray;

void mpi_check(int retcode) {
  if (retcode != MPI_SUCCESS) {
    throw std::runtime_error("mpi error");
  }
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  MPI_Comm comm = MPI_COMM_WORLD;
  int my_rank, commsize;

  mpi_check(MPI_Comm_rank(comm, &my_rank));
  mpi_check(MPI_Comm_size(comm, &commsize));

  distarray_global_t indices_a[3], indices_b[3];
  double a[3];
  double b[3] = {0, 0, 0};

  if (my_rank == 0) {
    indices_a[0] = 0;
    indices_a[1] = 2;
    indices_a[2] = 3;

    indices_b[0] = 0;
    indices_b[1] = 1;
    indices_b[2] = 2;

    a[0] = 0;
    a[1] = 1;
    a[2] = 2;
  } else {
    indices_a[0] = 1;
    indices_a[1] = 4;
    indices_a[2] = 5;

    indices_b[0] = 3;
    indices_b[1] = 4;
    indices_b[2] = 5;

    a[0] = 3;
    a[1] = 4;
    a[2] = 5;
  }
  std::vector<distarray_global_t> shape;
  std::vector<AxisDistribution_p> axes_a, axes_b;

  std::vector<int> grid;
  grid.push_back(0);
  grid.push_back(1);

  std::vector<int> grid_shape;
  grid.push_back(2);

  shape.push_back(6);
  axes_a.push_back(AxisDistribution_p(new DistributeByIndex(3, indices_a)));
  axes_b.push_back(AxisDistribution_p(new DistributeByIndex(3, indices_b)));
  Distribution dist_a(grid_shape, grid, shape, axes_a, sizeof(double), comm);
  Distribution dist_b(grid_shape, grid, shape, axes_b, sizeof(double), comm);

  std::tr1::shared_ptr<Copier> copier(create_copier(&dist_a, &dist_b));

  copier->copy(b, a);
  for (int irank = 0; irank != commsize; ++irank) {
    if (my_rank == irank) {
      std::cout << "Data on rank " << irank << std::endl;
      for (int i = 0; i != 3; ++i) std::cout << b[i] << " ";
      std::cout << std::endl;
    }
    mpi_check(MPI_Barrier(comm));
  }


  MPI_Finalize();
  return 0;
}
