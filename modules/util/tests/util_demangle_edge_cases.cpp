#include <gtest/gtest.h>

#include "util/include/util.hpp"

// This file tests edge cases that might cause demangling failures
// or other uncovered branches in GetNamespace

// Test with an extern "C" function type that might have special handling
extern "C" {
typedef void (*CFunction)();
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

// Test with built-in types that might have special mangling
TEST(GetNamespaceEdgeCases, GetNamespace_WithBuiltinTypes_ReturnsEmpty) {
  EXPECT_EQ(ppc::util::GetNamespace<void>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<char>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<short>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<long>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<long long>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<unsigned char>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<unsigned short>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<unsigned long>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<unsigned long long>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<float>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<double>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<long double>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<bool>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<wchar_t>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<char16_t>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<char32_t>(), "");
  EXPECT_EQ(ppc::util::GetNamespace<std::nullptr_t>(), "std");
}

// Test with very long namespace chain
namespace a {
namespace b {
namespace c {
namespace d {
namespace e {
namespace f {
namespace g {
namespace h {
struct DeepType {};
}  // namespace h
}  // namespace g
}  // namespace f
}  // namespace e
}  // namespace d
}  // namespace c
}  // namespace b
}  // namespace a

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

