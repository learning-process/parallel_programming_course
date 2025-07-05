#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "util/include/util.hpp"

class RunnersAdditionalTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Setup for each test
  }

  void TearDown() override {
    // Clean up after each test
  }
};

// Keep only unique functionality tests - InitJSONPtr
// No environment variable manipulation needed here

TEST_F(RunnersAdditionalTest, InitJSONPtr_BasicFunctionality) {
  // Test the InitJSONPtr function
  auto json_ptr = ppc::util::InitJSONPtr();

  // Verify the JSON pointer is valid
  EXPECT_NE(json_ptr, nullptr);

  // Test adding data to the JSON pointer - simplified to reduce complexity
  (*json_ptr)["test_key"] = "test_value";
  EXPECT_EQ((*json_ptr)["test_key"], "test_value");
}

TEST_F(RunnersAdditionalTest, InitJSONPtr_EmptyJSON) {
  // Test with empty JSON
  auto json_ptr = ppc::util::InitJSONPtr();

  // Should still return a valid pointer
  EXPECT_NE(json_ptr, nullptr);

  // Should be empty initially
  EXPECT_TRUE(json_ptr->empty());
}
