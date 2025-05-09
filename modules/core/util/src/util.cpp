#include "core/util/include/util.hpp"

#include <cstdlib>
#ifdef _WIN32
#include <cstdint>
#include <iostream>
#include <memory>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <filesystem>
#include <iostream>
#include <string>

std::string ppc::util::GetAbsolutePath(const std::string &relative_path) {
  const std::filesystem::path path = std::string(PPC_PATH_TO_PROJECT) + "/tasks/" + relative_path;
  return path.string();
}

int ppc::util::GetPPCNumThreads() {
#ifdef _WIN32
  size_t len;
  char omp_env[100];
  errno_t err = getenv_s(&len, omp_env, sizeof(omp_env), "OMP_NUM_THREADS");
  if (err != 0 || len == 0) {
    omp_env[0] = '\0';
  }
  int num_threads = std::atoi(omp_env);
#else
  const char *omp_env = std::getenv("OMP_NUM_THREADS");
  int num_threads = (omp_env != nullptr) ? std::atoi(omp_env) : 1;
#endif
  return num_threads;
}

bool ppc::util::GetImageData(const std::string &abs_path,
                             std::vector<uint8_t> &image, int &width, int &height, int &channels) {
  width = -1;
  height = -1;
  channels = -1;
  unsigned char *data = stbi_load(abs_path.c_str(), &width, &height, &channels, 0);
  if (data == nullptr) {
    std::cerr << "Failed to load image: " << stbi_failure_reason() << '\n';
    return false;
  }
  image = std::vector<uint8_t>(data, data + (width * height * channels));
  stbi_image_free(data);
  return true;
}
