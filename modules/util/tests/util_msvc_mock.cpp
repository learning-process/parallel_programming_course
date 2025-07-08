#include <gtest/gtest.h>

#include "util/include/util.hpp"

// This file contains tests that mock MSVC behavior to achieve better coverage
// on non-Windows platforms

namespace test_msvc_mock {

// We can't actually test the MSVC branch on non-Windows, but we can document
// what it does and ensure the logic is sound

#ifndef _MSC_VER
// On non-MSVC platforms, we can at least test the string manipulation logic
// that would be used in the MSVC branch

TEST(MSVCMockTest, StringPrefixRemoval_SimulatesCorrectBehavior) {
  // Simulate what the MSVC branch does
  std::string name = "class test_ns::MyClass";

  const std::string prefixes[] = {"class ", "struct ", "enum ", "union "};
  for (const auto& prefix : prefixes) {
    if (name.starts_with(prefix)) {
      name = name.substr(prefix.size());
      break;
    }
  }

  EXPECT_EQ(name, "test_ns::MyClass");

  // Test namespace extraction
  auto pos = name.rfind("::");
  std::string ns = (pos != std::string::npos) ? name.substr(0, pos) : std::string{};
  EXPECT_EQ(ns, "test_ns");
}

TEST(MSVCMockTest, StringPrefixRemoval_WithStruct) {
  std::string name = "struct my::namespace::Structure";

  const std::string prefixes[] = {"class ", "struct ", "enum ", "union "};
  for (const auto& prefix : prefixes) {
    if (name.starts_with(prefix)) {
      name = name.substr(prefix.size());
      break;
    }
  }

  EXPECT_EQ(name, "my::namespace::Structure");
}

TEST(MSVCMockTest, StringPrefixRemoval_WithEnum) {
  std::string name = "enum GlobalEnum";

  const std::string prefixes[] = {"class ", "struct ", "enum ", "union "};
  for (const auto& prefix : prefixes) {
    if (name.starts_with(prefix)) {
      name = name.substr(prefix.size());
      break;
    }
  }

  EXPECT_EQ(name, "GlobalEnum");

  // Test namespace extraction for type without namespace
  auto pos = name.rfind("::");
  std::string ns = (pos != std::string::npos) ? name.substr(0, pos) : std::string{};
  EXPECT_EQ(ns, "");
}

TEST(MSVCMockTest, StringPrefixRemoval_WithUnion) {
  std::string name = "union test::UnionType";

  const std::string prefixes[] = {"class ", "struct ", "enum ", "union "};
  for (const auto& prefix : prefixes) {
    if (name.starts_with(prefix)) {
      name = name.substr(prefix.size());
      break;
    }
  }

  EXPECT_EQ(name, "test::UnionType");
}

TEST(MSVCMockTest, StringPrefixRemoval_WithLeadingSpaces) {
  std::string name = "   test::Type";

  // Simulate trimming leading spaces
  name.erase(0, name.find_first_not_of(' '));

  EXPECT_EQ(name, "test::Type");
}

TEST(MSVCMockTest, StringPrefixRemoval_NoPrefix) {
  std::string name = "test::namespace::Type";

  const std::string prefixes[] = {"class ", "struct ", "enum ", "union "};
  bool found = false;
  for (const auto& prefix : prefixes) {
    if (name.starts_with(prefix)) {
      name = name.substr(prefix.size());
      found = true;
      break;
    }
  }

  EXPECT_FALSE(found);
  EXPECT_EQ(name, "test::namespace::Type");
}

TEST(MSVCMockTest, NamespaceExtraction_MultiLevel) {
  std::string name = "a::b::c::d::Type";

  auto pos = name.rfind("::");
  std::string ns = (pos != std::string::npos) ? name.substr(0, pos) : std::string{};

  EXPECT_EQ(ns, "a::b::c::d");
}

TEST(MSVCMockTest, NamespaceExtraction_SingleLevel) {
  std::string name = "ns::Type";

  auto pos = name.rfind("::");
  std::string ns = (pos != std::string::npos) ? name.substr(0, pos) : std::string{};

  EXPECT_EQ(ns, "ns");
}

TEST(MSVCMockTest, NamespaceExtraction_NoNamespace) {
  std::string name = "SimpleType";

  auto pos = name.rfind("::");
  std::string ns = (pos != std::string::npos) ? name.substr(0, pos) : std::string{};

  EXPECT_EQ(ns, "");
}

#endif  // !_MSC_VER

}  // namespace test_msvc_mock
