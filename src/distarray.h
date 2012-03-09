#include <mpi.h>

typedef size_t distarray_global_t;


typedef struct {
} distarray_distribution_t;

typedef struct {
} distarray_copier_t;


#ifdef __cplusplus
extern "C" {
#endif

distarray_distribution_t *distarray_create_distribution(int ndim,
                                                        int *grid_shape,
                                                        int *grid,
                                                        size_t item_size,
                                                        MPI_Comm comm);
void distarray_destroy(distarray_distribution_t *dist);

int distarray_add_indexed_axis(distarray_distribution_t *dist,
                               size_t global_len,
                               size_t local_stride,
                               size_t *n,
                               distarray_global_t *indices);

distarray_copier_t *distarray_create_copier(distarray_distribution_t *target,
                                            distarray_distribution_t *source);

int distarray_copy(distarray_copier_t * copier, void *target_local_buf, void *source_local_buf);


#ifdef __cplusplus
}
#endif
