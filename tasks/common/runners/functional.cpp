#include <gtest/gtest.h>

#include "oneapi/tbb/global_control.h"
#include "runners/include/runners.hpp"
#include "util/include/util.hpp"

int main(int argc, char **argv) {
  if (ppc::util::IsUnderMpirun()) {
    return ppc::runners::Init(argc, argv);
  }
  return ppc::runners::SimpleInit(argc, argv);
}
