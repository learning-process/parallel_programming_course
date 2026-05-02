#pragma once

#include "../../../nazyrov_a_a_vector_avg/common/include/common.hpp"

namespace nazyrov_a_a_vector_avg {

class VectorAvgMPI : public BaseTask {
 public:
  explicit VectorAvgMPI(const InType &in);
  ~VectorAvgMPI() override = default;

  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }

 protected:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;

 private:
  int world_rank_{0};
  int world_size_{1};
};

}  // namespace nazyrov_a_a_vector_avg
