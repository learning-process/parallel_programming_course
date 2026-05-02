#include "../../../nazyrov_a_a_vector_avg/seq/include/ops_seq.hpp"

#include <numeric>

namespace nazyrov_a_a_vector_avg {

VectorAvgSEQ::VectorAvgSEQ(const InType &in) : BaseTask() {
  GetInput() = in;
}

bool VectorAvgSEQ::ValidationImpl() {
  return true;
}

bool VectorAvgSEQ::PreProcessingImpl() {
  return true;
}

bool VectorAvgSEQ::RunImpl() {
  const auto &input = GetInput();
  double sum = std::accumulate(input.begin(), input.end(), 0.0);
  GetOutput() = sum / static_cast<double>(input.size());
  return true;
}

bool VectorAvgSEQ::PostProcessingImpl() {
  return true;
}

}  // namespace nazyrov_a_a_vector_avg
