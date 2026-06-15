#pragma once

namespace ppc::util {

class OshRuntime {
 public:
  OshRuntime() = delete;

  static void EnsureInitialized();
  static void Finalize();
  static bool IsInitialized();
  static int MyPe();
  static int NumPes();
  static void BarrierAll();
  static void GlobalExit(int status);
};

}  // namespace ppc::util
