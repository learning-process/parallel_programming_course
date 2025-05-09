#pragma once
#include <string>
#include <vector>

namespace ppc::util {

std::string GetAbsolutePath(const std::string &relative_path);
int GetPPCNumThreads();
bool GetImageData(const std::string &abs_path, std::vector<uint8_t> &image, int &width, int &height, int &channels);

}  // namespace ppc::util
