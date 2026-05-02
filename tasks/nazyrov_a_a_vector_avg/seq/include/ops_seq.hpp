#pragma once

#include "../../../nazyrov_a_a_vector_avg/common/include/common.hpp"

namespace nazyrov_a_a_vector_avg {

class VectorAvgSEQ : public BaseTask {
 public:
  explicit VectorAvgSEQ(const InType &in);
  ~VectorAvgSEQ() override = default;

  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kSEQ;
  }

 protected:
  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};

}  // namespace nazyrov_a_a_vector_avg
