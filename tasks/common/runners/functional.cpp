#include <gtest/gtest.h>

#include "core/runners/include/runners.hpp"
#include "core/util/include/util.hpp"
#include "oneapi/tbb/global_control.h"

int main(int argc, char** argv) {
  if (ppc::util::IsUnderMpirun()) {
    return ppc::core::Init(argc, argv);
  }
  return ppc::core::SimpleInit(argc, argv);
}
