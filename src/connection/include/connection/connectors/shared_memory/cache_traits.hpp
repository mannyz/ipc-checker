#pragma once

#include <new>

#ifdef __cpp_lib_hardware_interference_size
using std::hardware_constructive_interference_size;
using std::hardware_destructive_interference_size;
#else
// Define cache line sizes based on the architecture
#if defined(__x86_64__) || defined(_M_X64)
// 64 bytes on x86-64 │ L1_CACHE_BYTES │ L1_CACHE_SHIFT │ __cacheline_aligned
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#elif defined(__arm__) || defined(__aarch64__)
// 64 bytes on ARM64 (AArch64)
constexpr std::size_t hardware_constructive_interference_size = 64;
constexpr std::size_t hardware_destructive_interference_size = 64;
#else
// if the architecture is unknown
static_assert(false, "architecture is unknown");
#endif
#endif
