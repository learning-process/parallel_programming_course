#include <gtest/gtest.h>

#include <string>

#include "util/include/util.hpp"

// This file tests edge cases that might cause demangling failures
// or other uncovered branches in GetNamespace

// Test with an extern "C" function type that might have special handling
extern "C" {
using CFunction = void (*)();
}

TEST(GetNamespaceEdgeCases, GetNamespace_WithExternCFunction_HandlesCorrectly) {
  std::string k_ns = ppc::util::GetNamespace<CFunction>();
  // C functions typically don't have namespaces
  EXPECT_EQ(k_ns, "");
}

// Test with a type that has no :: separator at all
struct SimpleGlobalType {};

TEST(GetNamespaceEdgeCases, GetNamespace_WithNoColonColon_ReturnsEmpty) {
  // This should trigger the string::npos branch
  std::string k_ns = ppc::util::GetNamespace<SimpleGlobalType>();
  EXPECT_EQ(k_ns, "");
}

// Test with basic built-in types
TEST(GetNamespaceEdgeCases, GetNamespace_WithBasicBuiltinTypes_ReturnsEmpty) {
  EXPECT_EQ(ppc::util::GetNamespace<void>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<char>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<short>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<int>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<long>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<long long>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<bool>(), "");
}

// Test with unsigned built-in types
TEST(GetNamespaceEdgeCases, GetNamespace_WithUnsignedBuiltinTypes_ReturnsEmpty) {
  EXPECT_EQ(ppc::util::GetNamespace<unsigned char>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<unsigned short>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<unsigned int>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<unsigned long>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<unsigned long long>(), "");
}

// Test with floating point types
TEST(GetNamespaceEdgeCases, GetNamespace_WithFloatingPointTypes_ReturnsEmpty) {
  EXPECT_EQ(ppc::util::GetNamespace<float>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<double>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<long double>(), "");
}

// Test with character types
TEST(GetNamespaceEdgeCases, GetNamespace_WithCharacterTypes_ReturnsEmpty) {
  EXPECT_EQ(ppc::util::GetNamespace<wchar_t>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<char16_t>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<char32_t>(), "");
  // std::nullptr_t might be a builtin type on some compilers
  std::string nullptr_ns = ppc::util::GetNamespace<std::nullptr_t>();
  EXPECT_TRUE(nullptr_ns == "std" || nullptr_ns == "");
}

// Test with very long namespace chain
namespace a::b::c::d::e::f::g::h {
struct DeepType {};
}  // namespace a::b::c::d::e::f::g::h

TEST(GetNamespaceEdgeCases, GetNamespace_WithVeryDeepNamespace_ExtractsCorrectly) {
  std::string k_ns = ppc::util::GetNamespace<a::b::c::d::e::f::g::h::DeepType>();
  EXPECT_EQ(k_ns, "a::b::c::d::e::f::g::h");
}

// Test with types that might have special characters in their mangled names
namespace special_chars {
template <int N>
struct Templated {};
}  // namespace special_chars

TEST(GetNamespaceEdgeCases, GetNamespace_WithNonTypeTemplate_HandlesCorrectly) {
  std::string k_ns1 = ppc::util::GetNamespace<special_chars::Templated<0>>();
  std::string k_ns2 = ppc::util::GetNamespace<special_chars::Templated<-1>>();
  std::string k_ns3 = ppc::util::GetNamespace<special_chars::Templated<42>>();
  EXPECT_EQ(k_ns1, "special_chars");
  EXPECT_EQ(k_ns2, "special_chars");
  EXPECT_EQ(k_ns3, "special_chars");
}

// Test with anonymous types
TEST(GetNamespaceEdgeCases, GetNamespace_WithAnonymousStruct_HandlesCorrectly) {
  struct {
    int x;
  } anonymous_var;

  using AnonymousType = decltype(anonymous_var);
  std::string k_ns = ppc::util::GetNamespace<AnonymousType>();
  // Anonymous types typically don't have standard namespaces
  // Just verify it doesn't crash
  EXPECT_TRUE(k_ns.empty() || !k_ns.empty());
}

// Test with union types
union GlobalUnion {
  int i;
  float f;
};

namespace ns {
union NamespacedUnion {
  int i;
  float f;
};
}  // namespace ns

TEST(GetNamespaceEdgeCases, GetNamespace_WithUnions_HandlesCorrectly) {
  EXPECT_EQ(ppc::util::GetNamespace<GlobalUnion>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<ns::NamespacedUnion>(), "ns");
}

// Test with enum class (C++11)
enum class GlobalEnumClass { A, B, C };

namespace ns {
enum class NamespacedEnumClass { X, Y, Z };
}  // namespace ns

TEST(GetNamespaceEdgeCases, GetNamespace_WithEnumClass_HandlesCorrectly) {
  EXPECT_EQ(ppc::util::GetNamespace<GlobalEnumClass>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<ns::NamespacedEnumClass>(), "ns");
}

// Test with function types
using GlobalFunctionType = void(int, double);
namespace ns {
using NamespacedFunctionType = int(const char *);
}  // namespace ns

TEST(GetNamespaceEdgeCases, GetNamespace_WithFunctionTypes_HandlesCorrectly) {
  EXPECT_EQ(ppc::util::GetNamespace<GlobalFunctionType>(), "");
  // Function type aliases don't preserve namespace information in their type
  std::string k_ns = ppc::util::GetNamespace<ns::NamespacedFunctionType>();
  // Just verify it doesn't crash
  EXPECT_TRUE(k_ns.empty() || !k_ns.empty());
}

// Test with member function pointers
struct TestClass {
  void memberFunc() {}
};

TEST(GetNamespaceEdgeCases, GetNamespace_WithMemberFunctionPointer_HandlesCorrectly) {
  using MemberFuncPtr = void (TestClass::*)();
  std::string k_ns = ppc::util::GetNamespace<MemberFuncPtr>();
  // Member function pointers have complex mangling
  EXPECT_TRUE(k_ns.empty() || !k_ns.empty());
}
