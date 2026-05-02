#include "../../../nazyrov_a_a_vector_avg/mpi/include/ops_mpi.hpp"

#include <mpi.h>

#include <numeric>

namespace nazyrov_a_a_vector_avg {

VectorAvgMPI::VectorAvgMPI(const InType &in) : BaseTask() {
  GetInput() = in;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank_);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size_);
}

bool VectorAvgMPI::ValidationImpl() {
  return true;
}

bool VectorAvgMPI::PreProcessingImpl() {
  return true;
}

bool VectorAvgMPI::RunImpl() {
  const auto &input = GetInput();
  int n = static_cast<int>(input.size());
  int local_n = n / world_size_;
  int start = world_rank_ * local_n;
  int end = (world_rank_ == world_size_ - 1) ? n : start + local_n;

  double local_sum = std::accumulate(input.begin() + start, input.begin() + end, 0.0);
  int local_count = end - start;

  double global_sum = 0.0;
  int global_count = 0;

  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (world_rank_ == 0) {
    GetOutput() = global_sum / static_cast<double>(global_count);
  }

  return true;
}

bool VectorAvgMPI::PostProcessingImpl() {
  return true;
}

}  // namespace nazyrov_a_a_vector_avg
